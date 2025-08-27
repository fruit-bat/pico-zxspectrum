mkdir build
cd build

# $1 board
# $2 platform
build_group() {
  mkdir -p "$1_$2"
  pushd "$1_$2"
  cmake -DPICO_COPY_TO_RAM=0 -DPICO_PLATFORM=$2 -DPICO_BOARD=$1 ../..
  make -j4 relevant
  find . -name '*.uf2' -exec cp '{}' "../../uf2-$2" \;
  popd
}

# Pimoroni Pico VGA Demo Base with a Pico RP2040
build_group vgaboard rp2040
# Pimoroni Pico VGA Demo Base with a Pico2 RP2350 (Untested)
build_group vgaboard rp2350-arm-s
# Adafruit feather with built in RP2040
build_group adafruit_feather_rp2040 rp2040
# Various Pi Pico RP2040 boards
build_group pico rp2040
# Various Pi Pico2 RP2350 boards
build_group pico2 rp2350-arm-s
