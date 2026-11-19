#!/bin/sh
# Hosted runners can fail DNS for every Ubuntu-owned mirror while kernel.org
# remains reachable. Keep the image's signed sources and original fallbacks.
set -eu
[ "$#" -gt 0 ] || { echo 'usage: ubuntu-apt.sh PACKAGE...' >&2; exit 2; }

sudo sh -eu -c '
  mirror=$1
  temporary=$(mktemp)
  trap '\''rm -f "$temporary"'\'' EXIT HUP INT TERM
  printf "https://mirrors.edge.kernel.org/ubuntu/\tpriority:0\n" > "$temporary"
  cat "$mirror" >> "$temporary"
  cat "$temporary" > "$mirror"
' sh /etc/apt/apt-mirrors.txt

# Retry transient mirror fetch failures, but fail if any index remains stale.
sudo apt-get -o Acquire::Retries=2 update --error-on=any
sudo apt-get -o Acquire::Retries=2 install -y --no-install-recommends "$@"
