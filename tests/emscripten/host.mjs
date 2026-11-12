#!/usr/bin/env node
import { createHash } from 'node:crypto';
import { readFile, writeFile, stat } from 'node:fs/promises';
import { dirname, resolve, sep } from 'node:path';
import { performance } from 'node:perf_hooks';
import { pathToFileURL } from 'node:url';

const EXPECTED_SCENARIOS = [
  ['warmup', 3], ['neutral', 3], ['port1_up', 3], ['continuation', 2], ['port2_up', 3],
];
const REQUIRED_FACTORY_EXPORTS = [
  '_retro_api_version', '_retro_get_system_info', '_retro_get_system_av_info',
  '_retro_set_environment', '_retro_set_video_refresh', '_retro_set_audio_sample',
  '_retro_set_audio_sample_batch', '_retro_set_input_poll', '_retro_set_input_state',
  '_retro_init', '_retro_deinit', '_retro_run', '_retro_reset', '_retro_load_game',
  '_retro_unload_game', '_retro_serialize_size', '_retro_serialize', '_retro_unserialize',
  '_retro_get_memory_data', '_retro_get_memory_size', '_retro_get_region',
  '_retro_set_controller_port_device', '_malloc', '_free',
];
const REQUIRED_RUNTIME_EXPORTS = [
  ['addFunction', 'function'], ['removeFunction', 'function'],
  ['HEAPU8', 'uint8array'], ['HEAPU32', 'uint32array'],
];
const WIDTH = 320;
const HEIGHT = 242;
const LAST_ENTRY = 0x841;
const fail = (message) => { throw new Error(message); };
const check = (condition, message) => { if (!condition) fail(message); };
const sha256 = (bytes) => createHash('sha256').update(bytes).digest('hex');

function parseArgs(argv) {
  const options = {};
  for (let i = 0; i < argv.length; i++) {
    const key = argv[i];
    if (key === '--help') {
      process.stdout.write('host.mjs --module PATH --fixture PATH --native PATH --mode tracer|verify|controls|report [--report PATH]\n');
      process.exit(0);
    }
    check(['--module', '--fixture', '--native', '--mode', '--report'].includes(key), `unknown argument: ${key}`);
    check(i + 1 < argv.length && !argv[i + 1].startsWith('--'), `missing value for ${key}`);
    options[key.slice(2)] = argv[++i];
  }
  for (const required of ['module', 'fixture', 'native', 'mode']) check(options[required], `missing --${required}`);
  check(['tracer', 'verify', 'controls', 'report'].includes(options.mode), `unsupported mode: ${options.mode}`);
  options.module = resolve(options.module);
  options.fixture = resolve(options.fixture);
  options.native = resolve(options.native);
  options.report = resolve(options.report ?? `${dirname(options.native)}/measurement.json`);
  return options;
}

async function loadEvidence(options) {
  const native = JSON.parse(await readFile(options.native, 'utf8'));
  const fixture = await readFile(options.fixture);
  const wasmPath = options.module.replace(/\.mjs$/u, '.wasm');
  check(wasmPath !== options.module, 'production loader must use the .mjs suffix');
  const wasm = await readFile(wasmPath);
  check(native.schema === 1 || native.schema === 2, 'native observation schema must be 1 or 2');
  check(native.fixture_sha256 === sha256(fixture), 'fixture SHA-256 differs from native observation');
  const sourcePath = `${dirname(options.module)}${sep}muhtari-source-id.txt`;
  const sourceIdentity = (await readFile(sourcePath, 'utf8')).trim();
  check(/^[0-9a-f]{64}$/u.test(sourceIdentity), 'production source identity is invalid');
  check(native.source_identity === sourceIdentity, 'native observation source identity differs from production build');
  if (native.schema === 2) {
    validateYmNative(native, fixture, sourceIdentity);
    return { native, fixture, wasm: new Uint8Array(wasm), wasmPath, sourceIdentity, ym: true };
  }
  check(native.numeric_oracles?.width === WIDTH && native.numeric_oracles?.height === HEIGHT,
        'native numeric oracle must establish 320x242 geometry');
  check(native.numeric_oracles.sample_rate === 48000, 'native numeric oracle must establish 48000 Hz');
  check(JSON.stringify(native.numeric_oracles.audio_counts) === '[800,801]', 'native exact NTSC audio bounds are missing');
  check(native.numeric_oracles.swcha?.neutral === 255 && native.numeric_oracles.swcha?.port1_up === 239 &&
        native.numeric_oracles.swcha?.port2_up === 254, 'native numeric SWCHA oracle is incomplete');
  check(Array.isArray(native.scenarios) && native.scenarios.length === EXPECTED_SCENARIOS.length,
        'incomplete native scenario set');
  for (let i = 0; i < EXPECTED_SCENARIOS.length; i++) {
    const [name, count] = EXPECTED_SCENARIOS[i];
    const scenario = native.scenarios[i];
    check(scenario?.index === i && scenario.name === name, `missing or reordered native scenario: ${name}`);
    check(Array.isArray(scenario.frames) && scenario.frames.length === count, `incomplete native scenario: ${name}`);
    check(scenario.settled_swcha === ({ warmup: 255, neutral: 255, port1_up: 239, continuation: 255, port2_up: 254 })[name],
          `changed numeric input oracle: ${name}`);
    for (let frame = 0; frame < count; frame++) {
      const observation = scenario.frames[frame];
      check(observation?.index === frame, `missing or duplicate frame ${frame} in ${name}`);
      check(Number.isInteger(observation.swcha) && Number.isInteger(observation.audio_count),
            `native numeric observation is incomplete: ${name} frame ${frame}`);
      check(/^[0-9a-f]{64}$/u.test(observation.video_sha256) && /^[0-9a-f]{64}$/u.test(observation.audio_sha256),
            `native frame hashes are incomplete: ${name} frame ${frame}`);
      if (name !== 'warmup' || frame > 0) {
        check(native.numeric_oracles.audio_counts.includes(observation.audio_count),
              `native steady audio count is out of range: ${name} frame ${frame}`);
      }
    }
  }
  check(native.state?.refused_null_short_long_corrupt === true && native.state?.preserved_after_refusal === true,
        'native state refusal and preservation controls are missing');
  check(native.content_refusal?.initial_invalid_rejected === true &&
        native.content_refusal?.loaded_transition_rejected === true &&
        native.content_refusal?.old_game_released === true &&
        native.content_refusal?.loaded_library_invalid_preserved === true,
        'native malformed-content refusal controls are missing');
  return { native, fixture, wasm: new Uint8Array(wasm), wasmPath, sourceIdentity };
}

function assertRequiredExports(module) {
  for (const name of REQUIRED_FACTORY_EXPORTS) {
    check(typeof module[name] === 'function', `missing required export: ${name}`);
  }
  for (const [name, kind] of REQUIRED_RUNTIME_EXPORTS) {
    const valid = kind === 'function'
      ? typeof module[name] === 'function'
      : kind === 'uint8array'
        ? module[name] instanceof Uint8Array
        : module[name] instanceof Uint32Array;
    check(valid, `missing required runtime export: ${name}`);
  }
}

function validateCapture(capture, observation, nativeFrame, scenarioName) {
  check(capture.videoCalls === 1 && capture.video && capture.width === WIDTH && capture.height === HEIGHT,
        'missing video callback or changed numeric geometry');
  check(capture.batchCalls === 1 && capture.audio && capture.pollCalls === 1,
        'missing audio batch or input poll callback');
  check(capture.sampleCalls === 0, 'unexpected per-sample audio callback');
  if (nativeFrame) {
    check(observation.swcha === nativeFrame.swcha,
          `SWCHA mismatch in ${scenarioName}: ${observation.swcha} != ${nativeFrame.swcha}`);
    check(capture.audioCount === nativeFrame.audio_count,
          `exact audio count mismatch in ${scenarioName}: ${capture.audioCount} != ${nativeFrame.audio_count}`);
  }
  const steady = scenarioName !== 'warmup' || (nativeFrame && nativeFrame.index > 0);
  if (steady) check([800, 801].includes(capture.audioCount), `NTSC audio count out of range in ${scenarioName}`);
  if (nativeFrame) {
    check(observation.videoSha256 === nativeFrame.video_sha256,
          `video hash mismatch in ${scenarioName} frame ${nativeFrame.index}`);
    check(observation.audioSha256 === nativeFrame.audio_sha256,
          `audio hash mismatch in ${scenarioName} frame ${nativeFrame.index}`);
  }
}

class Core {
  constructor(module, evidence) {
    assertRequiredExports(module);
    this.module = module;
    this.native = evidence.native;
    this.capture = null;
    this.pads = [0, 0];
    this.env = this.callback((command, data) => this.environment(command, data), 'iii');
    this.video = this.callback((data, width, height, pitch) => this.videoRefresh(data, width, height, pitch), 'viiii');
    this.sample = this.callback((left, right) => {
      if (!this.capture) return;
      const c = this.capture;
      c.sampleCalls++;
      if (c.audioCount >= 1024) { c.error = 'sample callback exceeded capture'; return; }
      c.audio ??= Buffer.alloc(4096);
      c.audio.writeInt16LE((left << 16) >> 16, c.audioCount * 4);
      c.audio.writeInt16LE((right << 16) >> 16, c.audioCount * 4 + 2);
      c.audioCount++;
    }, 'vii');
    this.batch = this.callback((data, frames) => this.audioBatch(data, frames), 'iii');
    this.poll = this.callback(() => { if (this.capture) this.capture.pollCalls++; }, 'v');
    this.input = this.callback((port, device, index, id) => this.inputState(port, device, index, id), 'iiiii');
    module._retro_set_environment(this.env);
    module._retro_set_video_refresh(this.video);
    module._retro_set_audio_sample(this.sample);
    module._retro_set_audio_sample_batch(this.batch);
    module._retro_set_input_poll(this.poll);
    module._retro_set_input_state(this.input);
    check(module._retro_api_version() === 1, 'unexpected libretro API version');
    module._retro_init();
    module._retro_set_controller_port_device(0, this.native.abi.device_joypad);
    module._retro_set_controller_port_device(1, this.native.abi.device_joypad);
    this.started = true;
  }

  callback(fn, signature) { return this.module.addFunction(fn, signature); }

  environment(command, data) {
    const env = this.native.abi.environment;
    const { HEAPU8, HEAPU32 } = this.module;
    switch (command) {
      case env.get_core_options_version:
        if (data) HEAPU32[data >>> 2] = 1;
        return data ? 1 : 0;
      case env.set_core_options:
      case env.set_support_no_game:
      case env.set_controller_info:
      case env.set_input_descriptors:
      case env.set_memory_maps:
      case env.set_support_achievements:
      case env.set_message:
        return 1;
      case env.get_variable: {
        if (!data) return 0;
        const keyPointer = HEAPU32[data >>> 2];
        if (!keyPointer) return 0;
        const key = this.readCString(keyPointer);
        const value = this.optionValue(key);
        if (value === null) return 0;
        const valuePointer = this.cString(value);
        this.ownedStrings.push(valuePointer);
        this.module.HEAPU32[(data + 4) >>> 2] = valuePointer;
        return 1;
      }
      case env.get_variable_update:
        if (data) HEAPU8[data] = 0;
        return data ? 1 : 0;
      case env.get_savestate_context:
        if (data) HEAPU32[data >>> 2] = 0;
        return data ? 1 : 0;
      case env.set_pixel_format:
        return data && HEAPU32[data >>> 2] === this.native.abi.pixel_format_xrgb8888 ? 1 : 0;
      default:
        return 0;
    }
  }

  get ownedStrings() { return this._ownedStrings ?? (this._ownedStrings = []); }

  optionValue(key) {
    if (key === 'muhtari_region' && this.ymRegion) return this.ymRegion;
    if (key === 'muhtari_boot') return 'fast';
    if (['muhtari_palette', 'muhtari_region', 'muhtari_hsc', 'muhtari_expansion',
         'muhtari_battery_port1', 'muhtari_battery_port2'].includes(key)) return 'auto';
    if (['muhtari_left_difficulty', 'muhtari_right_difficulty'].includes(key)) return 'B';
    return null;
  }

  readCString(pointer) {
    const bytes = [];
    const heap = this.module.HEAPU8;
    for (let i = 0; i < 256 && pointer + i < heap.length; i++) {
      const byte = heap[pointer + i];
      if (byte === 0) return Buffer.from(bytes).toString('utf8');
      bytes.push(byte);
    }
    fail('unterminated or oversized libretro environment string');
  }

  cString(value) {
    const bytes = Buffer.from(`${value}\0`, 'utf8');
    const pointer = this.module._malloc(bytes.length);
    check(pointer !== 0, 'cannot allocate libretro option string');
    this.module.HEAPU8.set(bytes, pointer);
    return pointer;
  }

  setCallbacks() { /* callbacks are installed before retro_init */ }

  startGame(fixture, rejectTruncated = false) {
    if (rejectTruncated) {
      const bad = this.makeGameInfo(fixture.subarray(0, 16));
      let rejected;
      try { rejected = this.module._retro_load_game(bad.info) === 0; }
      finally { this.module._free(bad.data); this.module._free(bad.info); }
      check(rejected, 'invalid initial image was accepted');
    }
    const game = this.makeGameInfo(fixture);
    let loaded;
    try { loaded = this.module._retro_load_game(game.info) !== 0; }
    finally { this.module._free(game.data); this.module._free(game.info); }
    check(loaded, 'production libretro core refused generated input fixture');
    const badTransition = this.makeGameInfo(fixture.subarray(0, 16));
    let transitionRejected;
    try { transitionRejected = this.module._retro_load_game(badTransition.info) === 0; }
    finally { this.module._free(badTransition.data); this.module._free(badTransition.info); }
    check(transitionRejected, 'invalid loaded transition was accepted');
    check(this.module._retro_serialize_size() === 0 &&
          this.module._retro_get_memory_data(this.native.abi.memory_system_ram) === 0,
          'malformed loaded transition did not release old game state');
    const restored = this.makeGameInfo(fixture);
    let reloaded;
    try { reloaded = this.module._retro_load_game(restored.info) !== 0; }
    finally { this.module._free(restored.data); this.module._free(restored.info); }
    check(reloaded, 'production libretro core refused fixture reload after malformed transition');
    const av = this.allocate(40, 'system AV information');
    try {
      this.module._retro_get_system_av_info(av);
      check(this.module.HEAPU32[av >>> 2] === WIDTH && this.module.HEAPU32[(av + 4) >>> 2] === (this.ymRegion === 'pal' ? 292 : HEIGHT),
            'production libretro geometry is not 320x242');
    } finally { this.module._free(av); }
  }

  makeGameInfo(fixture) {
    const data = this.allocate(fixture.byteLength, 'input fixture');
    const info = this.allocate(16, 'wasm32 retro_game_info');
    this.module.HEAPU8.set(fixture, data);
    this.module.HEAPU8.fill(0, info, info + 16);
    // wasm32 retro_game_info: path@0, data@4, size@8, meta@12.
    this.module.HEAPU32[(info + 4) >>> 2] = data;
    this.module.HEAPU32[(info + 8) >>> 2] = fixture.byteLength;
    return { data, info };
  }

  allocate(bytes, name) {
    const pointer = this.module._malloc(bytes);
    check(pointer !== 0, `cannot allocate ${name}`);
    return pointer;
  }

  videoRefresh(data, width, height, pitch) {
    const capture = this.capture;
    if (!capture) return;
    capture.videoCalls++;
    const expectedHeight = this.ymRegion === 'pal' ? 292 : HEIGHT;
    if (!data || width !== WIDTH || height !== expectedHeight || pitch < WIDTH * 4) {
      capture.error = 'video callback geometry or data is invalid';
      return;
    }
    const heap = this.module.HEAPU8;
    const bytes = Buffer.allocUnsafe(WIDTH * height * 4);
    for (let row = 0; row < height; row++) {
      const start = data + row * pitch;
      if (start + WIDTH * 4 > heap.length) { capture.error = 'video callback exceeded current wasm heap'; return; }
      bytes.set(heap.subarray(start, start + WIDTH * 4), row * WIDTH * 4);
    }
    capture.video = bytes;
    capture.width = width;
    capture.height = height;
  }

  audioBatch(data, frames) {
    const capture = this.capture;
    if (!capture) return 0;
    capture.batchCalls++;
    if (!data || frames > 1024) { capture.error = 'audio batch pointer or frame count is invalid'; return 0; }
    const bytes = frames * 4;
    if (data + bytes > this.module.HEAPU8.length) { capture.error = 'audio batch exceeded current wasm heap'; return 0; }
    capture.audio = Buffer.from(this.module.HEAPU8.subarray(data, data + bytes));
    capture.audioCount = frames;
    return frames;
  }

  inputState(port, device, index, id) {
    if (port >= 2 || device !== this.native.abi.device_joypad || index !== 0 || id >= 16) return 0;
    return (this.pads[port] >>> id) & 1;
  }

  readSwcha() {
    const pointer = this.module._retro_get_memory_data(this.native.abi.memory_system_ram);
    const size = this.module._retro_get_memory_size(this.native.abi.memory_system_ram);
    check(pointer !== 0 && size > LAST_ENTRY + 8, 'system RAM input observation is unavailable');
    const heap = this.module.HEAPU8;
    check(pointer + size <= heap.length, 'system RAM exceeds current wasm heap');
    const entry = heap[pointer + LAST_ENTRY];
    check(entry + 8 <= size, 'input trace entry is outside system RAM');
    return heap[pointer + entry];
  }

  runFrame(firstPad, secondPad, nativeFrame, scenarioName) {
    this.pads = [firstPad, secondPad];
    const capture = { videoCalls: 0, batchCalls: 0, sampleCalls: 0, pollCalls: 0, audioCount: 0, error: null };
    this.capture = capture;
    this.module._retro_run();
    this.capture = null;
    if (capture.error) fail(capture.error);
    check(capture.videoCalls === 1 && capture.video && capture.width === WIDTH && capture.height === HEIGHT,
          'missing video callback or changed numeric geometry');
    check(capture.batchCalls === 1 && capture.audio && capture.pollCalls === 1,
          'missing audio batch or input poll callback');
    const swcha = this.readSwcha();
    const observation = {
      swcha,
      audioCount: capture.audioCount,
      videoSha256: sha256(capture.video),
      audioSha256: sha256(capture.audio),
    };
    validateCapture(capture, observation, nativeFrame, scenarioName);
    return observation;
  }

  runScenario(scenario, nativeScenario) {
    const output = [];
    let first = 0, second = 0;
    if (scenario === 'port1_up') first = 1 << this.native.abi.joypad_up;
    if (scenario === 'port2_up') second = 1 << this.native.abi.joypad_up;
    for (let i = 0; i < nativeScenario.frames.length; i++) {
      output.push(this.runFrame(first, second, nativeScenario.frames[i], scenario));
    }
    const last = output.at(-1);
    check(last.swcha === nativeScenario.settled_swcha, `settled SWCHA mismatch in ${scenario}`);
    if (scenario === 'port1_up') check(last.swcha === this.native.numeric_oracles.swcha.port1_up, 'port-1 Up numeric oracle failed');
    if (scenario === 'port2_up') check(last.swcha === this.native.numeric_oracles.swcha.port2_up, 'port-2 Up numeric oracle failed');
    if (scenario === 'neutral') check(last.swcha === this.native.numeric_oracles.swcha.neutral, 'neutral numeric oracle failed');
    return output;
  }

  serialize() {
    const size = this.module._retro_serialize_size();
    check(size > 0 && size <= 310402, `invalid loaded save-state size: ${size}`);
    const pointer = this.allocate(size, 'save state');
    try {
      check(this.module._retro_serialize(pointer, size) !== 0, 'production core refused state serialization');
      return Buffer.from(this.module.HEAPU8.subarray(pointer, pointer + size));
    } finally { this.module._free(pointer); }
  }

  unserialize(bytes, length = bytes.length) {
    const pointer = bytes === null ? 0 : this.allocate(bytes.length, 'state input');
    try {
      if (bytes !== null) this.module.HEAPU8.set(bytes, pointer);
      return this.module._retro_unserialize(pointer, length) !== 0;
    } finally { if (pointer) this.module._free(pointer); }
  }

  assertRefusals(saved) {
    const unchanged = () => check(sha256(this.serialize()) === sha256(saved), 'refused state changed live state');
    check(!this.unserialize(null, saved.length), 'null state was accepted'); unchanged();
    check(!this.unserialize(saved, 0), 'empty state was accepted'); unchanged();
    check(!this.unserialize(saved, saved.length - 1), 'short state was accepted'); unchanged();
    const longer = Buffer.concat([saved, Buffer.from([0])]);
    check(!this.unserialize(longer), 'long state was accepted'); unchanged();
    const corrupt = Buffer.from(saved);
    corrupt[Math.floor(corrupt.length / 2)] ^= 0x80;
    check(!this.unserialize(corrupt), 'corrupt state was accepted'); unchanged();
  }

  async isolatedFactory(evidence, factory) {
    const secondModule = await factory({ wasmBinary: evidence.wasm });
    const second = new Core(secondModule, evidence);
    try {
      second.startGame(evidence.fixture, true);
      for (let i = 0; i < 3; i++) second.runFrame(0, 0, this.native.scenarios[0].frames[i], 'warmup');
      const secondSaved = second.serialize();
      this.runFrame(1 << this.native.abi.joypad_up, 0, null, 'port1_up');
      const firstSaved = this.serialize();
      check(sha256(second.serialize()) === sha256(secondSaved), 'interleaved first-instance run changed second-instance state');
      check(second.unserialize(secondSaved), 'second instance refused its own saved state');
      check(sha256(this.serialize()) === sha256(firstSaved), 'second-instance restore changed first-instance state');
      check(sha256(second.serialize()) === sha256(secondSaved), 'second-instance restoration changed its state');
    } finally { second.dispose(); }
  }

  dispose() {
    if (!this.started) return;
    this.module._retro_unload_game();
    this.module._retro_deinit();
    this.started = false;
    for (const pointer of this.ownedStrings) this.module._free(pointer);
    for (const pointer of [this.env, this.video, this.sample, this.batch, this.poll, this.input]) {
      this.module.removeFunction(pointer);
    }
  }
}

async function importFactory(options, evidence) {
  const { default: factory } = await import(pathToFileURL(options.module).href);
  check(typeof factory === 'function', 'production ES module did not export a factory');
  const module = await factory({ wasmBinary: evidence.wasm });
  assertRequiredExports(module);
  const compiled = new WebAssembly.Module(evidence.wasm);
  const surface = {
    imports: WebAssembly.Module.imports(compiled),
    exports: WebAssembly.Module.exports(compiled),
    requiredFactoryExports: REQUIRED_FACTORY_EXPORTS,
    requiredRuntimeExports: REQUIRED_RUNTIME_EXPORTS.map(([name]) => name),
  };
  const surfacePath = `${dirname(options.native)}${sep}wasm-surface.json`;
  await writeFile(surfacePath, `${JSON.stringify(surface, null, 2)}\n`);
  return { factory, module };
}

function assertScenarioSet(scenarios) {
  check(Array.isArray(scenarios) && scenarios.length === EXPECTED_SCENARIOS.length, 'incomplete observation scenario set');
  for (let i = 0; i < EXPECTED_SCENARIOS.length; i++) {
    check(scenarios[i]?.index === i && scenarios[i]?.name === EXPECTED_SCENARIOS[i][0],
          `incomplete observation scenario set: ${EXPECTED_SCENARIOS[i][0]}`);
  }
}

async function runVerification(options, evidence, factory, mode) {
  const module = await factory({ wasmBinary: evidence.wasm });
  const core = new Core(module, evidence);
  const observed = [];
  try {
    core.startGame(evidence.fixture, true);
    for (const name of ['warmup', 'neutral', 'port1_up']) {
      const scenario = evidence.native.scenarios.find((item) => item.name === name);
      observed.push(...core.runScenario(name, scenario));
    }
    if (mode === 'tracer') return { core, observed };

    const saved = core.serialize();
    check(saved.length === evidence.native.state.size, 'save-state byte length differs from native adapter');
    check(sha256(saved) === evidence.native.state.saved_sha256, 'save-state hash differs from native adapter');
    core.assertRefusals(saved);

    const continuation = evidence.native.scenarios.find((item) => item.name === 'continuation');
    const continuationFrames = core.runScenario('continuation', continuation);
    observed.push(...continuationFrames);
    const continuedState = core.serialize();
    check(sha256(continuedState) === evidence.native.state.continuation_sha256,
          'continuation state hash differs from native adapter');
    for (let replay = 0; replay < 2; replay++) {
      check(core.unserialize(saved), `restore failed for continuation replay ${replay}`);
      const replayFrames = core.runScenario('continuation', continuation);
      for (let i = 0; i < replayFrames.length; i++) {
        check(JSON.stringify(replayFrames[i]) === JSON.stringify(continuationFrames[i]),
              `continuation replay ${replay} frame ${i} differs from uninterrupted result`);
      }
      check(sha256(core.serialize()) === sha256(continuedState), `continuation replay ${replay} state differs from uninterrupted result`);
    }
    check(core.unserialize(saved), 'restore failed before port-2 input scenario');
    const port2 = core.runScenario('port2_up', evidence.native.scenarios.find((item) => item.name === 'port2_up'));
    observed.push(...port2);

    await core.isolatedFactory(evidence, factory);
    core.module._retro_unload_game();
    check(core.module._retro_load_game(0) !== 0, 'production core refused no-game load');
    check(core.module._retro_serialize_size() === 0, 'no-game serialize size must be zero');
    const sentinel = core.allocate(16, 'no-game sentinel');
    try {
      core.module.HEAPU8.fill(0xa5, sentinel, sentinel + 16);
      check(core.module._retro_serialize(sentinel, 16) === 0, 'no-game serialization was accepted');
      const rejectedState = core.allocate(saved.length, 'no-game rejected state');
      try {
        core.module.HEAPU8.set(saved, rejectedState);
        check(core.module._retro_unserialize(rejectedState, saved.length) === 0,
              'no-game state restoration was accepted');
      } finally { core.module._free(rejectedState); }
      check(core.module.HEAPU8.subarray(sentinel, sentinel + 16).every((byte) => byte === 0xa5),
            'no-game refusal changed output storage');
    } finally { core.module._free(sentinel); }
    const noGameFrame = { videoCalls: 0, batchCalls: 0, sampleCalls: 0, pollCalls: 0, audioCount: 0, error: null };
    core.capture = noGameFrame;
    core.module._retro_run();
    core.capture = null;
    check(noGameFrame.videoCalls === evidence.native.no_game.video_calls &&
          noGameFrame.batchCalls === evidence.native.no_game.audio_calls &&
          noGameFrame.pollCalls === 1 && noGameFrame.audioCount === evidence.native.no_game.audio_count,
          'no-game callback lifecycle differs from native observation');
    check(noGameFrame.error === null && noGameFrame.width === WIDTH && noGameFrame.height === HEIGHT,
          'no-game video callback geometry differs from native observation');
    if (mode === 'verify') process.stdout.write(`offline libretro verification passed (${observed.length} parity frames, state replay and instance isolation)\n`);
    return { core, observed };
  } catch (error) {
    core.dispose();
    throw error;
  }
}

async function runControls(options, evidence, factory) {
  const result = await runVerification(options, evidence, factory, 'verify');
  result.core.dispose();
  const clean = EXPECTED_SCENARIOS.map(([name], index) => ({ index, name }));
  assertScenarioSet(clean);
  const nativeFrame = evidence.native.scenarios[1].frames[0];
  const capture = { videoCalls: 1, batchCalls: 1, sampleCalls: 0, pollCalls: 1,
    video: Buffer.alloc(4), audio: Buffer.alloc(4), width: WIDTH, height: HEIGHT,
    audioCount: nativeFrame.audio_count };
  const observation = { swcha: nativeFrame.swcha, audioCount: nativeFrame.audio_count,
    videoSha256: nativeFrame.video_sha256, audioSha256: nativeFrame.audio_sha256 };
  const controls = [
    ['missing video callback', () => validateCapture({ ...capture, videoCalls: 0 }, observation, nativeFrame, 'neutral')],
    ['video hash mismatch', () => validateCapture(capture, { ...observation, videoSha256: 'wrong' }, nativeFrame, 'neutral')],
    ['incomplete observation scenario set', () => assertScenarioSet(clean.slice(0, -1))],
  ];
  for (const name of REQUIRED_FACTORY_EXPORTS) {
    controls.push([`missing required export: ${name}`, () => assertRequiredExports(new Proxy(result.core.module, {
      get(target, key, receiver) { return key === name ? undefined : Reflect.get(target, key, receiver); },
    }))]);
  }
  for (const [name] of REQUIRED_RUNTIME_EXPORTS) {
    controls.push([`missing required runtime export: ${name}`, () => assertRequiredExports(new Proxy(result.core.module, {
      get(target, key, receiver) { return key === name ? undefined : Reflect.get(target, key, receiver); },
    }))]);
  }
  for (const [expected, invoke] of controls) {
    let message = '';
    try { invoke(); } catch (error) { message = String(error.message); }
    check(message.includes(expected), `fault control did not report named cause: ${expected}`);
    process.stdout.write(`fault control rejected: ${expected}\n`);
  }
}

async function writeMeasurement(options, evidence, result, factory) {
  const native = evidence.native.performance;
  check(native?.warmup_frames === 12 && native?.frames_per_window === 60 &&
        Array.isArray(native.windows) && native.windows.length === 3,
        'native fixed-workload performance receipt is missing or incomplete');
  const module = await factory({ wasmBinary: evidence.wasm });
  const core = new Core(module, evidence);
  const windows = [];
  try {
    core.startGame(evidence.fixture);
    for (let i = 0; i < 12; i++) core.runFrame(0, 0, null, 'warmup');
    const baseline = core.serialize();
    for (let window = 0; window < 3; window++) {
      check(core.unserialize(baseline), `cannot restore Node performance baseline for window ${window}`);
      const start = performance.now();
      for (let frame = 0; frame < 60; frame++) core.runFrame(0, 0, null, 'neutral');
      const seconds = (performance.now() - start) / 1000;
      check(Number.isFinite(seconds) && seconds > 0, `invalid Node performance duration in window ${window}`);
      windows.push({ elapsed_seconds: seconds, frames_per_second: 60 / seconds });
    }
  } finally { core.dispose(); }
  const median = (items) => [...items].sort((a, b) => a - b)[Math.floor(items.length / 2)];
  const loaderBytes = (await stat(options.module)).size;
  const wasmBytes = (await stat(evidence.wasmPath)).size;
  const report = {
    schema: 1,
    source_identity: evidence.sourceIdentity,
    fixture_sha256: evidence.native.fixture_sha256,
    artifacts: { loader_bytes: loaderBytes, wasm_bytes: wasmBytes, combined_bytes: loaderBytes + wasmBytes },
    workload: { warmup_frames: 12, frames_per_window: 60, windows: 3, input: 'neutral', callback_copy_and_sha256: true },
    native: { toolchain: evidence.native.toolchain, windows: native.windows, median_frames_per_second: native.median_frames_per_second },
    node: { version: process.version, platform: `${process.platform}/${process.arch}`, windows,
      median_frames_per_second: median(windows.map((item) => item.frames_per_second)) },
    sdk: {
      image_index: 'emscripten/emsdk@sha256:6bd7aef93408b10bf7b36642cb7c91119b302f720b9ec2d20439f73cf07fe894',
      selected_platform: process.env.MUHTARI_SDK_PLATFORM ?? 'linux/amd64',
      selected_leaf_digest: 'sha256:3803e37bf748ddee14d31cf712fcab09ca367fca83bbefeb78ce4c1f284e6f3f',
      host: process.env.MUHTARI_HOST_DESCRIPTION ?? 'container identity recorded; host details unavailable',
    },
    limits: ['native and Node medians are descriptive and have no pass threshold',
      'results describe this container/host only; no browser or device behavior is inferred'],
  };
  await writeFile(options.report, `${JSON.stringify(report, null, 2)}\n`);
  process.stdout.write(`wrote fixed-workload measurement to ${options.report}\n`);
}


const YM_FIXTURES = {
  ordinary: '65c25495be4a0d73ae83a9f3f742186ffda34bff4058c3a5bc304bed428258d6',
  worst: '89a0f03290d2442379991c661527ff83e6072fb6e0dd69896b936f64a4aafbd2',
};
const validHash = (value) => typeof value === 'string' && /^[0-9a-f]{64}$/u.test(value);

function validateYmNative(native, fixture, sourceIdentity) {
  check(native.schema === 2 && YM_FIXTURES[native.workload] === sha256(fixture) &&
        native.fixture_sha256 === sha256(fixture), 'wrong YM workload or fixture');
  check(validHash(native.source_identity) && native.source_identity === sourceIdentity, 'wrong YM source identity');
  check(Array.isArray(native.regions) && native.regions.length === 2, 'incomplete YM regions');
  for (let index = 0; index < 2; index++) {
    const region = native.regions[index];
    check(region.name === ['ntsc', 'pal'][index] && region.height === [242, 292][index] &&
          region.sample_rate === 48000, 'wrong YM region order or numbers');
    check(region.minimum < 0 && region.maximum > 0 && region.minimum >= -32768 &&
          region.maximum <= 32767, 'wrong YM signed PCM extrema');
    check(Array.isArray(region.mailbox) && region.mailbox.length === 4 &&
          region.mailbox[0] === 128 && region.mailbox[1] === 1 &&
          region.mailbox[2] > 0 && region.mailbox[3] === 2, 'wrong YM timer mailbox');
    check(region.state_bytes === 179330 && validHash(region.saved_sha256) &&
          validHash(region.continuation_sha256), 'wrong YM state extent or identity');
    check(Array.isArray(region.frames) && region.frames.length === 6, 'incomplete YM frames');
    for (let i = 0; i < 6; i++) {
      const f = region.frames[i];
      const allowed = i === 0 ? [index === 0 ? 54 : 960] : index === 0 ? [800, 801] : [961, 962];
      check(f.index === i && allowed.includes(f.audio_count) && validHash(f.video_sha256) &&
            validHash(f.audio_sha256), 'wrong YM frame extent or identity');
    }
    const perf = region.performance;
    check(perf?.warmup_frames === 12 && perf.frames_per_window === 60 &&
          perf.windows?.length === 3 && perf.windows.every((w) =>
            Number.isFinite(w.elapsed_seconds) && w.elapsed_seconds > 0 &&
            Number.isFinite(w.frames_per_second) && w.frames_per_second > 0 &&
            Math.abs(w.frames_per_second * w.elapsed_seconds - 60) < 0.001),
          'incomplete YM timing windows');
  }
}

function runYmFrame(core, nativeFrame, index, route = 'batch') {
  const c = { videoCalls: 0, batchCalls: 0, sampleCalls: 0, pollCalls: 0, audioCount: 0, error: null };
  core.capture = c;
  core.module._retro_run();
  core.capture = null;
  check(!c.error, c.error);
  const pal = core.ymRegion === 'pal';
  check(c.videoCalls === 1 && c.video && c.width === 320 && c.height === (pal ? 292 : 242) &&
        c.pollCalls === 1, 'missing YM video/poll callback');
  const allowed = index === 0 ? [pal ? 960 : 54] : pal ? [961, 962] : [800, 801];
  if (route === 'absent') {
    check(c.batchCalls === 0 && c.sampleCalls === 0 && c.audioCount === 0, 'unexpected absent YM audio callback');
  } else {
    check(allowed.includes(c.audioCount), 'wrong YM sample count');
    check(route === 'batch' ? c.batchCalls === 1 && c.sampleCalls === 0
                            : c.batchCalls === 0 && c.sampleCalls === c.audioCount,
          'missing YM audio callback');
    check(c.audio && c.audio.length >= c.audioCount * 4, 'short YM PCM capture');
    c.audio = c.audio.subarray(0, c.audioCount * 4);
    for (let i = 0; i < c.audioCount; i++)
      check(c.audio.readInt16LE(i * 4) === c.audio.readInt16LE(i * 4 + 2), 'YM duplicated mono differs');
  }
  const frame = { index, video_sha256: sha256(c.video), audio_count: c.audioCount,
    audio_sha256: c.audio ? sha256(c.audio) : null };
  if (nativeFrame) {
    check(frame.video_sha256 === nativeFrame.video_sha256, 'wrong YM video hash');
    if (route !== 'absent') check(frame.audio_count === nativeFrame.audio_count &&
      frame.audio_sha256 === nativeFrame.audio_sha256, 'wrong YM PCM hash or count');
  }
  return { frame, capture: c };
}

function repairStateCrc(state) {
  state.writeUInt32LE(0, 24);
  let crc = 0xffffffff;
  for (const byte of state) {
    crc ^= byte;
    for (let bit = 0; bit < 8; bit++) crc = (crc >>> 1) ^ ((crc & 1) ? 0xedb88320 : 0);
  }
  state.writeUInt32LE((crc ^ 0xffffffff) >>> 0, 24);
}

async function runYm(options, evidence, factory) {
  const measurements = [];
  for (const region of evidence.native.regions) {
    const core = new Core(await factory({ wasmBinary: evidence.wasm }), evidence);
    core.ymRegion = region.name;
    try {
      core.startGame(evidence.fixture, true);
      let saved;
      let minimum = 32767, maximum = -32768;
      for (let i = 0; i < 6; i++) {
        const { capture } = runYmFrame(core, region.frames[i], i);
        for (let j = 0; j < capture.audioCount; j++) {
          const sample = capture.audio.readInt16LE(j * 4);
          minimum = Math.min(minimum, sample); maximum = Math.max(maximum, sample);
        }
        if (i === 1) saved = core.serialize();
      }
      check(minimum === region.minimum && maximum === region.maximum, 'wrong YM numeric extrema');
      const ram = core.module._retro_get_memory_data(evidence.native.abi.memory_system_ram);
      const ramBytes = core.module._retro_get_memory_size(evidence.native.abi.memory_system_ram);
      check(ram && ramBytes >= 0xb00 && ram + ramBytes <= core.module.HEAPU8.length,
            'YM RAM exceeds heap');
      check(JSON.stringify([...core.module.HEAPU8.subarray(ram + 0xa00, ram + 0xa04)]) ===
            JSON.stringify(region.mailbox), 'wrong YM numeric mailbox');
      const ending = core.serialize();
      check(saved.length === region.state_bytes && saved.readUInt32LE(8) === 2 &&
            saved.readUInt32LE(20) === 7 && sha256(saved) === region.saved_sha256 &&
            sha256(ending) === region.continuation_sha256, 'wrong YM state bytes');
      core.assertRefusals(ending);
      const semantic = Buffer.from(ending);
      semantic[semantic.length - 1] = 2; // final YM initialized boolean
      repairStateCrc(semantic);
      check(!core.unserialize(semantic) && sha256(core.serialize()) === sha256(ending),
            'semantic YM state accepted or mutated live machine');
      const battery = core.module._retro_get_memory_data(0);
      const batterySize = core.module._retro_get_memory_size(0);
      check(battery && batterySize === 34816 && battery + batterySize <= core.module.HEAPU8.length,
            'YM battery extent unavailable');
      const batteryHash = sha256(core.module.HEAPU8.subarray(battery, battery + batterySize));
      const unsupported = Buffer.from(evidence.fixture); unsupported[67] |= 0x10;
      const bad = core.makeGameInfo(unsupported);
      try { check(core.module._retro_load_game(bad.info) === 0, 'unsupported YM reload accepted'); }
      finally { core.module._free(bad.data); core.module._free(bad.info); }
      check(sha256(core.serialize()) === sha256(ending) &&
            sha256(core.module.HEAPU8.subarray(battery, battery + batterySize)) === batteryHash,
            'refused YM reload changed state/battery');
      for (const route of ['batch', 'sample', 'absent']) {
        check(core.unserialize(saved), 'YM replay restore failed');
        core.module._retro_set_audio_sample_batch(route === 'batch' ? core.batch : 0);
        core.module._retro_set_audio_sample(route === 'absent' ? 0 : core.sample);
        for (let i = 2; i < 6; i++) runYmFrame(core, region.frames[i], i, route);
        check(sha256(core.serialize()) === sha256(ending), 'YM callback route changed continuation');
      }
      core.module._retro_set_audio_sample_batch(core.batch);
      core.module._retro_set_audio_sample(core.sample);
      const tail = core.allocate(ending.length + 16, 'YM serialization tail');
      try {
        core.module.HEAPU8.fill(0xa5, tail, tail + ending.length + 16);
        check(core.module._retro_serialize(tail, ending.length + 16) !== 0 &&
          core.module.HEAPU8.subarray(tail + ending.length, tail + ending.length + 16).every((x) => x === 0xa5),
          'YM serialize changed tail');
      } finally { core.module._free(tail); }
      core.module._retro_reset();
      runYmFrame(core, region.frames[0], 0);
      const second = new Core(await factory({ wasmBinary: evidence.wasm }), evidence);
      second.ymRegion = region.name;
      try {
        second.startGame(evidence.fixture);
        runYmFrame(second, region.frames[0], 0);
        const isolated = second.serialize();
        runYmFrame(core, region.frames[1], 1);
        check(sha256(second.serialize()) === sha256(isolated), 'YM factory instances share mutable state');
        const first = core.serialize();
        check(second.unserialize(isolated) && sha256(core.serialize()) === sha256(first),
              'YM second restore changed first instance');
      } finally { second.dispose(); }
      if (options.mode === 'report') {
        core.module._retro_reset();
        for (let i = 0; i < 12; i++) runYmFrame(core, null, i);
        const baseline = core.serialize();
        const windows = [];
        for (let w = 0; w < 3; w++) {
          check(core.unserialize(baseline), 'YM timing baseline restore failed');
          const start = performance.now();
          for (let i = 0; i < 60; i++) runYmFrame(core, null, i + 12);
          const seconds = (performance.now() - start) / 1000;
          check(Number.isFinite(seconds) && seconds > 0, 'invalid YM timing');
          windows.push({ elapsed_seconds: seconds, frames_per_second: 60 / seconds });
        }
        measurements.push({ name: region.name, native: region.performance,
          node: { warmup_frames: 12, frames_per_window: 60, windows } });
      }
    } finally { core.dispose(); }
  }
  if (options.mode === 'controls') {
    const faults = [
      ['wrong YM workload or fixture', (n) => { n.workload = 'unknown'; }],
      ['wrong YM source identity', (n) => { n.source_identity = 'a'.repeat(64); }],
      ['wrong YM workload or fixture', (n) => { n.fixture_sha256 = 'a'.repeat(64); }],
      ['incomplete YM regions', (n) => { n.regions.pop(); }],
      ['wrong YM region order or numbers', (n) => { n.regions.reverse(); }],
      ['incomplete YM frames', (n) => { n.regions[0].frames.pop(); }],
      ['wrong YM frame extent or identity', (n) => { n.regions[0].frames[0].audio_count++; }],
      ['wrong YM timer mailbox', (n) => { n.regions[0].mailbox[0] = 0; }],
      ['incomplete YM timing windows', (n) => { n.regions[1].performance.windows.pop(); }],
      ['incomplete YM timing windows', (n) => { n.regions[1].performance.windows[0].elapsed_seconds = 0; }],
    ];
    for (const [reason, mutate] of faults) {
      const n = structuredClone(evidence.native); mutate(n);
      let rejected = false;
      try { validateYmNative(n, evidence.fixture, evidence.sourceIdentity); } catch (error) { rejected = error.message.includes(reason); }
      check(rejected, `YM control did not reject: ${reason}`);
    }
    for (const name of REQUIRED_FACTORY_EXPORTS) {
      let rejected = false;
      try { assertRequiredExports(new Proxy({}, { get(_t, k) { return k === name ? undefined : () => {}; } })); }
      catch (error) { rejected = error.message.includes(`missing required export: ${name}`); }
      check(rejected, `YM export control failed: ${name}`);
    }
    const core = new Core(await factory({ wasmBinary: evidence.wasm }), evidence);
    core.ymRegion = 'ntsc';
    try {
      core.startGame(evidence.fixture);
      const n = evidence.native.regions[0].frames[0];
      core.module._retro_set_video_refresh(0);
      let rejected = false;
      try { runYmFrame(core, n, 0); } catch (error) { rejected = error.message.includes('missing YM video/poll callback'); }
      check(rejected, 'missing YM video callback accepted');
      core.module._retro_set_video_refresh(core.video);
      core.module._retro_reset();
      const wrong = { ...n, audio_sha256: 'a'.repeat(64) };
      rejected = false;
      try { runYmFrame(core, wrong, 0); } catch (error) { rejected = error.message.includes('wrong YM PCM hash or count'); }
      check(rejected, 'wrong YM PCM accepted');
    } finally { core.dispose(); }
  }
  if (options.mode === 'report') {
    const loader = await readFile(options.module);
    await writeFile(options.report, `${JSON.stringify({ schema: 2, workload: evidence.native.workload,
      fixture_sha256: evidence.native.fixture_sha256, source_identity: evidence.sourceIdentity,
      loader_sha256: sha256(loader), wasm_sha256: sha256(evidence.wasm),
      artifacts: { loader_bytes: loader.length, wasm_bytes: evidence.wasm.length },
      native_toolchain: evidence.native.toolchain,
      node: { version: process.version, platform: `${process.platform}/${process.arch}` },
      sdk: { image_index: 'emscripten/emsdk@sha256:6bd7aef93408b10bf7b36642cb7c91119b302f720b9ec2d20439f73cf07fe894',
        host: process.env.MUHTARI_HOST_DESCRIPTION ?? 'unavailable' },
      workload_definition: { callback_copy_and_sha256: true, input: 'neutral' }, regions: measurements }, null, 2)}\n`);
  }
  process.stdout.write(`YM ${evidence.native.workload} ${options.mode} passed (both regions, actual Wasm)\n`);
}

async function main() {
  const options = parseArgs(process.argv.slice(2));
  const evidence = await loadEvidence(options);
  const { factory, module } = await importFactory(options, evidence);
  if (evidence.ym) return runYm(options, evidence, factory);
  if (options.mode === 'controls') return runControls(options, evidence, factory);
  if (options.mode === 'report') {
    const result = await runVerification(options, evidence, factory, 'verify');
    await writeMeasurement(options, evidence, result, factory);
    result.core.dispose();
    return;
  }
  const result = await runVerification(options, evidence, factory, options.mode);
  process.stdout.write(`production artifact ${options.mode} passed (${result.observed.length} parity frames)\n`);
  result.core.dispose();
}

main().catch((error) => {
  process.stderr.write(`emscripten host: ${error.stack ?? error}\n`);
  process.exitCode = 1;
});
