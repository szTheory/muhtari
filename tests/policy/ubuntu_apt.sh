#!/bin/sh
# Exercise the real mirror transformation and failure propagation without sudo,
# network access or a dependency on the test machine's package manager.
set -eu
driver=$1
work=$2
mkdir -p "$work/bin"
work=$(cd "$work" && pwd)
export APT_FIXTURE="$work/mirrors" APT_CALLS="$work/calls"
PATH="$work/bin:$PATH"
export PATH
cat > "$work/bin/sudo" <<'EOF'
#!/bin/sh
set -eu
case "$1" in
  sh)
    [ "$#" -eq 6 ]
    [ "$2" = -eu ] && [ "$3" = -c ] && [ "$5" = sh ]
    [ "$6" = /etc/apt/apt-mirrors.txt ]
    exec sh -eu -c "$4" sh "$APT_FIXTURE"
    ;;
  apt-get) shift; exec apt-get "$@" ;;
  *) exit 99 ;;
esac
EOF
cat > "$work/bin/apt-get" <<'EOF'
#!/bin/sh
set -eu
printf '%s\n' "$@" >> "$APT_CALLS"
if [ "${APT_FAILURE:-}" = "$1" ]; then exit 42; fi
EOF
chmod +x "$work/bin/sudo" "$work/bin/apt-get"

reset_fixture() {
  printf 'http://azure.archive.ubuntu.com/ubuntu/\tpriority:1\nhttps://archive.ubuntu.com/ubuntu/\tpriority:2\nhttps://security.ubuntu.com/ubuntu/\tpriority:3\n' > "$APT_FIXTURE"
  : > "$APT_CALLS"
}
expect_exit() {
  expected=$1
  shift
  actual=0
  "$@" > "$work/output" 2>&1 || actual=$?
  if [ "$actual" -ne "$expected" ]; then
    cat "$work/output" >&2
    echo "ubuntu apt: expected exit $expected, got $actual" >&2
    exit 1
  fi
}

reset_fixture
expect_exit 0 sh "$driver" valgrind gcc-powerpc-linux-gnu
printf 'https://mirrors.edge.kernel.org/ubuntu/\tpriority:0\nhttp://azure.archive.ubuntu.com/ubuntu/\tpriority:1\nhttps://archive.ubuntu.com/ubuntu/\tpriority:2\nhttps://security.ubuntu.com/ubuntu/\tpriority:3\n' > "$work/expected-mirrors"
cmp "$APT_FIXTURE" "$work/expected-mirrors"
printf '%s\n' update --error-on=any install -y --no-install-recommends valgrind gcc-powerpc-linux-gnu > "$work/expected-calls"
cmp "$APT_CALLS" "$work/expected-calls"

reset_fixture
export APT_FAILURE=update
expect_exit 42 sh "$driver" valgrind
printf '%s\n' update --error-on=any > "$work/expected-calls"
cmp "$APT_CALLS" "$work/expected-calls"

reset_fixture
export APT_FAILURE=install
expect_exit 42 sh "$driver" valgrind
unset APT_FAILURE

reset_fixture
rm "$APT_FIXTURE"
expect_exit 1 sh "$driver" valgrind
test ! -s "$APT_CALLS"

reset_fixture
expect_exit 2 sh "$driver"
test ! -s "$APT_CALLS"
echo 'ubuntu apt: signed-source mirror routing and setup failure controls passed'
