cmake_minimum_required(VERSION 2.8)

# Name of the project (will be the name of the plugin)
project(poziomka)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_definitions(-fno-builtin-memcmp)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME}
  PRIVATE
    "${PROJECT_SOURCE_DIR}/src/poziomka.cc"
    "${PROJECT_SOURCE_DIR}/src/poziomka.h"
    "${PROJECT_SOURCE_DIR}/src/workers.cc"
    "${PROJECT_SOURCE_DIR}/src/workers.h"
)

# Gives our library file a .node extension without any "lib" prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Essential include files to build a node addon,
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})

# Essential library files to link to a node addon
target_link_libraries(
  ${PROJECT_NAME}
  ${CMAKE_JS_LIB}
)

# crc32c

set(CRC32C_BUILD_TESTS OFF CACHE BOOL "Build CRC32C's unit tests")
set(CRC32C_BUILD_BENCHMARKS OFF CACHE BOOL "Build CRC32C's benchmarks")
set(CRC32C_USE_GLOG OFF CACHE BOOL "Build CRC32C's tests with Google Logging")
set(CRC32C_INSTALL OFF CACHE BOOL "Install CRC32C's header and library")

set(HAVE_CRC32C ON CACHE BOOL "CRC32C is build here")

add_subdirectory(deps/crc32c)

include_directories(SYSTEM "${PROJECT_SOURCE_DIR}/deps/crc32c/include")
target_link_libraries(${PROJECT_NAME} crc32c)

# snappy

set(SNAPPY_BUILD_TESTS CACHE BOOL "Don not Build Snappy's own tests.")

set(HAVE_SNAPPY ON CACHE BOOL "snappy is build also")

add_subdirectory(deps/snappy)

include_directories(SYSTEM "${PROJECT_SOURCE_DIR}/deps/snappy")
target_link_libraries(${PROJECT_NAME} snappy)

# leveldb

set(LEVELDB_BUILD_TESTS OFF CACHE BOOL "do not build benchmarks")
set(LEVELDB_BUILD_BENCHMARKS OFF CACHE BOOL "do not build benchmarks")
set(LEVELDB_INSTALL OFF CACHE BOOL "do not install")

add_subdirectory(deps/leveldb)

target_link_libraries(${PROJECT_NAME} leveldb)
