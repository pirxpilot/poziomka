#pragma once

#include <memory>

#include <nan.h>
#include <leveldb/options.h>

class Poziomka;
class Slice;

namespace leveldb {
  class DB;
  class Slice;
  class WriteBatch;
}


class OpenWorker : public Nan::AsyncWorker
{
public:
  OpenWorker(Nan::Callback* fn, Poziomka& poziomka):
    Nan::AsyncWorker(fn, "open"),
    poziomka(poziomka)
    {}

  void Execute() override;

private:
  Poziomka& poziomka;
};


class CloseWorker : public Nan::AsyncWorker
{
public:
  CloseWorker(Nan::Callback* fn, Poziomka& poziomka):
    Nan::AsyncWorker(fn, "close"),
    poziomka(poziomka)
    {}

  void Execute() override;

private:
  Poziomka& poziomka;
};

class BatchWorker : public Nan::AsyncWorker
{
public:
  BatchWorker(Nan::Callback* fn, leveldb::DB& db, leveldb::WriteBatch* batch):
    Nan::AsyncWorker(fn, "batch"),
    db(db),
    batch(batch)
    {}

  void Execute() override;

private:
  leveldb::DB& db;
  leveldb::WriteOptions options;
  std::unique_ptr<leveldb::WriteBatch> batch;
};

class GetWorker : public Nan::AsyncWorker
{
public:
  GetWorker(Nan::Callback* fn, leveldb::DB& db, std::vector<Slice>&& slices):
    Nan::AsyncWorker(fn, "get"),
    db(db),
    keys(std::move(slices)),
    values(keys.size())
    {}

  void Execute() override;
  void HandleOKCallback() override;

private:
  leveldb::DB& db;
  leveldb::ReadOptions options;
  std::vector<Slice> keys;
  std::vector<std::string> values;
  std::vector<unsigned int> missing;
};
