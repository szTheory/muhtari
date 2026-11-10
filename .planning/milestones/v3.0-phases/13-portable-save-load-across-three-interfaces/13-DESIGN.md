# Phase 13 execution design and source coverage

This is the planner-owned byte/API contract, not implementation evidence. All offsets below are absolute; packed means consecutive bytes, never C struct layout. Source baseline is shipped v2.0, package 0.10.0, API7/ABI1/behavior6. Implementation must produce the field-to-producer proof before exposing public save support. No Phase12 runtime, workflows or test repair is admitted; all its allowances remain spent. Actual RetroArch/rewind/runahead belongs to Phase14.

## Public surface and result precedence

Add size_t muhtari_state_size(const muhtari *m), muhtari_status muhtari_save_state(const muhtari *m, void *buffer, size_t capacity), and muhtari_status muhtari_load_state(muhtari *m, const void *buffer, size_t bytes). Size(NULL) and size(unloaded) are zero. Save/load NULL instance or buffer is ERR_ARGUMENT even with zero length; nonnull unloaded is new ERR_STATE_UNAVAILABLE=15. Save capacity below required is existing ERR_BUFFER_TOO_SMALL; larger capacity writes only canonical prefix. Load length must equal required; all shorter/trailing states are ERR_INVALID_STATE=16 before buffer reads beyond supplied length. Check magic/header envelope, then format/behavior (unknown => ERR_UNSUPPORTED_STATE=17), exact topology size and reserved bytes, CRC, identity (mismatch => ERR_STATE_MISMATCH=18), then all payload invariants. Corrupt bytes are invalid, never trusted by hash identity. Unsupported versions are diagnosed before interpreting their payload. Aliasing returns ERR_ARGUMENT before any buffer or live write. Failed initial content load stays unavailable; refused replacement retains old size and identity.

API8, ABI1, behavior6, state_format1, raw-battery-layout1. Append u32 state_format to muhtari_versions at byte28, sizeof32; old minimum remains28 and size-tagged old callers receive the unchanged prefix. Clarify behavior revision describes emulated observations, not an automatic increment for first state format. Do not change version.txt as a planning shortcut or change behavior6 without demonstrated execution-contract change.

## Envelope

| Offset | Bytes | Value |
|---:|---:|---|
| 0 | 8 | ASCII MUHTST01, exactly eight bytes |
| 8 | 4 | u32 LE state format =1 |
| 12 | 4 | u32 LE header bytes =64 |
| 16 | 4 | u32 LE exact total length |
| 20 | 4 | u32 LE behavior revision =6 |
| 24 | 4 | CRC32 of entire canonical prefix with bytes24..27 treated as zero |
| 28 | 32 | SHA-256 effective machine identity |
| 60 | 4 | reserved zero |

CRC is the existing reflected polynomial0xEDB88320 implementation and its current initialization/final XOR. No trailer, padding, extension, wrapper, compression or attacker-selected section count.

## Complete packed payload

Unsigned values are explicit LE at the width shown; signed values use their mathematical two's-complement representation via defined unsigned conversion, with portable decoding that does not depend on implementation-defined signed casts. Arrays retain fixed row-major ordering. The following inventory includes statistics/diagnostic integer fields for deterministic continuation of existing owned observation tools. No diagnostic is used as a substitute for semantic evidence.

| Absolute offset | Field (row-major arrays) | Encoding | Bytes |
|---:|---|---|---:|
| 64 | audio_rem | u32 | 4 |
| 68 | frame_start | u64 | 8 |
| 76 | frame_deadline | u64 | 8 |
| 84 | frames | u64 | 8 |
| 92 | lag_frames | u64 | 8 |
| 100 | cpu.poll_phi2 | u64 | 8 |
| 108 | cpu.pc | u16 | 2 |
| 110 | cpu.a | u8 | 1 |
| 111 | cpu.x | u8 | 1 |
| 112 | cpu.y | u8 | 1 |
| 113 | cpu.s | u8 | 1 |
| 114 | cpu.p | u8 | 1 |
| 115 | cpu.jammed | u8 | 1 |
| 116 | cpu.irq_sampled | u8 | 1 |
| 117 | bus.clock_origin_tick | u64 | 8 |
| 125 | bus.clock_origin_phi2 | u64 | 8 |
| 133 | bus.phi2 | u64 | 8 |
| 141 | bus.nmi_phi2 | u64 | 8 |
| 149 | bus.ticks | u64 | 8 |
| 157 | bus.rdy_release | u64 | 8 |
| 165 | bus.halt_at | u64 | 8 |
| 173 | bus.halt_until | u64 | 8 |
| 181 | bus.halt_fall | u64 | 8 |
| 189 | bus.maria_from | u64 | 8 |
| 197 | bus.maria_until | u64 | 8 |
| 205 | bus.nmi_at | u64 | 8 |
| 213 | bus.open_bus | u8 | 1 |
| 214 | bus.inptctrl | u8 | 1 |
| 215 | bus.halt_gate_writes | u8 | 1 |
| 216 | bus.halt_stage | u8 | 1 |
| 217 | bus.halt_cycles | u8 | 1 |
| 218 | bus.nmi_pending | u8 | 1 |
| 219 | bus.cycle_len | u8 | 1 |
| 220 | bus.sync | u8 | 1 |
| 221 | bus.irq_pipe | u8 | 1 |
| 222 | bus.irq_level | u8 | 1 |
| 223 | riot.timer_phi2 | u64 | 8 |
| 231 | riot.timer_n | u16 | 2 |
| 233 | riot.timer_shift | u8 | 1 |
| 234 | riot.timer_cleared | u8 | 1 |
| 235 | riot.ora | u8 | 1 |
| 236 | riot.ddra | u8 | 1 |
| 237 | riot.orb | u8 | 1 |
| 238 | riot.ddrb | u8 | 1 |
| 239 | riot.pa7_flag | u8 | 1 |
| 240 | riot.pa7_positive | u8 | 1 |
| 241 | riot.pa7_detect | u8 | 1 |
| 242 | riot.ram | u8 ×128 | 128 |
| 370 | ram | u8 ×4096 | 4096 |
| 4466 | maria.reg | u8 ×32 | 32 |
| 4498 | maria.dll | u16 | 2 |
| 4500 | maria.dl | u16 | 2 |
| 4502 | maria.entry | u8 | 1 |
| 4503 | maria.offset | u8 | 1 |
| 4504 | maria.write_mode | u8 | 1 |
| 4505 | maria.write_buf | u8 | 1 |
| 4506 | maria.line_ram | u8 ×320 | 320 |
| 4826 + 8 × i, i=0..312 | maria.stats[i]: cost@0 u16, bytes@2 u16, headers@4 u8, objects@5 u8, flags@6 u16 | record ×313 | 2504 |
| 7330 | maria.features | u32 | 4 |
| 7334 | maria.origin | u64 | 8 |
| 7342 | maria.dma_lrc | u64 | 8 |
| 7350 | maria.event_line | u16 | 2 |
| 7352 | maria.dma_line | u16 | 2 |
| 7354 | maria.event_index | u8 | 1 |
| 7355 | maria.mstat | u8 | 1 |
| 7356 | maria.px | u16 | 2 |
| 7358 | cart.bank | u8 | 1 |
| 7359 | cart.ram_bank | u8 | 1 |
| 7360 | cart.expansion.control | u8 | 1 |
| 7361 | cart.expansion.cpu_pages | u8 | 1 |
| 7362 | cart.expansion.maria_pages | u8 | 1 |
| 7363 | ports.motion[0][0].next_tick | u64 | 8 |
| 7371 | ports.motion[0][0].residual | i32 | 4 |
| 7375 | ports.motion[0][0].phase | u8 | 1 |
| 7376 | ports.motion[0][0].negative | u8 | 1 |
| 7377 | ports.motion[0][1].next_tick | u64 | 8 |
| 7385 | ports.motion[0][1].residual | i32 | 4 |
| 7389 | ports.motion[0][1].phase | u8 | 1 |
| 7390 | ports.motion[0][1].negative | u8 | 1 |
| 7391 | ports.motion[1][0].next_tick | u64 | 8 |
| 7399 | ports.motion[1][0].residual | i32 | 4 |
| 7403 | ports.motion[1][0].phase | u8 | 1 |
| 7404 | ports.motion[1][0].negative | u8 | 1 |
| 7405 | ports.motion[1][1].next_tick | u64 | 8 |
| 7413 | ports.motion[1][1].residual | i32 | 4 |
| 7417 | ports.motion[1][1].phase | u8 | 1 |
| 7418 | ports.motion[1][1].negative | u8 | 1 |
| 7419 | ports.light[0].sample_tick | u64 | 8 |
| 7427 | ports.light[0].response_tick | u64 | 8 |
| 7435 | ports.light[0].response_until | u64 | 8 |
| 7443 | ports.light[0].low_until | u64 | 8 |
| 7451 | ports.light[0].sample_raster | u16 | 2 |
| 7453 | ports.light[0].sample_x | u16 | 2 |
| 7455 | ports.light[0].delay | u8 | 1 |
| 7456 | ports.light[0].width | u8 | 1 |
| 7457 | ports.light[0].lines | u8 | 1 |
| 7458 | ports.light[0].threshold | u8 | 1 |
| 7459 | ports.light[0].low | u8 | 1 |
| 7460 | ports.light[1].sample_tick | u64 | 8 |
| 7468 | ports.light[1].response_tick | u64 | 8 |
| 7476 | ports.light[1].response_until | u64 | 8 |
| 7484 | ports.light[1].low_until | u64 | 8 |
| 7492 | ports.light[1].sample_raster | u16 | 2 |
| 7494 | ports.light[1].sample_x | u16 | 2 |
| 7496 | ports.light[1].delay | u8 | 1 |
| 7497 | ports.light[1].width | u8 | 1 |
| 7498 | ports.light[1].lines | u8 | 1 |
| 7499 | ports.light[1].threshold | u8 | 1 |
| 7500 | ports.light[1].low | u8 | 1 |
| 7501 | ports.pot[0][0].action | u64 | 8 |
| 7509 | ports.pot[0][0].last_tick | u64 | 8 |
| 7517 | ports.pot[0][0].crossing_tick | u64 | 8 |
| 7525 | ports.pot[0][0].duration | u32 | 4 |
| 7529 | ports.pot[0][1].action | u64 | 8 |
| 7537 | ports.pot[0][1].last_tick | u64 | 8 |
| 7545 | ports.pot[0][1].crossing_tick | u64 | 8 |
| 7553 | ports.pot[0][1].duration | u32 | 4 |
| 7557 | ports.pot[1][0].action | u64 | 8 |
| 7565 | ports.pot[1][0].last_tick | u64 | 8 |
| 7573 | ports.pot[1][0].crossing_tick | u64 | 8 |
| 7581 | ports.pot[1][0].duration | u32 | 4 |
| 7585 | ports.pot[1][1].action | u64 | 8 |
| 7593 | ports.pot[1][1].last_tick | u64 | 8 |
| 7601 | ports.pot[1][1].crossing_tick | u64 | 8 |
| 7609 | ports.pot[1][1].duration | u32 | 4 |
| 7613 | ports.adapter[0].snapshot | u32 | 4 |
| 7617 | ports.adapter[0].host | u8 | 1 |
| 7618 | ports.adapter[0].position | u8 | 1 |
| 7619 | ports.adapter[0].scanning | u8 | 1 |
| 7620 | ports.adapter[1].snapshot | u32 | 4 |
| 7624 | ports.adapter[1].host | u8 | 1 |
| 7625 | ports.adapter[1].position | u8 | 1 |
| 7626 | ports.adapter[1].scanning | u8 | 1 |
| 7627 | ports.keypad_keys | u16 ×2 | 4 |
| 7631 | ports.keypad_low | u8 ×2 | 2 |
| 7633 | ports.device | u32 ×2 | 8 |
| 7641 | ports.buttons | u32 ×2 | 8 |
| 7649 | ports.switches | u32 | 4 |
| 7653 | ports.pending_device | u32 ×2 | 8 |
| 7661 | ports.pending_buttons | u32 ×2 | 8 |
| 7669 | ports.pending_switches | u32 | 4 |
| 7673 | ports.pending_ex.port[0].device | u32 | 4 |
| 7677 | ports.pending_ex.port[0].buttons | u32 | 4 |
| 7681 | ports.pending_ex.port[0].paddle | u32 ×2 | 8 |
| 7689 | ports.pending_ex.port[0].keypad | u32 | 4 |
| 7693 | ports.pending_ex.port[0].dx | i32 | 4 |
| 7697 | ports.pending_ex.port[0].dy | i32 | 4 |
| 7701 | ports.pending_ex.port[0].gun_x | u32 | 4 |
| 7705 | ports.pending_ex.port[0].gun_y | u32 | 4 |
| 7709 | ports.pending_ex.port[0].flags | u32 | 4 |
| 7713 | ports.pending_ex.port[1].device | u32 | 4 |
| 7717 | ports.pending_ex.port[1].buttons | u32 | 4 |
| 7721 | ports.pending_ex.port[1].paddle | u32 ×2 | 8 |
| 7729 | ports.pending_ex.port[1].keypad | u32 | 4 |
| 7733 | ports.pending_ex.port[1].dx | i32 | 4 |
| 7737 | ports.pending_ex.port[1].dy | i32 | 4 |
| 7741 | ports.pending_ex.port[1].gun_x | u32 | 4 |
| 7745 | ports.pending_ex.port[1].gun_y | u32 | 4 |
| 7749 | ports.pending_ex.port[1].flags | u32 | 4 |
| 7753 | ports.pending_ex.switches | u32 | 4 |
| 7757 | ports.applied_ex.port[0].device | u32 | 4 |
| 7761 | ports.applied_ex.port[0].buttons | u32 | 4 |
| 7765 | ports.applied_ex.port[0].paddle | u32 ×2 | 8 |
| 7773 | ports.applied_ex.port[0].keypad | u32 | 4 |
| 7777 | ports.applied_ex.port[0].dx | i32 | 4 |
| 7781 | ports.applied_ex.port[0].dy | i32 | 4 |
| 7785 | ports.applied_ex.port[0].gun_x | u32 | 4 |
| 7789 | ports.applied_ex.port[0].gun_y | u32 | 4 |
| 7793 | ports.applied_ex.port[0].flags | u32 | 4 |
| 7797 | ports.applied_ex.port[1].device | u32 | 4 |
| 7801 | ports.applied_ex.port[1].buttons | u32 | 4 |
| 7805 | ports.applied_ex.port[1].paddle | u32 ×2 | 8 |
| 7813 | ports.applied_ex.port[1].keypad | u32 | 4 |
| 7817 | ports.applied_ex.port[1].dx | i32 | 4 |
| 7821 | ports.applied_ex.port[1].dy | i32 | 4 |
| 7825 | ports.applied_ex.port[1].gun_x | u32 | 4 |
| 7829 | ports.applied_ex.port[1].gun_y | u32 | 4 |
| 7833 | ports.applied_ex.port[1].flags | u32 | 4 |
| 7837 | ports.applied_ex.switches | u32 | 4 |
| 7841 | ports.input_mode | u8 | 1 |
| 7842 | ports.initialized | u8 | 1 |
| 7843 | ports.pending | u8 | 1 |
| 7844 | ports.input_read | u8 | 1 |
| 7845 | ports.io_hazard | u64 | 8 |
| 7853 | boot.handoff_frame | u64 | 8 |
| 7861 | boot.rejected | u8 | 1 |
| 7862 | synth.tick | u64 | 8 |
| 7870 | synth.origin | u64 | 8 |
| 7878 | synth.horizon | u64 | 8 |
| 7886 | synth.pending_tick | u64 | 8 |
| 7894 | synth.impulse | i64 ×16 | 128 |
| 8022 | synth.integrator_q23 | i64 | 8 |
| 8030 | synth.previous_q30 | i64 | 8 |
| 8038 | synth.blocker_q30 | i64 | 8 |
| 8046 | synth.pre_clip_peak | u64 | 8 |
| 8054 | synth.clipped_samples | u64 | 8 |
| 8062 | synth.phase | u32 | 4 |
| 8066 | synth.pcm_read | u32 | 4 |
| 8070 | synth.pcm_count | u32 | 4 |
| 8074 | synth.sample_num | u32 | 4 |
| 8078 | synth.sample_den | u32 | 4 |
| 8082 | synth.max_samples | u32 | 4 |
| 8086 | synth.kernel_adds | u32 | 4 |
| 8090 | synth.kernel_adds_per_sample | u32 | 4 |
| 8094 | synth.carry_high_water | u32 | 4 |
| 8098 | synth.mixed_level | i32 | 4 |
| 8102 | synth.raw | u16 ×10 | 20 |
| 8122 | synth.head | u8 | 1 |
| 8123 | synth.pending | u8 | 1 |
| 8124 | synth.pokeys | u8 | 1 |
| 8125 | synth.secondary | u8 | 1 |
| 8126 | synth.pcm | i16 ×1103 | 2206 |
| 10332 | tia.next_tick | u64 | 8 |
| 10340 | tia.epoch_tick | u64 | 8 |
| 10348 | tia.epoch_phase | u16 | 2 |
| 10350 | tia.phase | u8 | 1 |
| 10351 | tia.initialized | u8 | 1 |
| 10352 | tia.input_latch_enable | u8 | 1 |
| 10353 | tia.input_dump | u8 | 1 |
| 10354 | tia.input_latch | u8 ×2 | 2 |
| 10356 | tia.channel[0].audc | u8 | 1 |
| 10357 | tia.channel[0].audf | u8 | 1 |
| 10358 | tia.channel[0].audv | u8 | 1 |
| 10359 | tia.channel[0].count | u8 | 1 |
| 10360 | tia.channel[0].enabled | u8 | 1 |
| 10361 | tia.channel[0].output | u8 | 1 |
| 10362 | tia.channel[0].noise | u8 | 1 |
| 10363 | tia.channel[0].pulse | u8 | 1 |
| 10364 | tia.channel[0].noise_input | u8 | 1 |
| 10365 | tia.channel[0].pulse_gate | u8 | 1 |
| 10366 | tia.channel[1].audc | u8 | 1 |
| 10367 | tia.channel[1].audf | u8 | 1 |
| 10368 | tia.channel[1].audv | u8 | 1 |
| 10369 | tia.channel[1].count | u8 | 1 |
| 10370 | tia.channel[1].enabled | u8 | 1 |
| 10371 | tia.channel[1].output | u8 | 1 |
| 10372 | tia.channel[1].noise | u8 | 1 |
| 10373 | tia.channel[1].pulse | u8 | 1 |
| 10374 | tia.channel[1].noise_input | u8 | 1 |
| 10375 | tia.channel[1].pulse_gate | u8 | 1 |
| 10376 | pokey[0].cycle | u64 | 8 |
| 10384 | pokey[0].tick | u64 | 8 |
| 10392 | pokey[0].half | u64 | 8 |
| 10400 | pokey[0].carry | u64 ×2 | 16 |
| 10416 | pokey[0].resync | u64 ×2 | 16 |
| 10432 | pokey[0].poly17 | u32 | 4 |
| 10436 | pokey[0].poly4 | u8 | 1 |
| 10437 | pokey[0].poly5 | u8 | 1 |
| 10438 | pokey[0].noise | u8 ×3 | 3 |
| 10441 | pokey[0].prescale28 | u16 | 2 |
| 10443 | pokey[0].prescale114 | u16 | 2 |
| 10445 | pokey[0].audctl | u8 | 1 |
| 10446 | pokey[0].skctl | u8 | 1 |
| 10447 | pokey[0].irqen | u8 | 1 |
| 10448 | pokey[0].irqst | u8 | 1 |
| 10449 | pokey[0].initialized | u8 | 1 |
| 10450 | pokey[0].hp_delay | u8 ×2 | 2 |
| 10452 | pokey[0].hp_capture | u8 ×2 | 2 |
| 10454 | pokey[0].irq_enable | u8 ×3 | 3 |
| 10457 | pokey[0].irq_signal | u8 | 1 |
| 10458 | pokey[0].channel[0].reload | u64 | 8 |
| 10466 | pokey[0].channel[0].fire | u64 | 8 |
| 10474 | pokey[0].channel[0].last_fire | u64 | 8 |
| 10482 | pokey[0].channel[0].fires | u32 | 4 |
| 10486 | pokey[0].channel[0].count | u16 | 2 |
| 10488 | pokey[0].channel[0].audf | u8 | 1 |
| 10489 | pokey[0].channel[0].audc | u8 | 1 |
| 10490 | pokey[0].channel[0].output | u8 | 1 |
| 10491 | pokey[0].channel[0].hp | u8 | 1 |
| 10492 | pokey[0].channel[0].starting | u8 | 1 |
| 10493 | pokey[0].channel[1].reload | u64 | 8 |
| 10501 | pokey[0].channel[1].fire | u64 | 8 |
| 10509 | pokey[0].channel[1].last_fire | u64 | 8 |
| 10517 | pokey[0].channel[1].fires | u32 | 4 |
| 10521 | pokey[0].channel[1].count | u16 | 2 |
| 10523 | pokey[0].channel[1].audf | u8 | 1 |
| 10524 | pokey[0].channel[1].audc | u8 | 1 |
| 10525 | pokey[0].channel[1].output | u8 | 1 |
| 10526 | pokey[0].channel[1].hp | u8 | 1 |
| 10527 | pokey[0].channel[1].starting | u8 | 1 |
| 10528 | pokey[0].channel[2].reload | u64 | 8 |
| 10536 | pokey[0].channel[2].fire | u64 | 8 |
| 10544 | pokey[0].channel[2].last_fire | u64 | 8 |
| 10552 | pokey[0].channel[2].fires | u32 | 4 |
| 10556 | pokey[0].channel[2].count | u16 | 2 |
| 10558 | pokey[0].channel[2].audf | u8 | 1 |
| 10559 | pokey[0].channel[2].audc | u8 | 1 |
| 10560 | pokey[0].channel[2].output | u8 | 1 |
| 10561 | pokey[0].channel[2].hp | u8 | 1 |
| 10562 | pokey[0].channel[2].starting | u8 | 1 |
| 10563 | pokey[0].channel[3].reload | u64 | 8 |
| 10571 | pokey[0].channel[3].fire | u64 | 8 |
| 10579 | pokey[0].channel[3].last_fire | u64 | 8 |
| 10587 | pokey[0].channel[3].fires | u32 | 4 |
| 10591 | pokey[0].channel[3].count | u16 | 2 |
| 10593 | pokey[0].channel[3].audf | u8 | 1 |
| 10594 | pokey[0].channel[3].audc | u8 | 1 |
| 10595 | pokey[0].channel[3].output | u8 | 1 |
| 10596 | pokey[0].channel[3].hp | u8 | 1 |
| 10597 | pokey[0].channel[3].starting | u8 | 1 |
| 10598 | pokey[1].cycle | u64 | 8 |
| 10606 | pokey[1].tick | u64 | 8 |
| 10614 | pokey[1].half | u64 | 8 |
| 10622 | pokey[1].carry | u64 ×2 | 16 |
| 10638 | pokey[1].resync | u64 ×2 | 16 |
| 10654 | pokey[1].poly17 | u32 | 4 |
| 10658 | pokey[1].poly4 | u8 | 1 |
| 10659 | pokey[1].poly5 | u8 | 1 |
| 10660 | pokey[1].noise | u8 ×3 | 3 |
| 10663 | pokey[1].prescale28 | u16 | 2 |
| 10665 | pokey[1].prescale114 | u16 | 2 |
| 10667 | pokey[1].audctl | u8 | 1 |
| 10668 | pokey[1].skctl | u8 | 1 |
| 10669 | pokey[1].irqen | u8 | 1 |
| 10670 | pokey[1].irqst | u8 | 1 |
| 10671 | pokey[1].initialized | u8 | 1 |
| 10672 | pokey[1].hp_delay | u8 ×2 | 2 |
| 10674 | pokey[1].hp_capture | u8 ×2 | 2 |
| 10676 | pokey[1].irq_enable | u8 ×3 | 3 |
| 10679 | pokey[1].irq_signal | u8 | 1 |
| 10680 | pokey[1].channel[0].reload | u64 | 8 |
| 10688 | pokey[1].channel[0].fire | u64 | 8 |
| 10696 | pokey[1].channel[0].last_fire | u64 | 8 |
| 10704 | pokey[1].channel[0].fires | u32 | 4 |
| 10708 | pokey[1].channel[0].count | u16 | 2 |
| 10710 | pokey[1].channel[0].audf | u8 | 1 |
| 10711 | pokey[1].channel[0].audc | u8 | 1 |
| 10712 | pokey[1].channel[0].output | u8 | 1 |
| 10713 | pokey[1].channel[0].hp | u8 | 1 |
| 10714 | pokey[1].channel[0].starting | u8 | 1 |
| 10715 | pokey[1].channel[1].reload | u64 | 8 |
| 10723 | pokey[1].channel[1].fire | u64 | 8 |
| 10731 | pokey[1].channel[1].last_fire | u64 | 8 |
| 10739 | pokey[1].channel[1].fires | u32 | 4 |
| 10743 | pokey[1].channel[1].count | u16 | 2 |
| 10745 | pokey[1].channel[1].audf | u8 | 1 |
| 10746 | pokey[1].channel[1].audc | u8 | 1 |
| 10747 | pokey[1].channel[1].output | u8 | 1 |
| 10748 | pokey[1].channel[1].hp | u8 | 1 |
| 10749 | pokey[1].channel[1].starting | u8 | 1 |
| 10750 | pokey[1].channel[2].reload | u64 | 8 |
| 10758 | pokey[1].channel[2].fire | u64 | 8 |
| 10766 | pokey[1].channel[2].last_fire | u64 | 8 |
| 10774 | pokey[1].channel[2].fires | u32 | 4 |
| 10778 | pokey[1].channel[2].count | u16 | 2 |
| 10780 | pokey[1].channel[2].audf | u8 | 1 |
| 10781 | pokey[1].channel[2].audc | u8 | 1 |
| 10782 | pokey[1].channel[2].output | u8 | 1 |
| 10783 | pokey[1].channel[2].hp | u8 | 1 |
| 10784 | pokey[1].channel[2].starting | u8 | 1 |
| 10785 | pokey[1].channel[3].reload | u64 | 8 |
| 10793 | pokey[1].channel[3].fire | u64 | 8 |
| 10801 | pokey[1].channel[3].last_fire | u64 | 8 |
| 10809 | pokey[1].channel[3].fires | u32 | 4 |
| 10813 | pokey[1].channel[3].count | u16 | 2 |
| 10815 | pokey[1].channel[3].audf | u8 | 1 |
| 10816 | pokey[1].channel[3].audc | u8 | 1 |
| 10817 | pokey[1].channel[3].output | u8 | 1 |
| 10818 | pokey[1].channel[3].hp | u8 | 1 |
| 10819 | pokey[1].channel[3].starting | u8 | 1 |
| 10820 | savekey.busy_until | u64 | 8 |
| 10828 | savekey.address | u16 | 2 |
| 10830 | savekey.page_base | u16 | 2 |
| 10832 | savekey.host_scl | u8 | 1 |
| 10833 | savekey.host_sda | u8 | 1 |
| 10834 | savekey.sda_low | u8 | 1 |
| 10835 | savekey.phase | u8 | 1 |
| 10836 | savekey.next_phase | u8 | 1 |
| 10837 | savekey.bits | u8 | 1 |
| 10838 | savekey.shift | u8 | 1 |
| 10839 | savekey.address_high | u8 | 1 |
| 10840 | savekey.read_byte | u8 | 1 |
| 10841 | savekey.page | u8 ×64 | 64 |
| 10905 | savekey.touched | u8 ×64 | 64 |
| 10969 | savekey.page_offset | u8 | 1 |
| 10970 | savekey.pending | u8 | 1 |
| 10971 | savekey.ack | u8 | 1 |

After byte10971, append cart RAM (exact installed cart.info.ram_size bytes), expansion RAM (131072 bytes iff installed kind XBOARD or XM, otherwise0), then battery.data[0..battery.bytes). Their offsets are 10972, 10972+C and 10972+C+E respectively. Exact size =10972+C+E+B. Checked size_t addition and u32 conversion precede any write. C<=131072, E<=131072, B<=34816 from current source topology => universal installed-size upper bound307932 bytes. This upper bound is not a newly imposed quota. Confirm the largest simultaneously accepted topology; distinguish an unreachable sum of independent maxima from measured maximum legal work. No capacity, length, allocation, loop or pointer is selected from untrusted bytes.

## Canonical values, exclusions and reconstruction

The table includes every integer leaf of mutable owners, except bus.next_deadline and ports.next_tick (derived); public input_ex size tags (host layout, rebuild to sizeof); immutable effective_region, profile and load_device (identity-bound installed topology); cart allocation pointers/length/info/irq wiring/pokey_at and expansion.kind (installed identity); battery bytes count/attachment/firmware metadata (identity); battery.generation (host token). m.loaded, allocator, observers, observation pointer, audio_work, bios and hsc staging, BIOS/HSC/cart ROM, MARIA rows/pitch and every pointer remain outside payload. Terse stale pending-input comments and prep prose must be corrected.

For synth.pcm serialize active ring entries at their fixed physical indices and emit zero for inactive slots; preserve read/count/head and validate them. MARIA stats entries outside selected region lines are zero; input packets use only normalized owned fields, not caller padding. Any other producer-inactive byte retained by the format must be either initialized, proven behavioral, or canonical zero with an explicit inventory row. Never canonicalize a valid UINT64_MAX scheduling sentinel to zero.

SIZE/WRITE/VALIDATE/APPLY use one owner-ordered typed traversal; validators share only decoded scalar scratch, not a full copied muhtari. VALIDATE does not mutate instance, tokens, indexes, callbacks or media. Save preflights all conditions and valid-live canonicalization so WRITE cannot fail midway. Validate complete envelope/fields/cross-field relationships before APPLY. APPLY has no allocation, fallible operation, event catch-up, callback, emulated tick advance or failure return. Compute changed battery comparison before writes; apply all fields; advance live generation once iff committed bytes changed, under existing unsigned wrap/equality semantics. Restore pending EEPROM page/protocol without dirtying equal committed bytes. Preserve all public RAM/battery span addresses. Finish by reconstructing input size tags, leaving MARIA rows=NULL/pitch=0, invoking a narrow ports-owned rearm hook and then muhtari__bus_rearm. Rebuild only after every semantic field is applied; never call a reset or catch-up function as reconstruction.

Reject supplied canonical range intersecting [m,m+sizeof(*m)) or [m->cart.rom,m->cart.rom+allocation_bytes), including ROM+cartRAM+expansion in that single actual allocation. Also check any owner-separated actual allocation if the inspected layout changes. Save checks the prefix it writes, not unused excess caller capacity; load checks supplied exact range. Checked uintptr_t half-open arithmetic rejects wrap, zero-length alias is covered by exact-length/argument rules; adjacent endpoints do not overlap. Tests cover interior and boundary aliasing through each public mutable span, firmware view, and private instance storage. Caller must provide valid accessible C memory and keep it stable for the duration; racing modification is outside existing nonconcurrent instance contract.

## Effective identity encoding

SHA256 over ASCII MUHTARI-STATE-IDENTITY followed by byte0, then fields in this order. Each field is a record u32 LE tag + u32 LE byte_length + value. Integer values are u32 LE; byte strings are exact installed bytes. Tags1..22:
1 installed cart ROM; 2 cart.info.kind; 3 board; 4 board_options; 5 ram_size; 6 effective_region; 7 effective sound mapping encoded as two u32 resolved pokey_at addresses; 8 cart.irq_mask; 9 resolved expansion.kind; 10 effective load-time port0 device; 11 effective load-time port1 device; 12 battery.hsc_attached; 13 battery.eeprom_port; 14 battery.eeprom_device; 15 battery.bytes; 16 boot mode (0 fast,1 BIOS); 17 BIOS bytes (empty fast); 18 HSC firmware mode (0 absent,1 stub,2 supplied); 19 installed HSC firmware bytes (empty absent,4096 otherwise); 20 profile bytes as ane_lxa_constant u8, riot_orb_power_on u8, ram_fill u8, tia_phase u16 LE, pokey_init u8; 21 synth installed pokeys u32; 22 synth installed secondary u32.

Resolved load-time port device uses the existing device-resolution/fallback semantics for media.port override then accepted cart device, never current transient input.device. Cache digest only after successful content installation and final power_cycle initialization, with no subsequent failure possible; failed replacement preserves previous digest. Power cycling keeps identity. Identity is independent of source paths, A78 metadata/title/warnings, raw header versus equivalent effective payload, Auto spelling versus same resolved option, callback/allocator addresses, build/package timestamp, generation and future movie input. Don't include firmware warning labels in place of immutable firmware bytes. Relocate ownership of the existing SHA/CRC through the root private core build, retaining the owned source files if that avoids broad mechanical churn; do not replace SHA with HSC SHA1 or introduce a package.

## Producer-derived validation gate

For EVERY inventory leaf, implementation adds a row in 13-FIELD-PROOF.md naming source producer/function, reachable range or unrestricted fixed-width status, inactivity/sentinel meaning, relationships and hostile test or explicit nonbehavioral canonical rule. Numeric range proofs not established here remain mandatory execution work, not guessed limits. Public APIs may be exposed only after this ledger has no unexplained mutable field or unsafe decoded consumer. State validation must not reject a value solely for being large. No source-enforced global tick cap exists; do not import stale2^62.

Cross-owner obligations: CPU poll_phi2<=bus.phi2; raw status/register bits reflect actual producer semantics; cycle_len actual allowed values4/6 and HALT/RDY/NMI stages agree with timestamps; bus clock_origin and phi2 mapping preserved; RIOT timer ranges/shifts/time ordering; MARIA region lines/event_index/DMA cursor/offset/write-buffer/px, line-stat masks and event arithmetic; TIA phases/counters/epoch lattice; POKEY polynomial/counter/prescale/IRQ/half-cycle and fire/reload/carry/resync no-event UINT64_MAX meaning; synth phase/sample lattice from installed timing, FIFO read/count/head, impulse/mixer/filter numeric domains and overshoot/carry horizon from existing proof; controller motion/pot/light/adapter/keypad enums, bounded pending motion and electrical deadline consistency; SaveKey protocol phase/bits/addresses/page_base/touched masks/pending/ack/busy consistency; frame_start/deadline/bus/MARIA/audio times and counters. Validate overflow by checking precisely the operations the current producers will perform and their established domains, preserving all legitimate sentinel paths. Do not impose an absolute clock ceiling or enlarge existing frame bounds. Pointer/index reconstruction itself cannot walk decoded unbounded loops. Derive and measure traversal count, stack scalar scratch, object growth and full SIZE/WRITE/VALIDATE/APPLY work on maximum legal topology, including late rejection. Retain completed-work observations only.

## Runner decisions

--load-state FILE once; --state-at-frame N:FILE repeatable; --hash-state N repeatable. N is completed frame calls in this invocation, in0..frames. Allow explicit --frames0 only for a content-loaded state operation with no frame/movie-dependent output; retain default old positive-frame behavior elsewhere. Zero is after content/effective options/initial battery and optional state load, before next input; pointN after Nth successful run_frame and completion increment, before setting subsequent input. Restored public frame counter does not change invocation index.

Require content; reject bench/probe combinations, duplicate --load-state, repeated hash-state point, repeated state-save point, repeated output filename among state outputs or existing --dump-frame outputs, and same lexical path for state output and load-state/movie/content/BIOS/HSC input. Reject '-' and empty state file names (no stream ambiguity). State hashes print 'state N sha256 <64 lowercase hex>\n' sorted by N; saves may coexist with hash at same N. Reuse current count/overflow checks and allocation ownership; do not invent a separate recording protocol. --movie remains supported only as explicit input sequence starting at its own row0; a fresh continuation uses a separately supplied suffix movie with matching metadata. No implicit restored movie cursor, auto seeking or inference from restored counters.

One host state buffer allocated from queried exact size and reused. Read expected bytes plus a one-byte trailing probe, checking ferror and fclose before calling core. Refuse short/extra/I/O errors without restore. Reuse checked sibling temporary write/exclusive-create/flush/sync/close/replace mechanics, preserving target before replacement. Library never performs file I/O. Explicit accepted rollback may later overwrite newer durable battery data on ordinary committed runner flush; refusal exits before frame execution and before final battery flush. Invocation with only point0 and zero successful frames does not flush durable battery. Failure1, usage2, existing unsupported game77 unchanged. Use useful separate diagnostics for file I/O versus malformed/version/machine refusal.

## Tests and coverage matrix

Plan04 produces 13-CONTINUATION-MATRIX.md as an execution ledger with row, supported owner class, independent oracle, pending boundary, fixture, comparisons, command/exit/evidence. Required representatives: all accepted board classes currently supported (linear, SuperGame including banked RAM, Activision, Absolute, Fractalus, BankSet; do not treat refused SOUPER as supported), NTSC/PAL, fast/owned BIOS, primary+secondary POKEY mappings, expansion OFF/XBOARD/XM, HSC absent/stub/supplied and SaveKey port1/port2 when legal; active/pending legacy+extended packet, once-only motion, pot crossing, light response, keypad/adapter scan; DMA/HALT/NMI boundary, mixed audio pending tick/PCM carry, EEPROM protocol/page/busy before and after commit. Every supported class maps to a row or an evidence-backed alias of an existing row. Add justified interaction rows (e.g. PAL+BIOS+POKEY, expansion+banked RAM+HSC, pending motion+restore, pending EEPROM+equal battery); no Cartesian enumeration.

Each row pairs uninterrupted/restored canonical bytes, pixels, raw transition audio, PCM and counters with an independent owned expected mailbox/pin/register/pixel/numeric timing oracle. Include a deliberate missing-pending-field or wrong-observation control through existing harness, so equally wrong split runs fail. Tests use owned generated or manifest-admitted inputs, ephemeral state bytes only. Negative tests repair CRC after payload/header mutations, assert the intended validation branch, and compare post-refusal next execution, pending input, generation, battery bytes and span identity. Repeated identical save/load is canonical; changed battery restore dirties once then equal repeat preserves generation. Existing instance nonconcurrency contract applies; two distinct instances isolate buffers/state. No threads added.

## Source-bound BE evidence and real boundary

be-equality.yml is workflow_call only, contents:read, immutable ref, complete native ci + be32-full and normal/release inventories. Existing nightly is schedule + workflow_dispatch + pull_request path filter that includes src/runner/tests/build changes; its BE call uses github.sha (for PR events, qualify the actual checked-out merge SHA, never label it the branch head). Release calls BE as publication gate. Required PR be32 stays compile-only; required hash-equality stays six native lanes. Nightly remains advisory; Phase13 independently requires executed BE results.

Plan08 must first locate an already installed Linux environment/toolchain satisfying cmake/toolchains/ppc-linux-gnu.cmake (powerpc-linux-gnu-gcc/g++ and qemu-ppc with /usr/powerpc-linux-gnu). Local macOS currently exposes Docker CLI but no native PPC compiler/qemu in PATH; CLI existence is not a proved usable environment. Use an existing independently authorized local Linux environment only, no guessed image pin, install or hosted allowance. Run the exact existing producer shell steps (the immutable checkout, pinned fixture fetches, ci and be32-full workflows and both check_junit consumers) in a clean checkout of the final execution commit, then build evidence/native and evidence/be commit.txt, identity.txt and hashes.txt exactly as be-equality.yml does and run tests/cmake/lane_equality.cmake with EXPECTED_SHA. Include state rows in existing hashes.txt. One-off local evidence records command, exit, N/A-local and source identity, not committed logs.

If that environment is unavailable, complete all local authorized work and STOP at Plan08's explicit evidence checkpoint. Present final source, green local receipts and the concrete route: separately authorized PR publication to trigger the unchanged nightly BE caller, or owner-provided existing Linux tool environment. Do not dispatch nightly's whole workflow or publish a branch implicitly. A future release is not present evidence. After authority/evidence arrives, verify artifact digest/commit.txt/identity.txt/hashes.txt and complete inventories against actual exact source; include queue/critical-path/durations/cancelled-work assessment without an unrelated tuning project. No Phase12 allowance or host test is consumed. Phase acceptance remains pending until the executed native/BE receipt exists.

## Spec-less edge/prohibition resolution and assumptions

Authoritative /tmp edge report has13 items: idempotency and concurrency for STATE-01/03/04/05/06/FRONT-01, plus STATE-02 unclassified. Twelve explicit predicates are lifted into corresponding plan truths: repeated identical operations canonical/preserve correct generation and output; operations between calls only; distinct instances isolated; host file writes owned and explicit; callback context rules unchanged. Concurrency is resolved by existing same-instance nonconcurrency and distinct-instance isolation, without a threading feature.

Flagged assumption: STATE-02 remains unresolved/unclassified by the automatic shape probe. Explicit hostile-validation matrix covers the requirement but classification itself must not be silently marked resolved. Verifier inspects the matrix and reports any untested interpretation; no new generic gate is created.

Prohibition recall retained3 product/authority constraints, descriptor-less status unverified: never embed immutable/proprietary media or personal durable bytes in committed state fixtures; never turn state rollback into automatic durable/frontend recovery ownership; never promote callback/compile/advisory evidence into actual-host/required/release claims or renew Phase12 allowances. Ordinary correctness and generic OWASP/security candidates are dropped to existing code review, not new phase gates. Canon projection is projectProhibitions; no fabricated check_* descriptors. These are flagged assumptions, not claimed verified results.

Assumption-delta detector: no-change. 'Chosen frame' means a loaded compatible machine plus this invocation's completed-frame point; primary noun and existing effective identity remain stable. No new identity abstraction or movie timeline is introduced.

## Multi-source coverage audit

| Source | Item | Plans | Status |
|---|---|---|---|
| GOAL | Safely restore complete compatible machine and continue through one bounded contract | 01–08 | COVERED |
| REQ | STATE-01 complete stable-size library format | 01,02,03,08 | COVERED |
| REQ | STATE-02 transactional hostile refusal | 01,03,04,05,06 | COVERED |
| REQ | STATE-03 independent portable continuation | 04,07,08 | COVERED |
| REQ | STATE-04 bounded no-allocation repeated use | 01,03,04,08 | COVERED |
| REQ | STATE-05 battery token/durability separation | 03,04,05,06 | COVERED |
| REQ | STATE-06 runner save/load/hash/file failures | 05,07 | COVERED |
| REQ | FRONT-01 loaded-module callbacks and truthful claims | 06,08 | COVERED |
| RESEARCH | Owner field inventory, exact typed layout, derived rebuild | 01,03 | COVERED |
| RESEARCH | Effective SHA identity and private owned CRC/SHA reuse | 02,03 | COVERED |
| RESEARCH | Producer-based limits/sentinels and full-work cost | 01,04,08 | COVERED |
| RESEARCH | API/status/version and lifecycle | 03,06 | COVERED |
| RESEARCH | Runner parser, frame points, suffix movie and checked files | 05 | COVERED |
| RESEARCH | Libretro context3 and capacity/exact-length semantics | 06 | COVERED |
| RESEARCH | Independent matrix, repaired-CRC faults, seeded consumers | 04,07 | COVERED |
| RESEARCH | Exact-source executed BE producer and honest labels | 07,08 | COVERED |
| CONTEXT | D-01,D-02,D-03 | 01,03,08 | COVERED |
| CONTEXT | D-04 | 02,03,04 | COVERED |
| CONTEXT | D-05,D-06 | 01,03,04,08 | COVERED |
| CONTEXT | D-07,D-10 | 03,06,08 | COVERED |
| CONTEXT | D-08,D-09 | 03,04,05,06 | COVERED |
| CONTEXT | D-11,D-12 | 05,07 | COVERED |
| CONTEXT | D-13 | 06 | COVERED |
| CONTEXT | D-14,D-15 | 04,07 | COVERED |
| CONTEXT | D-16 | 07,08 | COVERED |
| CONTEXT | D-17 | 01–08 | COVERED |

Exclusions: Phase12 repair/publication allowances, actual RetroArch/rewind/runahead qualification, cross-version migration, compression, netplay, generalized movie/AI/TAS and Playstead-native recovery. No source item is dropped for implementation difficulty.

