#pragma once

#include <memory>
#include <string>

#include <napi.h>
#include <leveldb/db.h>

class Poziomka : public Napi::ObjectWrap<Poziomka>
{
public:
  Poziomka(const Napi::CallbackInfo&);
  void Open(const Napi::CallbackInfo&);
  void Close(const Napi::CallbackInfo&);
  void GetMany(const Napi::CallbackInfo&);
  void PutMany(const Napi::CallbackInfo&);
  void RemoveMany(const Napi::CallbackInfo&);

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
