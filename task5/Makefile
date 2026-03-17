all: build

build:
	cmake -S . -B build
	cmake --build build

run: build
	cd build && ./myftxui
