#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "workers.h"

#include "poziomka.h"
#include "slice.h"

void OpenWorker::Execute() {
  auto status = poziomka.OpenDatabase();
  if (!status.ok()) SetError(status.ToString());
}

void CloseWorker::Execute() {
  poziomka.CloseDatabase();
}

void BatchWorker::Execute() {
  auto status = db.Write(options, batch.get());
  if (!status.ok()) SetError(status.ToString());
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
    SetError(status.ToString());
    break;
  }
}

void GetWorker::OnOK() {
  auto env = Env();

  auto results = Napi::Array::New(env, values.size());
  for (int i = 0; i < values.size(); i++) {
    auto value = values[i];
    auto buffer = Napi::Buffer<char>::Copy(env, value.data(), value.size());
    results.Set(i, buffer);
  }

  if (missing.size() == 0) {
    Callback().MakeCallback(Receiver().Value(), { env.Null(), results });
  } else {

    auto orphans = Napi::Array::New(env, missing.size());
    for (int i = 0; i < missing.size(); i++) {
      orphans.Set(i, Napi::Number::New(env, missing[i]));
    }

    Callback().MakeCallback(Receiver().Value(), { env.Null(), results, orphans });
  }
}
