
1.0.0 / 2021-07-12
==================

 * upgrade prebuild-install to ~6
 * fix for node >= 13
 * set C++ standard to 14

0.0.9 / 2019-05-11
==================

 * add support for various leveldb options
 * rewrite using NaN to compare performance vs. N-API implementation
 * fix Travis builds by forcing PIC in crc32c

0.0.8 / 2019-05-07
==================

 * update cmake-js
 * build shared libraries by default

0.0.7 / 2019-05-05
==================

 * upgrade cmake-js to 4.2.0
 * switch to building/installing napi binary
 * upgrade prebuild install to ~5

0.0.6 / 2019-05-04
==================

 * upgrade leveldb to 1.22
 * update snappy to latest version 2019-02
 * update crc32 to 1.0.7
 * upgrade node-addon-api to 1.6.3
 * upgrade bindings to 1.5.0

0.0.5 / 2018-05-23
==================

 * add Slice struct to simplify conversion from Napi value to leveldb value
 * CMake - improve handling compiler flags
 * remove node-gyp-build dependency

0.0.4 / 2018-05-22
==================

 * fix builds performed when installed as dependency
 * fix compilation errors for clang

0.0.3 / 2018-05-20
==================

 * upgrade cmake-js to fix build issues during installation
 * use `auto` types where possible

0.0.2 / 2018-05-18
==================

 * fix `addon` target in make
 * add prebuild support

0.0.1 / 2018-05-18
==================

 * initial implementation
