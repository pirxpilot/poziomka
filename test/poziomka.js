const test = require('tape');
const { dirSync } = require('tmp');

const Poziomka = require('../');

test('open and close', function (t) {
  const { name } = dirSync({ prefix: 'poziomka-' });
  const db = new Poziomka(name);
  db.open(function(err) {
    t.error(err, 'opens without error');
    db.close(function(err) {
      t.error(err, 'closes without error');
      t.end();
    });
  });
});

let db;

test('open', function(t) {
  const { name } = dirSync({ prefix: 'poziomka-' });

  db = new Poziomka(name);
  db.open(t.end);
});

test('put and get', function (t) {
  let keys =  from([ 1, 13, 5, 88 ]);
  let values =  from([ 100, 101, 102, 103 ]);

  db.putMany(keys, values, get);

  function get() {
    db.getMany(keys, check);
  }

  function check(err, result) {
    t.error(err);
    t.same(result, values);
    t.end();
  }
});

test('put, remove, and get', function (t) {
  let keys =  from([ 1, 13, 5, 88 ]);
  let values =  from([ 100, 101, 102, 103 ]);
  let empty = Buffer.alloc(0);

  db.putMany(keys, values, remove);

  function remove() {
    db.removeMany([ keys[1], keys[2] ], get);
  }

  function get() {
    db.getMany(keys, check);
  }

  function check(err, result, missing) {
    t.error(err);
    t.same(missing, [ 1 , 2 ]);
    t.equal(result.length, 4);
    t.same(result[0], values[0]);
    t.same(result[1], empty);
    t.same(result[2], empty);
    t.same(result[3], values[3]);
    t.end();
  }
});

test('close', function(t) {
  db.close(t.end);
  db = undefined;
});

function from(array) {
  return array.map(x => Buffer.from([x]));
}
