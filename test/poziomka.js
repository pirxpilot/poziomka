const test = require('node:test');
const { dirSync } = require('tmp');

const Poziomka = require('..');

test('open and close', (t, done) => {
  const { name } = dirSync({ prefix: 'poziomka-' });
  const db = new Poziomka(name);
  db.open(err => {
    t.assert.ifError(err, 'opens without error');
    db.close(err => {
      t.assert.ifError(err, 'closes without error');
      done();
    });
  });
});

test('crud', async t => {
  let db;

  t.before((_, done) => {
    const { name } = dirSync({ prefix: 'poziomka-' });

    db = new Poziomka(name);
    db.open(done);
  });

  t.after((_, done) => {
    db.close(done);
    db = undefined;
  });

  await t.test('put and get', (t, done) => {
    const keys = from([1, 13, 5, 88]);
    const values = from([100, 101, 102, 103]);

    db.putMany(keys, values, get);

    function get() {
      db.getMany(keys, check);
    }

    function check(err, result) {
      t.assert.ifError(err);
      t.assert.deepEqual(result, values);
      done();
    }
  });

  await t.test('put, remove, and get', (t, done) => {
    const keys = from([1, 13, 5, 88]);
    const values = from([100, 101, 102, 103]);
    const empty = Buffer.alloc(0);

    db.putMany(keys, values, remove);

    function remove() {
      db.removeMany([keys[1], keys[2]], get);
    }

    function get() {
      db.getMany(keys, check);
    }

    function check(err, result, missing) {
      t.assert.ifError(err);
      t.assert.deepEqual(missing, [1, 2]);
      t.assert.equal(result.length, 4);
      t.assert.deepEqual(result[0], values[0]);
      t.assert.deepEqual(result[1], empty);
      t.assert.deepEqual(result[2], empty);
      t.assert.deepEqual(result[3], values[3]);
      done();
    }
  });
});

function from(array) {
  return array.map(x => Buffer.from([x]));
}
