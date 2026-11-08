#!/bin/sh
set -eu

usage()
{
    cat <<'USAGE'
Usage: scripts/retroarch-manual-input.sh --run [--retroarch EXECUTABLE]
       scripts/retroarch-manual-input.sh --prepare-only [--retroarch EXECUTABLE]

--prepare-only builds the diagnostic tools and verifies isolated preparation without launching an app.
--run launches RetroArch with the owned input-history cartridge, waits for exit, and reports sampled callbacks.
USAGE
}

fail()
{
    printf 'manual input: %s\n' "$*" >&2
    exit 2
}

mode=
retroarch=/Applications/RetroArch.app/Contents/MacOS/RetroArch
while [ "$#" -gt 0 ]; do
    case "$1" in
        --run|--prepare-only)
            [ -z "$mode" ] || fail 'choose exactly one of --run or --prepare-only'
            mode=$1
            shift
            ;;
        --retroarch)
            [ "$#" -ge 2 ] || fail '--retroarch needs an executable path'
            retroarch=$2
            shift 2
            ;;
        --help|-h)
            usage
            exit 0
            ;;
        *)
            fail "unknown argument: $1"
            ;;
    esac
done
[ -n "$mode" ] || { usage >&2; exit 2; }

script_dir=$(CDPATH= cd -P "$(dirname "$0")" && pwd -P)
root=$(CDPATH= cd -P "$script_dir/.." && pwd -P)
build_dir=${MUHTARI_BUILD_DIR:-$root/build/ci}
case "$build_dir" in
    /*) ;;
    *) build_dir=$(CDPATH= cd -P "$build_dir" 2>/dev/null && pwd -P) || fail "build directory does not exist: $build_dir" ;;
esac

if [ ! -f "$retroarch" ] || [ -d "$retroarch" ] || [ ! -x "$retroarch" ]; then
    fail "RetroArch executable is absent or not executable: $retroarch"
fi
case "$retroarch" in
    /*) ;;
    *) retroarch=$(CDPATH= cd -P "$(dirname "$retroarch")" && pwd -P)/$(basename "$retroarch") ;;
esac

cmake --build "$build_dir" --target \
    muhtari_libretro_input_trace muhtari-test-input \
    muhtari-test-trace-protocol muhtari-test-trace-host

input_bin=$build_dir/tests/muhtari-test-input
trace_protocol=$build_dir/tests/muhtari-test-trace-protocol
trace_host=$build_dir/tests/muhtari-test-trace-host
if [ -f "$build_dir/libretro/muhtari_libretro_input_trace.dylib" ]; then
    core=$build_dir/libretro/muhtari_libretro_input_trace.dylib
elif [ -f "$build_dir/libretro/muhtari_libretro_input_trace.so" ]; then
    core=$build_dir/libretro/muhtari_libretro_input_trace.so
else
    fail "diagnostic core was not produced under $build_dir/libretro"
fi
for executable in "$input_bin" "$trace_protocol" "$trace_host" "$core"; do
    [ -f "$executable" ] || fail "required build output is missing: $executable"
done

umask 077
mkdir -p "$build_dir"
output_dir=$(mktemp -d "$build_dir/manual-input.XXXXXX") || fail 'could not create private output directory'
chmod 700 "$output_dir"
private_home=$output_dir/home
trace_dir=$output_dir/traces
config=$output_dir/retroarch.cfg
mkdir -m 700 "$private_home" "$private_home/.config" "$private_home/.cache" \
    "$output_dir/save" "$output_dir/states" "$output_dir/system" "$output_dir/assets" \
    "$output_dir/cores" "$output_dir/info" "$output_dir/playlists" "$output_dir/cache" \
    "$output_dir/logs" "$output_dir/shots" "$trace_dir"
cp "$core" "$output_dir/cores/"
if [ -f "$build_dir/libretro/muhtari_libretro.info" ]; then
    cp "$build_dir/libretro/muhtari_libretro.info" "$output_dir/info/"
fi
{
    printf '%s\n' \
        'config_save_on_exit = "false"' \
        'video_fullscreen = "false"' \
        'video_vsync = "true"' \
        'video_swap_interval = "1"' \
        'audio_driver = "null"' \
        'audio_enable = "false"' \
        'pause_nonactive = "false"' \
        'log_to_file = "true"' \
        'input_player1_up = "up"' \
        'input_player1_b = "z"' \
        "assets_directory = \"$output_dir/assets\"" \
        "cache_directory = \"$output_dir/cache\"" \
        "core_options_path = \"$output_dir/retroarch-core-options.cfg\"" \
        "libretro_directory = \"$output_dir/cores\"" \
        "libretro_info_path = \"$output_dir/info\"" \
        "playlist_directory = \"$output_dir/playlists\"" \
        "savefile_directory = \"$output_dir/save\"" \
        "savestate_directory = \"$output_dir/states\"" \
        "screenshot_directory = \"$output_dir/shots\"" \
        "system_directory = \"$output_dir/system\"" \
        "log_dir = \"$output_dir/logs\""
} > "$config"
chmod 600 "$config"

cart_dir=
cart_file=
cleanup_cart()
{
    if [ -n "$cart_file" ]; then
        rm -f "$cart_file"
    fi
    if [ -n "$cart_dir" ]; then
        rmdir "$cart_dir" 2>/dev/null || true
    fi
}
trap cleanup_cart EXIT

cart_dir=$(mktemp -d /tmp/muhtari-manual-cart.XXXXXX) || fail 'could not create an external temporary cartridge directory'
cart_file=$cart_dir/input-history.a78
"$input_bin" --write-history-cart "$cart_file"
"$input_bin" --check-history-cart "$cart_file"

if [ "$mode" = "--prepare-only" ]; then
    printf 'Prepared isolated RetroArch configuration and checked the owned history cartridge; no frontend was launched.\n'
    printf 'output_dir=%s\n' "$output_dir"
    printf 'trace_dir=%s\n' "$trace_dir"
    exit 0
fi

test_child=${MUHTARI_MANUAL_INPUT_TEST_CHILD:-0}
if [ "$test_child" = 1 ]; then
    expected_core=${MUHTARI_MANUAL_INPUT_EXPECT_CORE:-}
    [ -n "$expected_core" ] && [ "$core" = "$expected_core" ] ||
        fail 'synthetic wrapper test expected a different diagnostic core path'
    printf 'Synthetic wrapper check: no physical input or GUI app will be used.\n'
else
    printf 'Focus the RetroArch window, hold Up+Z together for about one second, then release both. The owned cartridge advances waiting → held → complete colors and exits at its frame limit.\n' >&2
fi

launch_rc=0
if HOME="$private_home" CFFIXED_USER_HOME="$private_home" \
    XDG_CONFIG_HOME="$private_home/.config" XDG_CACHE_HOME="$private_home/.cache" \
    MUHTARI_MANUAL_INPUT_EXPECT_CONFIG="$config" \
    MUHTARI_MANUAL_INPUT_EXPECT_CORE="$core" \
    MUHTARI_MANUAL_INPUT_EXPECT_CONTENT="$cart_file" \
    "$trace_host" --manual-launch "$trace_dir" -- "$retroarch" \
        --verbose --config "$config" -L "$core" --max-frames=3600 "$cart_file"; then
    :
else
    launch_rc=$?
fi

collect_rc=0
if "$trace_protocol" --collect "$trace_dir"; then :; else collect_rc=$?; fi
callback_rc=0
if "$trace_protocol" --manual-core "$trace_dir/core.trace"; then :; else callback_rc=$?; fi

printf 'output_dir=%s\ntrace_dir=%s\n' "$output_dir" "$trace_dir"
if [ "$launch_rc" -eq 0 ] && [ "$collect_rc" -eq 0 ] && [ "$callback_rc" -eq 0 ]; then
    printf 'Callback result: RetroArch delivered the requested neutral → Up+Z held → released sequence to the diagnostic core.\n'
    printf 'The trace does not prove the cartridge display changed; record its waiting/held/complete color progression separately.\n'
else
    printf 'Callback result is inconclusive (launch=%s, trace_pair=%s, callback_sequence=%s); no cause is inferred.\n' \
        "$launch_rc" "$collect_rc" "$callback_rc" >&2
    exit 1
fi
