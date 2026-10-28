#!/bin/sh
# Fault prevented: the history scan failing a commit that only deletes a listed
# binary and its manifest row, or passing a commit that adds an unlisted one.
# usage: history_control.sh HYGIENE_SH WORKDIR
set -eu
hygiene=$1
work=$2
rm -rf "$work"
mkdir -p "$work/tests/roms"
cd "$work"
git init -q .
git config user.email fixture@users.noreply.github.com
git config user.name fixture
git config commit.gpgsign false
printf '\000\001' > fixture.dat
printf 'fixture.dat\tthis-repo\tx\tMIT\t0\t-\n' > tests/roms/manifest.txt
git add .
git commit -q -m add
base=$(git rev-parse HEAD)
git rm -q fixture.dat
: > tests/roms/manifest.txt
git add -A
git commit -q -m delete
sh "$hygiene" --history "$base..HEAD"
printf '\000\002' > stray.dat
git add stray.dat
git commit -q -m stray
if sh "$hygiene" --history HEAD~1..HEAD 2> err; then
  echo "history_control: an unlisted binary was accepted" >&2
  exit 1
fi
grep -q 'binary file not in manifest' err
