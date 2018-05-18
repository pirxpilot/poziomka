cmake_minimum_required(VERSION 2.8)

include(ExternalProject)

# Name of the project (will be the name of the plugin)
project(poziomka)

# crc32c

# set(CRC32C_BUILD_TESTS OFF CACHE BOOL "Build CRC32C's unit tests")
# set(CRC32C_BUILD_BENCHMARKS OFF CACHE BOOL "Build CRC32C's benchmarks")
# set(CRC32C_USE_GLOG OFF CACHE BOOL "Build CRC32C's tests with Google Logging")
# set(CRC32C_INSTALL OFF CACHE BOOL "Install CRC32C's header and library")

# set(HAVE_CRC32C ON CACHE BOOL "CRC32C is build here")

# set_target_properties(crc32c PROPERTIES COMPILE_FLAGS "-fPIC -fno-builtin-memcmp")
# set_target_properties(crc32c_sse42 PROPERTIES COMPILE_FLAGS "-fPIC -fno-builtin-memcmp")
# set_target_properties(crc32c_arm64 PROPERTIES COMPILE_FLAGS "-fPIC -fno-builtin-memcmp")

# ExternalProject_Add(crc32c
#   GIT_REPOSITORY https://github.com/google/crc32c
#   GIT_TAG 1.0.5
#   CMAKE_CACHE_ARGS
#     -DCRC32C_BUILD_TESTS:STRING=OFF
#     -DCRC32C_BUILD_BENCHMARKS:STRING=OFF
#     -DCRC32C_USE_GLOG:STRING=OFF
# )

# ExternalProject_Get_Property(crc32c INSTALL_DIR)

# add_library(tcmalloc STATIC IMPORTED)
# set_property(TARGET tcmalloc PROPERTY IMPORTED_LOCATION ${INSTALL_DIR}/lib/libtcmalloc_minimal.a)

# add_dependencies(tcmalloc gperftools)

# snappy

# set(SNAPPY_BUILD_TESTS CACHE BOOL "Don not Build Snappy's own tests.")

# set(HAVE_SNAPPY ON CACHE BOOL "snappy is build also")

# ExternalProject_Add(snappy
#   GIT_REPOSITORY https://github.com/google/snappy
#   GIT_TAG 1.1.7
# )

# set_target_properties(snappy PROPERTIES COMPILE_FLAGS "-fPIC -fno-builtin-memcmp")

# tcmalloc


ExternalProject_Add(gperftools
  GIT_REPOSITORY https://github.com/gperftools/gperftools.git
  GIT_TAG gperftools-2.7
  UPDATE_COMMAND autoreconf --install
  CONFIGURE_COMMAND <SOURCE_DIR>/configure
    --prefix=${PROJECT_BINARY_DIR}/install
    --disable-heap-checker
    --disable-heap-profiler
    --disable-dependency-tracking
    --disable-debugalloc
    --disable-cpu-profiler
    --disable-shared
    --with-pic
  BUILD_COMMAND make
  INSTALL_COMMAND make install
)

ExternalProject_Get_Property(gperftools INSTALL_DIR)

add_library(tcmalloc STATIC IMPORTED)
set_property(TARGET tcmalloc PROPERTY IMPORTED_LOCATION ${PROJECT_BINARY_DIR}/install/lib/libtcmalloc_minimal.a)

add_dependencies(tcmalloc gperftools)

# set(HAVE_TCMALLOC ON CACHE BOOL "tcmalloc is being built here")


# leveldb

set(LEVELDB_BUILD_TESTS OFF CACHE BOOL "do not build benchmarks")
set(LEVELDB_BUILD_BENCHMARKS OFF CACHE BOOL "do not build benchmarks")
set(LEVELDB_INSTALL OFF CACHE BOOL "do not install")

#    -DHAVE_TCMALLOC:STRING=ON


# GIT_TAG v1.20
ExternalProject_Add(leveldb-project
  GIT_REPOSITORY https://github.com/google/leveldb
  CMAKE_CACHE_ARGS
    -DCMAKE_PREFIX_PATH:STRING=${PROJECT_BINARY_DIR}/install
    -DCMAKE_INSTALL_PREFIX:STRING=${PROJECT_BINARY_DIR}/install
    -DLEVELDB_BUILD_TESTS:STRING=OFF
    -DLEVELDB_BUILD_BENCHMARKS:STRING=OFF
)

ExternalProject_Get_Property(leveldb-project INSTALL_DIR)

add_library(leveldb STATIC IMPORTED)
set_property(TARGET leveldb PROPERTY IMPORTED_LOCATION ${PROJECT_BINARY_DIR}/install/lib/libleveldb.a)

add_dependencies(leveldb leveldb-project)


# set_target_properties(leveldb PROPERTIES COMPILE_FLAGS "-fPIC -fno-builtin-memcmp")

# include_directories(
#   SYSTEM
#   "${PROJECT_SOURCE_DIR}/lib/crc32c/include"
#   "${PROJECT_SOURCE_DIR}/lib/snappy"
#   "${PROJECT_SOURCE_DIR}/lib/leveldb/include"
# )

# Build a shared library named after the project from the files in `src/`
add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME}
  PRIVATE
    "${PROJECT_SOURCE_DIR}/src/poziomka.h"
    "${PROJECT_SOURCE_DIR}/src/poziomka.cc"
)

# Gives our library file a .node extension without any "lib" prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Essential include files to build a node addon,
# You should add this line in every CMake.js based project
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})

# Essential library files to link to a node addon
# You should add this line in every CMake.js based project
target_link_libraries(
  ${PROJECT_NAME}
  ${CMAKE_JS_LIB}
  # crc32c
  # snappy
  leveldb
)
