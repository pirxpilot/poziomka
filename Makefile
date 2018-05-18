PROJECT=poziomka

CC_SRC=$(wildcard src/*.h src/*.cc)

check: lint test

lint:
	./node_modules/.bin/jshint *.js test

test: addon
	./node_modules/.bin/mocha --recursive --require should

build/Release/leveldown.node: $(CC_SRC) CMakeLists.txt
	./node_modules/.bin/cmake-js build

addon: build/Release/leveldown.node

clean:
	rm -rf build

.PHONY: check lint test addon clean
