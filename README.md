# Libnesfrontend

This is a small wrapper library used to remote control NES games.

This is used in [pyneat](https://github.com/rugo/pyneat) to let a neuronal network play.

## How to use
This was hacked and used on and for my Linux machine. Feel free to port.

To use this lib on Linux you need to get liballegro5 and the libretro core QuickNES.

Here is how to do it on Debian:

```bash
git clone https://github.com/rugo/libnesfrontend
cd libnesfrontend
apt install liballegro5.0 # install allegro
# get QuickNES
git clone https://github.com/libretro/QuickNES_Core.git quicknes
# Compile QuickNES
cd quicknes 
make -j
# Rename it
mv quicknes_libretro.so libretrones.so
cd .. # go to libnesfrontend folder
# Build libnesfrontend
mkdir build && cd build
cmake .. && make
./main_test # start the test program
```