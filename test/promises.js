const test = require('node:test');
const { dirSync } = require('tmp');

const Poziomka = require('../lib/promises');

test('open and close - promises', async () => {
  const { name } = dirSync({ prefix: 'poziomka-' });
  const db = new Poziomka(name);
  await db.open();
  await db.close();
});

test('crud - promises', async t => {
  let db;

  t.before(async () => {
    const { name } = dirSync({ prefix: 'poziomka-' });

    db = new Poziomka(name);
    await db.open();
  });

  t.after(async () => {
    await db.close();
    db = undefined;
  });

  await t.test('put and get', async t => {
    const keys = from([1, 13, 5, 88]);
    const values = from([100, 101, 102, 103]);

    await db.putMany(keys, values);
    const [result] = await db.getMany(keys);
    t.assert.deepEqual(result, values);
  });

  await t.test('put, remove, and get', async t => {
    const keys = from([1, 13, 5, 88]);
    const values = from([100, 101, 102, 103]);
    const empty = Buffer.alloc(0);

    await db.putMany(keys, values);
    await db.removeMany([keys[1], keys[2]]);

    const [result, missing] = await db.getMany(keys);

    t.assert.deepEqual(missing, [1, 2]);
    t.assert.equal(result.length, 4);
    t.assert.deepEqual(result[0], values[0]);
    t.assert.deepEqual(result[1], empty);
    t.assert.deepEqual(result[2], empty);
    t.assert.deepEqual(result[3], values[3]);
  });
});

function from(array) {
  return array.map(x => Buffer.from([x]));
}
