cmake_minimum_required(VERSION 2.8)

include(ExternalProject)

# Name of the project (will be the name of the plugin)
project(poziomka)

set(MAIN_INSTALL_DIR ${PROJECT_BINARY_DIR}/install)

# crc32c

# set(HAVE_CRC32C ON CACHE BOOL "CRC32C is build here")

# set_target_properties(crc32c PROPERTIES COMPILE_FLAGS "-fPIC -fno-builtin-memcmp")
# set_target_properties(crc32c_sse42 PROPERTIES COMPILE_FLAGS "-fPIC -fno-builtin-memcmp")
# set_target_properties(crc32c_arm64 PROPERTIES COMPILE_FLAGS "-fPIC -fno-builtin-memcmp")

ExternalProject_Add(crc32c-project
  GIT_REPOSITORY https://github.com/google/crc32c
  GIT_TAG 1.0.5
  CMAKE_CACHE_ARGS
    -DCMAKE_INSTALL_PREFIX:STRING=${MAIN_INSTALL_DIR}
    -DCRC32C_BUILD_TESTS:BOOL=OFF
    -DCRC32C_BUILD_BENCHMARKS:BOOL=OFF
    -DCRC32C_USE_GLOG:BOOL=OFF
)

# ExternalProject_Get_Property(crc32c INSTALL_DIR)

add_library(crc32c STATIC IMPORTED)
set_property(TARGET crc32c PROPERTY IMPORTED_LOCATION ${MAIN_INSTALL_DIR}/lib/libcrc32c.a)

add_dependencies(crc32c crc32c-project)

# snappy

# set(HAVE_SNAPPY ON CACHE BOOL "snappy is build also")

ExternalProject_Add(snappy-project
  GIT_REPOSITORY https://github.com/google/snappy
  GIT_TAG 1.1.7
  CMAKE_CACHE_ARGS
    -DCMAKE_INSTALL_PREFIX:STRING=${MAIN_INSTALL_DIR}
    -DCRC32C_BUILD_TESTS:BOOL=OFF
)

# set_target_properties(snappy PROPERTIES COMPILE_FLAGS "-fPIC -fno-builtin-memcmp")

add_library(snappy STATIC IMPORTED)
set_property(TARGET snappy PROPERTY IMPORTED_LOCATION ${MAIN_INSTALL_DIR}/lib/libsnappy.a)

add_dependencies(snappy snappy-project)


# tcmalloc


ExternalProject_Add(gperftools
  GIT_REPOSITORY https://github.com/gperftools/gperftools.git
  GIT_TAG gperftools-2.7
  UPDATE_COMMAND autoreconf --install
  CONFIGURE_COMMAND <SOURCE_DIR>/configure
    --prefix=${MAIN_INSTALL_DIR}
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
set_property(TARGET tcmalloc PROPERTY IMPORTED_LOCATION ${MAIN_INSTALL_DIR}/lib/libtcmalloc_minimal.a)

add_dependencies(tcmalloc gperftools)

# set(HAVE_TCMALLOC ON CACHE BOOL "tcmalloc is being built here")


# leveldb

# set(LEVELDB_BUILD_TESTS OFF CACHE BOOL "do not build benchmarks")
# set(LEVELDB_BUILD_BENCHMARKS OFF CACHE BOOL "do not build benchmarks")

#    -DHAVE_TCMALLOC:STRING=ON


# GIT_TAG v1.20
ExternalProject_Add(leveldb-project
  GIT_REPOSITORY https://github.com/google/leveldb
  CMAKE_CACHE_ARGS
    -DCMAKE_INSTALL_PREFIX:STRING=${MAIN_INSTALL_DIR}
    -DLEVELDB_BUILD_TESTS:BOOL=OFF
    -DLEVELDB_BUILD_BENCHMARKS:BOOL=OFF
  DEPENDS
    crc32c-project
    snappy-project
    gperftools
)

ExternalProject_Get_Property(leveldb-project INSTALL_DIR)

add_library(leveldb STATIC IMPORTED)
set_property(TARGET leveldb PROPERTY IMPORTED_LOCATION ${MAIN_INSTALL_DIR}/lib/libleveldb.a)

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
  tcmalloc
  crc32c
  snappy
  leveldb
)
