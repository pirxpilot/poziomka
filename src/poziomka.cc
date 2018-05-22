#include <leveldb/write_batch.h>

#include "poziomka.h"

#include "workers.h"

static inline leveldb::Slice MakeSlice(Napi::Value from) {
  if (from.IsNull() || from.IsUndefined()) return leveldb::Slice();

  auto buffer = from.As<Napi::Buffer<char>>();
  return leveldb::Slice(buffer.Data(), buffer.Length());
}

Poziomka::Poziomka(const Napi::CallbackInfo& info) : ObjectWrap(info) {
  auto env = info.Env();

  if (info.Length() < 1) {
    Napi::TypeError::New(env, "Wrong number of arguments")
      .ThrowAsJavaScriptException();
    return;
  }

  if (!info[0].IsString()) {
    Napi::TypeError::New(env, "Need database location")
      .ThrowAsJavaScriptException();
    return;
  }

  options.create_if_missing = true;
  location = info[0].As<Napi::String>().Utf8Value();
}

void Poziomka::Open(const Napi::CallbackInfo& info) {
  auto fn = info[0].As<Napi::Function>();

  (new OpenWorker(fn, *this))->Queue();
}

void Poziomka::Close(const Napi::CallbackInfo& info) {
  auto fn = info[0].As<Napi::Function>();

  (new CloseWorker(fn, *this))->Queue();
}

void Poziomka::GetMany(const Napi::CallbackInfo& info) {
  auto keys = info[0].As<Napi::Array>();
  auto fn = info[1].As<Napi::Function>();

  const auto len = keys.Length();

  std::vector<leveldb::Slice> slices;
  slices.reserve(len);

  for(auto i = 0; i < len; i++) {
    auto key = MakeSlice(keys.Get(i));
    slices.push_back(key);
  }

  (new GetWorker(fn, *db, std::move(slices)))->Queue();
}

void Poziomka::PutMany(const Napi::CallbackInfo& info) {
  auto keys = info[0].As<Napi::Array>();
  auto values = info[1].As<Napi::Array>();
  auto fn = info[2].As<Napi::Function>();

  auto batch = new leveldb::WriteBatch();

  const auto len = keys.Length();
  for (auto i = 0; i < len; i++) {
    auto key = MakeSlice(keys.Get(i));
    auto value = MakeSlice(values.Get(i));

    batch->Put(key, value);
  }

  (new BatchWorker(fn, *db, batch))->Queue();
}

void Poziomka::RemoveMany(const Napi::CallbackInfo& info) {
  auto keys = info[0].As<Napi::Array>();
  auto fn = info[1].As<Napi::Function>();

  auto batch = new leveldb::WriteBatch();

  const auto len = keys.Length();
  for (auto i = 0; i < len; i++) {
    auto key = MakeSlice(keys.Get(i));

    batch->Delete(key);
  }

  (new BatchWorker(fn, *db, batch))->Queue();
}

Napi::Function Poziomka::GetClass(Napi::Env env) {
  return DefineClass(env, "Poziomka", {
    Poziomka::InstanceMethod("open", &Poziomka::Open),
    Poziomka::InstanceMethod("close", &Poziomka::Close),
    Poziomka::InstanceMethod("getMany", &Poziomka::GetMany),
    Poziomka::InstanceMethod("putMany", &Poziomka::PutMany),
    Poziomka::InstanceMethod("removeMany", &Poziomka::RemoveMany),
  });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  auto name = Napi::String::New(env, "Poziomka");
  exports.Set(name, Poziomka::GetClass(env));
  return exports;
}

NODE_API_MODULE(addon, Init)
