#! /bin/sh

chmod a+x clear_cmake.sh
./clear_cmake.sh

mkdir build && cd build
cmake .. && make -j8
