#!/bin/sh
# Reproduce the required offline WebAssembly/libretro observer lane.
set -eu

image='emscripten/emsdk@sha256:6bd7aef93408b10bf7b36642cb7c91119b302f720b9ec2d20439f73cf07fe894'
platform='linux/amd64'
root_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
mode='run'

usage() {
  echo 'usage: scripts/emscripten.sh [--preflight] build/ci' >&2
  exit 2
}

if [ "${1-}" = '--preflight' ]; then
  mode='preflight'
  shift
fi
[ "$#" -eq 1 ] || usage
[ "$1" = 'build/ci' ] || usage
cd "$root_dir"

fail() { echo "emscripten: $*" >&2; exit 1; }

bounded() {
  seconds=$1
  shift
  if [ "${MUHTARI_FORCE_PERL_TIMEOUT-}" != '1' ] && command -v timeout >/dev/null 2>&1; then
    timeout --signal=TERM --kill-after=5 "$seconds" "$@"
    return
  fi
  perl -e '
    my ($seconds, @command) = @ARGV;
    my $pid = fork();
    defined $pid or die "fork: $!\n";
    if ($pid == 0) { exec { $command[0] } @command; die "exec: $!\n"; }
    $SIG{ALRM} = sub {
      kill "TERM", $pid;
      select undef, undef, undef, 2;
      kill "KILL", $pid;
      waitpid($pid, 0);
      exit 124;
    };
    alarm $seconds;
    my $waited = waitpid($pid, 0);
    if ($waited != $pid) {
      warn "waitpid failed: $!\n";
      exit 125;
    }
    my $status = $?;
    exit(128 + ($status & 127)) if ($status & 127);
    exit(($status >> 8) & 255);
  ' "$seconds" "$@"
}

assert_perl_timeout_status() {
  expected=$1
  shift
  if MUHTARI_FORCE_PERL_TIMEOUT=1 bounded "$@" >/dev/null 2>&1; then
    actual=0
  else
    actual=$?
  fi
  [ "$actual" -eq "$expected" ] ||
    fail "Perl timeout fallback returned $actual; expected $expected"
}

check_perl_timeout_fallback() {
  command -v perl >/dev/null 2>&1 || fail 'Perl is required to exercise the bounded-runner fallback'
  assert_perl_timeout_status 0 5 sh -c 'exit 0'
  assert_perl_timeout_status 37 5 sh -c 'exit 37'
  assert_perl_timeout_status 143 5 sh -c 'kill -TERM "$$"'
  assert_perl_timeout_status 124 1 sh -c 'exec sleep 3'
  echo 'Perl timeout fallback controls passed (normal, nonzero, signal and timeout).'
}

test -x build/ci/tests/muhtari-test-libretro-observe || fail 'build/ci observer is missing; run the native ci build first'
test -f build/ci/tests/input-carts/input.a78 || fail 'generated input fixture is missing; run input.cart_files first'
test -f build/ci/tests/emscripten/native.json || fail 'native numeric observer receipt is missing; run libretro.observe first'
docker version >/dev/null 2>&1 || fail 'Docker client/daemon is unavailable'
docker pull --help 2>&1 | grep -q -- '--platform' || fail 'Docker pull lacks platform selection'
daemon_os=$(docker info --format '{{.OSType}}' 2>/dev/null) || fail 'Docker daemon information is unavailable'
[ "$daemon_os" = 'linux' ] || fail "Docker daemon must run Linux containers (found $daemon_os)"
space_kb=$(df -Pk "$root_dir" | awk 'NR == 2 { print $4 }')
case $space_kb in ''|*[!0-9]*) fail 'cannot determine free disk space' ;; esac
[ "$space_kb" -ge 3000000 ] || fail 'at least 3 GB of free disk space is required for this build'

image_digests=$(docker image inspect --format '{{range .RepoDigests}}{{println .}}{{end}}' "$image" 2>/dev/null) || {
  [ "$mode" = 'run' ] || fail 'pinned emsdk image is not present locally'
  echo 'Pulling the exact pinned Emscripten image for linux/amd64.'
  bounded 1200 docker pull --platform "$platform" "$image"
  image_digests=$(docker image inspect --format '{{range .RepoDigests}}{{println .}}{{end}}' "$image")
}
printf '%s\n' "$image_digests" | grep -Fxq "$image" || fail 'local Docker image does not retain the exact pinned index digest'

tool_report=$(docker run --rm --network none --platform "$platform" "$image" sh -c \
  'emcc --version | head -1; node --version; cmake --version | head -1; uname -sm; command -v make') || fail 'pinned emsdk toolchain qualification failed'
printf '%s\n' "$tool_report" | grep -q '7\.0\.0' || fail 'pinned emcc is not version 7.0.0'
printf '%s\n' "$tool_report" | grep -q '^v24\.' || fail 'pinned image Node runtime identity changed'
printf '%s\n' "$tool_report" | grep -q 'Linux x86_64' || fail 'selected image leaf is not Linux x86_64'
if [ ! -x build/pins/cmake/bin/cmake ]; then
  [ "$mode" = 'run' ] || fail 'verified CMake 4.4.3 pin is missing; run the full entry point to fetch its existing pin'
  sh scripts/fetch-pin.sh cmake-linux-x86_64 build/pins/cmake
fi
pinned_cmake=$(docker run --rm --network none --platform "$platform" \
  -v "$root_dir:/src:ro" -w /src "$image" sh -c \
  'PATH="/src/build/pins/cmake/bin:$PATH" cmake --version | head -1') || fail 'pinned CMake cannot run in the selected container'
printf '%s\n' "$pinned_cmake" | grep -q '4\.4\.3' || fail 'existing CMake pin is not version 4.4.3'

# Qualify the exact upstream notices before compiling or running tests. The
# checker owns the literal expected hashes used again when creating/validating
# the archive, so a caller cannot replace the production digest table.
mkdir -p build/emscripten
bounded 1200 docker run --rm --network none --platform "$platform" \
  -v "$root_dir:/src:ro" \
  -v "$root_dir/build/emscripten:/src/build/emscripten:rw" \
  -w /src "$image" sh -c '
    sdk_root=$(dirname "$(command -v emcc)")
    PATH="/src/build/pins/cmake/bin:$PATH" cmake \
      -DROOT=/src -DBUILD=/src/build/emscripten \
      -DWORK=/src/build/emscripten/notice-preflight \
      -DSDK_ROOT="$sdk_root" -DMODE=licenses \
      -P tests/emscripten/bundle.cmake
  ' || fail 'pinned SDK notice qualification failed'

if [ "$mode" = 'preflight' ]; then
  echo 'Emscripten preflight passed: exact SDK index, amd64 leaf, bundled Node, pinned CMake, fixture and native evidence are available.'
  exit 0
fi

check_perl_timeout_fallback
cmake --build --preset ci --target muhtari-test-libretro-observe muhtari-test-input
ctest --test-dir build/ci --output-on-failure -R '^input\.cart_files$' --no-tests=error
mkdir -p build/ci/tests/emscripten
build/ci/tests/muhtari-test-libretro-observe \
  --fixture build/ci/tests/input-carts/input.a78 \
  --output build/ci/tests/emscripten/native.json

host_description="$(uname -sm); Docker daemon ${daemon_os}/$(docker info --format '{{.Architecture}}'); selected container leaf linux/amd64"
expected_sha=$(git -C "$root_dir" rev-parse --verify HEAD) || fail 'cannot resolve the exact checkout commit'
container_name="muhtari-emscripten-$$"
mkdir -p build/emscripten build/ci/tests/emscripten
rm -f build/emscripten/junit.xml \
  build/ci/tests/emscripten/measurement.json \
  build/ci/tests/emscripten/wasm-surface.json
cleanup_container() { docker rm -f "$container_name" >/dev/null 2>&1 || true; }
trap cleanup_container 0 HUP INT TERM
bounded 1200 docker run --rm --name "$container_name" --network none --platform "$platform" \
  -e "MUHTARI_HOST_DESCRIPTION=$host_description" \
  -e "MUHTARI_EXPECTED_SHA=$expected_sha" \
  -v "$root_dir:/src:ro" \
  -v "$root_dir/build/emscripten:/src/build/emscripten:rw" \
  -v "$root_dir/build/ci/tests/emscripten:/src/build/ci/tests/emscripten:rw" \
  -w /src "$image" sh -c '
    if touch /src/.emscripten-readonly-probe 2>/dev/null; then
      rm -f /src/.emscripten-readonly-probe
      echo "source mount unexpectedly permits writes" >&2
      exit 1
    fi
    test -w /src/build/emscripten || { echo "Emscripten build mount is not writable" >&2; exit 1; }
    test -w /src/build/ci/tests/emscripten || { echo "Emscripten evidence mount is not writable" >&2; exit 1; }
    PATH="/src/build/pins/cmake/bin:$PATH" cmake --workflow --preset emscripten &&
    PATH="/src/build/pins/cmake/bin:$PATH" cmake \
      -DJUNIT=build/emscripten/junit.xml \
      -DINVENTORY=tests/inventory/emscripten.txt \
      -P tests/cmake/check_junit.cmake &&
    node tests/emscripten/host.mjs \
      --module build/emscripten/libretro/muhtari_libretro.mjs \
      --fixture build/ci/tests/input-carts/input.a78 \
      --native build/ci/tests/emscripten/native.json \
      --mode report --report build/ci/tests/emscripten/measurement.json
  '
trap - 0 HUP INT TERM
cleanup_container

test -s build/ci/tests/emscripten/measurement.json || fail 'fixed-workload measurement was not produced'
test -s build/ci/tests/emscripten/wasm-surface.json || fail 'actual Wasm import/export evidence was not produced'
test -s build/emscripten/junit.xml || fail 'required Emscripten JUnit inventory was not produced'
echo 'Emscripten artifact, no-skip test inventory, import/export surface and measurement passed.'
