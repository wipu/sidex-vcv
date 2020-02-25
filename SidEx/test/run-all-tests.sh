#!/bin/bash

set -euo pipefail

HERE=$(dirname "$0")
HERE=$(readlink -f "$HERE")

. "$HERE"/../../functions.sh

log() {
    echo "$@" >&2
}

TMP=/tmp/sidex-test
rm -rf "$TMP"
mkdir -p "$TMP"

TESTER="$TMP"/tester
COMMANDS="$TMP"/commands
ACTUAL_OUT="$TMP"/actual-out
mkdir -p "$ACTUAL_OUT"

log Compiling
c++  -std=c++11 -o "$TESTER" -I"$HERE"/../src -I"$RACK"/include -I"$RACK"/dep/include "$HERE"/convertertester.cpp
log Compiled

test-start() {
    TESTNAME=$1
    echo "!" > "$COMMANDS"
    log
    log ">>> $TESTNAME"
}

cmd() {
    local NAME=$1
    local VALUE=$2
    cat >> "$COMMANDS" <<EOF
$NAME
$VALUE
EOF
}

comment() {
    echo "// $@" >> "$COMMANDS"
}

test-end() {
    cat "$COMMANDS" | "$TESTER" > "$ACTUAL_OUT"/$TESTNAME
    log "<<< ($TESTNAME)"
    log
}

step() {
    cmd convert 1234567890
}

test-start freq1-many_values
cmd freq1 -5
step
cmd freq1 0
step
cmd freq1 2.5
step
cmd freq1 5
step
cmd freq1 7.5
step
cmd freq1 10
step
test-end

test-start freq2-5v
cmd freq2 5
step
test-end

test-start freq3-5v
cmd freq3 5
step
test-end

test-start gate1-many_values
cmd gate1 -1
step
cmd gate1 0
step
cmd gate1 0.9
step
cmd gate1 1
step
cmd gate1 1.9
step
comment 'gate 2 => up:'
cmd gate1 2
step
comment 'gate 1.9 => stays up:'
cmd gate1 1.9
step
comment 'gate 0.9 => down:'
cmd gate1 0.9
step
test-end

test-start gate2
cmd gate2 0
step
cmd gate2 5
step
test-end

test-start gate3
cmd gate3 0
step
cmd gate3 5
step
test-end

test-start modevol
comment vol -1
cmd master_volume -1
step
comment vol 0
cmd master_volume 0
step
comment vol 5
cmd master_volume 5
step
comment vol 10
cmd master_volume 10
step
comment filter_mode .3
cmd filter_mode .3
step
test-end

test-start adsr1
comment half
cmd a1 5
cmd d1 5
cmd s1 5
cmd r1 5
step
comment full
cmd a1 10
cmd d1 10
cmd s1 10
cmd r1 10
step
comment "2,4,6,8"
cmd a1 2
cmd d1 4
cmd s1 6
cmd r1 8
step
test-end

test-start adsr2
comment "2,4,6,8"
cmd a2 2
cmd d2 4
cmd s2 6
cmd r2 8
step
test-end

test-start adsr3
comment "2,4,6,8"
cmd a3 2
cmd d3 4
cmd s3 6
cmd r3 8
step
test-end

test-start wf1
comment tri on
cmd wf_tri_1 5
step
comment +saw on
cmd wf_saw_1 5
step
comment +pul on
cmd wf_pul_1 5
step
comment +noi on
cmd wf_noi_1 5
step
comment +gate on
cmd gate1 5
step
test-end

test-start wf2
comment tri on
cmd wf_tri_2 5
step
comment +saw on
cmd wf_saw_2 5
step
comment +pul on
cmd wf_pul_2 5
step
comment +noi on
cmd wf_noi_2 5
step
comment +gate on
cmd gate2 5
step
test-end

test-start wf3
comment tri on
cmd wf_tri_3 5
step
comment +saw on
cmd wf_saw_3 5
step
comment +pul on
cmd wf_pul_3 5
step
comment +noi on
cmd wf_noi_3 5
step
comment +gate on
cmd gate3 5
step
test-end

test-start cutoff
comment -1
cmd cutoff -1
step
comment .1
cmd cutoff .1
step
comment 1
cmd cutoff 1
step
comment 5
cmd cutoff 5
step
comment 10
cmd cutoff 10
step
test-end

test-start resfilt
comment reso -5
cmd reso -5
step
comment reso 2
cmd reso 2
step
comment reso 5
cmd reso 5
step
comment reso 10
cmd reso 10
step
comment filtN 5 0 0 0
cmd filt1 5
cmd filt2 0
cmd filt3 0
cmd filtExt 0
step
comment filtN 0 5 0 0
cmd filt1 0
cmd filt2 5
cmd filt3 0
cmd filtExt 0
step
comment filtN 0 0 5 0
cmd filt1 0
cmd filt2 0
cmd filt3 5
cmd filtExt 0
step
comment filtN 0 0 0 5
cmd filt1 0
cmd filt2 0
cmd filt3 0
cmd filtExt 5
step
test-end

test-start pw1
comment -6
cmd pw1 -6
step
comment -5
cmd pw1 -5
step
comment -4.9
cmd pw1 -4.9
step
comment -1
cmd pw1 -1
step
comment 0
cmd pw1 0
step
comment 1
cmd pw1 1
step
comment 6
cmd pw1 6
step
test-end

test-start pw2
comment -5
cmd pw2 -5
step
comment 6
cmd pw2 6
step
test-end

test-start pw3
comment -5
cmd pw3 -5
step
comment 6
cmd pw3 6
step
test-end

diff -r "$HERE"/expected-out "$ACTUAL_OUT" || {
    log "Tests failed. You may want to run:"
    log "meld $HERE/expected-out $ACTUAL_OUT"
    exit 1
}

log "All tests passed :D"
