const { promisify } = require('node:util');
const Poziomka = require('..');

class PoziomkaPromises extends Poziomka {
  getMany(keys) {
    return new Promise((resolve, reject) =>
      super.getMany(keys, (err, result, missing) => {
        if (err) reject(err);
        else resolve([result, missing]);
      })
    );
  }
}

module.exports = PoziomkaPromises;

PoziomkaPromises.prototype.open = promisify(Poziomka.prototype.open);
PoziomkaPromises.prototype.close = promisify(Poziomka.prototype.close);
PoziomkaPromises.prototype.putMany = promisify(Poziomka.prototype.putMany);
PoziomkaPromises.prototype.removeMany = promisify(Poziomka.prototype.removeMany);
