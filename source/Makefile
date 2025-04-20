
.PHONY: run release

run: build/bin/Release/ddraw.dll
	mv ./build/bin/Release/ddraw.dll "C:\GOG Games\Settlers 4 Gold\Exe"

build/bin/Release/ddraw.dll: build
	cmake --build build --config Release

build: CMakeLists.txt
	mkdir build
	cmake -S . -B build

release:
	cmake --build build --config Release
