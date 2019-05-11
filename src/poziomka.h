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
  Poziomka(std::string location): location(location) {
    options.create_if_missing = true;
    options.block_size =  32 * 1024 * 1024;
    options.write_buffer_size = 32 * 1024 * 1024;
    options.max_file_size = 8 * 1024 * 1024;
  }

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
