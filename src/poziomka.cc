#include <leveldb/write_batch.h>

#include "slice.h"
#include "workers.h"

#include "poziomka.h"

Nan::Persistent<v8::Function> Poziomka::constructor;

void Poziomka::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }

  if (!info.IsConstructCall()) {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    auto cons = Nan::New<v8::Function>(constructor);
    v8::Local<v8::Value> argv[] = { info[0] };
    info.GetReturnValue().Set(Nan::NewInstance(cons, 1, argv).ToLocalChecked());
    return;
  }

  auto str = new Nan::Utf8String(info[0]);
  Poziomka* obj = new Poziomka(**str);

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
