#!/bin/bash
set -ex
mkdir -p build
cd build
sudo apt-get install cmake libsfml-dev libglm-dev libglew-dev libglfw3-dev libconfig++-dev
cmake -Wno-dev ..
make
