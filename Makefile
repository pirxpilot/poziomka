PROJECT=poziomka
ADDON=build/Release/$(PROJECT).node

CC_SRC=$(wildcard src/*.h src/*.cc)

check: lint test

lint:
	./node_modules/.bin/jshint *.js test

test: addon
	./node_modules/.bin/mocha --recursive --require should

$(ADDON): $(CC_SRC) CMakeLists.txt
	./node_modules/.bin/cmake-js build

addon: $(ADDON)

clean:
	rm -rf build

.PHONY: check lint test addon clean
