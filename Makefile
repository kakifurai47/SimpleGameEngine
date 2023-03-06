.PHONY: all build run clean


all: build


build:
	sh gen_makefile.sh
	make -C build/SimpleGameEngine-makefile

run:
	make build
	./build/SimpleGameEngine-makefile/src/core/test/sge_core_test

clean:
	rm -rf build
