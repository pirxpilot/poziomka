[![NPM version][npm-image]][npm-url]
[![Build Status][travis-image]][travis-url]
[![Dependency Status][deps-image]][deps-url]
[![Dev Dependency Status][deps-dev-image]][deps-dev-url]

# poziomka

Fast and opinionated leveldb interface.
 - It does not support any encoding (works with buffers).
 - It allows for fast insertion and lookup through `putMany` and `getMany`

If you need encoding, iteration and other goodies check out [level].

## Install

```sh
$ npm install --save poziomka
```

## Usage

```js
var Poziomka = require('poziomka');

let db = new Poziomka('/path/to/my/db');

// no encoding/decoding - poziomka works with Buffers
let keys = [2, 3, 4].map(x => Buffer.from([x]));

db.open(function(err) {
  if (err) {
    // something went wrong
  }
  // database is not open
});

// keys and values have to be Arrays of Buffer's
db.putMany(keys, values, function(err) {
  // for each key in an
});


// keys and values have to be Arrays of Buffer's
db.getMany(keys, function(err, values, missing) {
  if (err) {
    // leveldb error
  }
  if (missing) {
    // some of the keys were not in DB
    // missing has indexes in the keys array, so to print the keys
    console.log('Missing keys:', missing.map(i => keys[i]));
  }
  // empty Buffer is returned for each missing key
});

db.removeMany(keys, function(err) {
  // keys are now deleted
});

```

## License

MIT © [Damian Krzeminski](https://pirxpilot.me)

[npm-image]: https://img.shields.io/npm/v/poziomka.svg
[npm-url]: https://npmjs.org/package/poziomka

[travis-url]: https://travis-ci.org/pirxpilot/poziomka
[travis-image]: https://img.shields.io/travis/pirxpilot/poziomka.svg

[deps-image]: https://img.shields.io/david/pirxpilot/poziomka.svg
[deps-url]: https://david-dm.org/pirxpilot/poziomka

[deps-dev-image]: https://img.shields.io/david/dev/pirxpilot/poziomka.svg
[deps-dev-url]: https://david-dm.org/pirxpilot/poziomka?type=dev

[level]: https://npmjs.org/package/level
