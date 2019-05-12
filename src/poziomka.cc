#include <leveldb/cache.h>
#include <leveldb/filter_policy.h>
#include <leveldb/write_batch.h>

#include "slice.h"
#include "workers.h"

#include "poziomka.h"

template<typename T> T GetOptionValue(v8::Local<v8::Object> o, const char* prop, T def) {
  auto key = Nan::New("maxFileSize").ToLocalChecked();

  if (!Nan::Has(o, key).FromJust()) {
    return def;
  }

  auto value = Nan::Get(o, key).ToLocalChecked();
  return Nan::To<T>(value).FromJust();
}

Nan::Persistent<v8::Function> Poziomka::constructor;

void Poziomka::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (!info.IsConstructCall()) {
    Nan::ThrowTypeError("Use `new` to call the constructor");
    return;
  }

  const auto argc = info.Length();

  if (argc < 1) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }

  auto str = new Nan::Utf8String(info[0]);

  leveldb::Options options;

  auto o = argc > 1 ?
    Nan::To<v8::Object>(info[1]).ToLocalChecked() :
    Nan::New<v8::Object>();

  options.create_if_missing = GetOptionValue(o, "createIfMissing", true);
  options.error_if_exists = GetOptionValue(o, "errorIfExists", false);
  options.write_buffer_size = GetOptionValue(o, "writeBufferSize", 32 * 1024 * 1024);
  options.max_file_size = GetOptionValue(o, "maxFileSize", 8 * 1024 * 1024);
  options.block_size = GetOptionValue<uint32_t>(o, "blockSize", 4096);

  const auto block_cache_size = GetOptionValue<uint32_t>(o, "cacheSize", 0);
  if (block_cache_size > 0) {
    options.block_cache = leveldb::NewLRUCache(block_cache_size);
  }

  const auto compression = GetOptionValue(o, "compression", true);
  if (!compression) {
    options.compression = leveldb::kNoCompression;
  }

  const auto bits_per_key = GetOptionValue<uint32_t>(o, "bitsPerKey", 0);
  if (bits_per_key > 0) {
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);
  }

  Poziomka* obj = new Poziomka(**str, options);

  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

void Poziomka::Open(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::HandleScope scope;

  auto fn = new Nan::Callback(info[0].As<v8::Function>());

  Poziomka* poziomka = ObjectWrap::Unwrap<Poziomka>(info.Holder());
  auto worker = new OpenWorker(fn, *poziomka);
  Nan::AsyncQueueWorker(worker);
}

void Poziomka::Close(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  auto fn = new Nan::Callback(info[0].As<v8::Function>());

  Poziomka* poziomka = ObjectWrap::Unwrap<Poziomka>(info.Holder());
  auto worker = new CloseWorker(fn, *poziomka);
  Nan::AsyncQueueWorker(worker);
}

void Poziomka::GetMany(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  auto keys = info[0].As<v8::Array>();
  auto fn = new Nan::Callback(info[1].As<v8::Function>());

  const auto len = keys->Length();

  std::vector<Slice> slices;
  slices.reserve(len);

  for(auto i = 0; i < len; i++) {
    slices.emplace_back(keys->Get(i));
  }

  Poziomka* poziomka = ObjectWrap::Unwrap<Poziomka>(info.Holder());
  auto worker = new GetWorker(fn, *(poziomka->db), std::move(slices));
  Nan::AsyncQueueWorker(worker);
}

void Poziomka::PutMany(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  auto keys = info[0].As<v8::Array>();
  auto values = info[1].As<v8::Array>();
  auto fn = new Nan::Callback(info[2].As<v8::Function>());

  auto batch = new leveldb::WriteBatch();

  const auto len = keys->Length();
  for (auto i = 0; i < len; i++) {
    batch->Put(Slice{ keys->Get(i) }, Slice{ values->Get(i) });
  }

  Poziomka* poziomka = ObjectWrap::Unwrap<Poziomka>(info.Holder());
  auto worker = new BatchWorker(fn, *(poziomka->db), batch);
  Nan::AsyncQueueWorker(worker);
}

void Poziomka::RemoveMany(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  auto keys = info[0].As<v8::Array>();
  auto fn = new Nan::Callback(info[1].As<v8::Function>());

  auto batch = new leveldb::WriteBatch();

  const auto len = keys->Length();
  for (auto i = 0; i < len; i++) {
    batch->Delete(Slice{ keys->Get(i) });
  }

  Poziomka* poziomka = ObjectWrap::Unwrap<Poziomka>(info.Holder());
  auto worker = new BatchWorker(fn, *(poziomka->db), batch);
  Nan::AsyncQueueWorker(worker);
}

void Poziomka::Init(v8::Local<v8::Object> exports) {
  // Prepare constructor template
  auto tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Poziomka").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "open", Open),
  Nan::SetPrototypeMethod(tpl, "close", Close),
  Nan::SetPrototypeMethod(tpl, "getMany", GetMany),
  Nan::SetPrototypeMethod(tpl, "putMany", PutMany),
  Nan::SetPrototypeMethod(tpl, "removeMany", RemoveMany),

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(
    exports,
    Nan::New("Poziomka").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked()
  );
}

void InitAll(v8::Local<v8::Object> exports) {
  Poziomka::Init(exports);
}

NODE_MODULE(addon, InitAll)
