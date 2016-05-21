.PHONY: game qt

game:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Release ..
	cd build && cmake --build . --config Release

qt:
	mkdir -p build
	cd build && qmake ../qt
	cd build && make release

clean:
	rm -rf build
