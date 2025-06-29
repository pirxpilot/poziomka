PROJECT=poziomka
ADDON=build/Release/$(PROJECT).node

CC_SRC=$(wildcard src/*.h src/*.cc)

check: lint test

lint:
	./node_modules/.bin/biome ci

format:
	./node_modules/.bin/biome check --fix

test: addon
	node --test

$(ADDON): $(CC_SRC) CMakeLists.txt
	./node_modules/.bin/cmake-js build --CDCMAKE_POLICY_VERSION_MINIMUM=3.5

addon: $(ADDON)

prebuild:
	prebuild --backend cmake-js --runtime node --strip -- --CDCMAKE_POLICY_VERSION_MINIMUM=3.5

clean:
	rm -rf build

.PHONY: check format lint test addon clean prebuild
