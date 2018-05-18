#pragma once

#include <memory>

#include <napi.h>

#include <leveldb/options.h>

class Poziomka;

namespace leveldb {
  class DB;
  class Slice;
  class WriteBatch;
}

class OpenWorker : public Napi::AsyncWorker
{
public:
  OpenWorker(const Napi::Function& fn, Poziomka& poziomka):
    Napi::AsyncWorker(fn),
    poziomka(poziomka)
    {}

  void Execute() override;

private:
  Poziomka& poziomka;
};


class CloseWorker : public Napi::AsyncWorker
{
public:
  CloseWorker(const Napi::Function& fn, Poziomka& poziomka):
    Napi::AsyncWorker(fn),
    poziomka(poziomka)
    {}

  void Execute() override;

private:
  Poziomka& poziomka;
};

class BatchWorker : public Napi::AsyncWorker
{
public:
  BatchWorker(const Napi::Function& fn, leveldb::DB& db, leveldb::WriteBatch* batch):
    Napi::AsyncWorker(fn),
    db(db),
    batch(batch)
    {}

  void Execute() override;

private:
  leveldb::DB& db;
  leveldb::WriteOptions options;
  std::unique_ptr<leveldb::WriteBatch> batch;
};

class GetWorker : public Napi::AsyncWorker
{
public:
  GetWorker(const Napi::Function& fn, leveldb::DB& db, std::vector<leveldb::Slice>&& slices):
    Napi::AsyncWorker(fn),
    db(db),
    keys(std::move(slices)),
    values(keys.size())
    {}

  void Execute() override;
  void OnOK() override;

private:
  leveldb::DB& db;
  leveldb::ReadOptions options;
  std::vector<leveldb::Slice> keys;
  std::vector<std::string> values;
  std::vector<unsigned int> missing;
};
