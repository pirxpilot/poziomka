#pragma once

#include <memory>
#include <string>

#include <nan.h>
#include <leveldb/db.h>

class Poziomka : public Nan::ObjectWrap
{
public:
  static void Init(v8::Local<v8::Object>);

  friend class OpenWorker;
  friend class CloseWorker;

private:
  Poziomka(std::string location, leveldb::Options options):
    location(location),
    options(options) {};

  static void New(const Nan::FunctionCallbackInfo<v8::Value>&);
  static void Open(const Nan::FunctionCallbackInfo<v8::Value>&);
  static void Close(const Nan::FunctionCallbackInfo<v8::Value>&);
  static void GetMany(const Nan::FunctionCallbackInfo<v8::Value>&);
  static void PutMany(const Nan::FunctionCallbackInfo<v8::Value>&);
  static void RemoveMany(const Nan::FunctionCallbackInfo<v8::Value>&);

  static Nan::Persistent<v8::Function> constructor;

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
