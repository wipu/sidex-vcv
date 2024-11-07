# to be sourced

RACK_COMMIT=652b4449548c4a2ddb9dd6cb955bcf5d352d8031
FUNDAMENTAL_COMMIT=eb41dd7cdf1112530c1a8059cfa3a99a402944ef

ZLIB_URL=https://zlib.net/fossils/zlib-1.2.11.tar.gz
PFFFT_VER=fbc405860280
PFFFT_SHA=03294be29a772ac515d7d5f07bf475b10fe644df132ff56dee4a18338e767533

# TODO use official Rack or even SDK when it contains sysex send:
RACK=/tmp/Rack-$RACK_COMMIT
[ -e "$RACK".done ] || {
    rm -rf "$RACK"
    git clone https://github.com/wipu/Rack.git "$RACK"
    cd "$RACK"
    git checkout "$RACK_COMMIT"

    sed -i "s|https://www.zlib.net/zlib-1.2.11.tar.gz|$ZLIB_URL|" dep/Makefile
    sed -i "s|29e4f76ac53b|$PFFFT_VER|g" dep/Makefile
    sed -i "s|bb10afba127904a0c6c553fa445082729b7d72373511bda1b12a5be0e03f318a|$PFFFT_SHA|" dep/Makefile

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
