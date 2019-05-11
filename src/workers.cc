#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "workers.h"

#include "poziomka.h"
#include "slice.h"

void OpenWorker::Execute() {
  auto status = poziomka.OpenDatabase();
  if (!status.ok()) SetErrorMessage(status.ToString().c_str());
}

void CloseWorker::Execute() {
  poziomka.CloseDatabase();
}

void BatchWorker::Execute() {
  auto status = db.Write(options, batch.get());
  if (!status.ok()) SetErrorMessage(status.ToString().c_str());
}

void GetWorker::Execute() {
  for (size_t i = 0; i < keys.size(); i++) {
    auto status = db.Get(options, keys[i], &values[i]);
    if (status.ok()) continue;
    if (status.IsNotFound()) {
      missing.push_back(i);
      continue;
    }
    // not OK...
    SetErrorMessage(status.ToString().c_str());
    break;
  }
}

void GetWorker::HandleOKCallback() {
  Nan::HandleScope scope;

  auto results = Nan::New<v8::Array>(values.size());
  for (int i = 0; i < values.size(); i++) {
    auto value = values[i];
    // FIXME: or is it Nan::NewBuffer ???
    auto buffer = Nan::CopyBuffer(value.data(), value.size());
    Nan::Set(results, i, buffer.ToLocalChecked());
  }

  if (missing.size() == 0) {
    v8::Local<v8::Value> argv[] = { Nan::Null(), results };
    callback->Call(2, argv, async_resource);
  } else {

    auto orphans = Nan::New<v8::Array>(missing.size());
    for (int i = 0; i < missing.size(); i++) {
      Nan::Set(orphans, i, Nan::New(missing[i]));
    }

    v8::Local<v8::Value> argv[] = { Nan::Null(), results, orphans };
    callback->Call(3, argv, async_resource);
  }
}
