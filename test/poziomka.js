var should = require('should');
const Poziomka = require('../');
const tmp = require('tmp');

describe('poziomka', function () {
  before(function () {
    this.location = tmp.dirSync({
      prefix: 'poziomka-'
    }).name;
  });

  it('open and close', function (done) {
    let db = new Poziomka(this.location);
    db.open(function(err) {
      should.not.exist(err);
      db.close(function(err) {
        should.not.exist(err);
        done();
      });
    });
  });

  describe('crud', function() {

    function from(array) {
      return array.map(x => Buffer.from([x]));
    }

    before(function(done) {
      let { name } = tmp.dirSync({
        prefix: 'poziomka-'
      });

      this.db = new Poziomka(name);
      this.db.open(done);
    });

    after(function(done) {
      this.db.close(done);
    });


    it('put and get', function (done) {
      let keys =  from([ 1, 13, 5, 88 ]);
      let values =  from([ 100, 101, 102, 103 ]);

      let db = this.db;

      db.putMany(keys, values, get);

      function get() {
        db.getMany(keys, check);
      }

      function check(err, result) {
        result.should.eql(values);
        done(err);
      }
    });


    it('put, remove, and get', function (done) {
      let keys =  from([ 1, 13, 5, 88 ]);
      let values =  from([ 100, 101, 102, 103 ]);
      let empty = Buffer.alloc(0);

      let db = this.db;

      db.putMany(keys, values, remove);

      function remove() {
        db.removeMany([ keys[1], keys[2] ], get);
      }

      function get() {
        db.getMany(keys, check);
      }

      function check(err, result, missing) {
        missing.should.eql([ 1 , 2 ]);
        result.should.have.length(4);
        result[0].should.eql(values[0]);
        result[1].should.eql(empty);
        result[2].should.eql(empty);
        result[3].should.eql(values[3]);
        done(err);
      }
    });

  });
});
