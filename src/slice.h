#pragma once

#include <napi.h>
#include <leveldb/slice.h>

struct Slice: public leveldb::Slice {
  explicit Slice(Napi::Buffer<char> buffer):
    leveldb::Slice(buffer.Data(), buffer.Length())
    {}

  explicit Slice(Napi::Value from):
    Slice(from.As<Napi::Buffer<char>>())
    {}
};
