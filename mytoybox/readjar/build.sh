#!/bin/bash

rm CMakeCache.txt

cmake -DBOOST_INCLUDEDIR=~/Downloads/boost_1_69_0 \
-DBOOST_INCLUDEDIR=~/Downloads/boost_1_69_0 \
-DBOOST_ROOT=~/Downloads/boost_1_69_0 \
-DZLIB_ROOT=~/oss/zlib \
 -DwxWidgets_CONFIG_EXECUTABLE=~/Downloads/wxWidgets-3.1.2/bld/wx-config ..
 
cmake --build .