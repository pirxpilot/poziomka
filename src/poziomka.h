#pragma once

#include <memory>
#include <string>

#include <napi.h>

#include <leveldb/db.h>

class Poziomka : public Napi::ObjectWrap<Poziomka>
{
public:
  Poziomka(const Napi::CallbackInfo&);
  Napi::Value Open(const Napi::CallbackInfo&);
  Napi::Value Close(const Napi::CallbackInfo&);
  Napi::Value GetMany(const Napi::CallbackInfo&);
  Napi::Value PutMany(const Napi::CallbackInfo&);
  Napi::Value RemoveMany(const Napi::CallbackInfo&);

  static Napi::Function GetClass(Napi::Env);

  friend class OpenWorker;
  friend class CloseWorker;

private:
  leveldb::Status OpenDatabase() {
    leveldb::DB* pdb;
    auto status = leveldb::DB::Open(options, location, &pdb);
    db.reset(pdb);
    return status;
  }

  void CloseDatabase() {
    db.reset();
  }

  std::string location;
  leveldb::Options options;
  std::unique_ptr<leveldb::DB> db;
};
