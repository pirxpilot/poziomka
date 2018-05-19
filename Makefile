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

prebuild:
	prebuild --backend cmake-js -t 8.11.2 -t 10.0.0 --strip

clean:
	rm -rf build

.PHONY: check lint test addon clean prebuild
