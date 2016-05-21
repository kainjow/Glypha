.PHONY: game qt

game:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Release ..
	cd build && cmake --build . --config Release

xcode:
	mkdir -p build
	cd build && cmake -GXcode ..
	open build/*.xcodeproj

qt:
	mkdir -p build
	cd build && qmake ../qt
	cd build && make release

clean:
	rm -rf build
