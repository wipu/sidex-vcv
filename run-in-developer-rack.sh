#!/bin/bash

set -euo pipefail

HERE=$(dirname "$0")
HERE=$(readlink -f "$HERE")

. "$HERE"/functions.sh

cd "$RACK"
make -j$(nproc)

DEST=$RACK/plugins/SidEx
rm -rf "$DEST"
cp -a "$HERE"/SidEx "$DEST"
cd "$DEST"
make -j$(nproc)

cd "$RACK"
make run
