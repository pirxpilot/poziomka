#pragma once

#include <nan.h>
#include <leveldb/slice.h>

struct Slice: public leveldb::Slice {
  explicit Slice(v8::Local<v8::Object> o):
    leveldb::Slice(node::Buffer::Data(o), node::Buffer::Length(o))
    {}

  explicit Slice(Nan::MaybeLocal<v8::Value> from):
    Slice(Nan::To<v8::Object>(from.ToLocalChecked()).ToLocalChecked())
    {}

};
