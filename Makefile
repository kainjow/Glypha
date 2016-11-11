.PHONY: game qt

game:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Release ..
	cd build && cmake --build . --config Release

xcode:
	mkdir -p build
	cd build && cmake -GXcode ..
	open build/*.xcodeproj

mac_release:
	mkdir -p build_release
	cd build_release && \
		cmake -GXcode -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && \
		cmake --build . --config RelWithDebInfo

qt:
	mkdir -p build
	cd build && qmake ../qt
	cd build && make release

clean:
	rm -rf build
