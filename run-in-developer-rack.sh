#!/bin/bash

set -euo pipefail

HERE=$(dirname "$0")
HERE=$(readlink -f "$HERE")

RACK_COMMIT=652b4449548c4a2ddb9dd6cb955bcf5d352d8031
FUNDAMENTAL_COMMIT=eb41dd7cdf1112530c1a8059cfa3a99a402944ef

# TODO use official Rack or even SDK when it contains sysex send:
RACK=/tmp/Rack-$RACK_COMMIT
[ -e "$RACK".done ] || {
    rm -rf "$RACK"
    git clone https://github.com/wipu/Rack.git "$RACK"
    cd "$RACK"
    git checkout "$RACK_COMMIT"
    git submodule update --init --recursive
    make -j$(nproc) dep
    cd plugins
    git clone https://github.com/VCVRack/Fundamental.git
    cd Fundamental
    git checkout "$FUNDAMENTAL_COMMIT"
    git submodule update --init --recursive
    make -j$(nproc)
    touch "$RACK".done
}

cd "$RACK"
make -j$(nproc)

DEST=$RACK/plugins/SidEx
rm -rf "$DEST"
cp -a "$HERE"/SidEx "$DEST"
cd "$DEST"
make -j$(nproc)

cd "$RACK"
make run
