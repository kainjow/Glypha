.PHONY: game
game:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Release ..
	cd build && cmake --build . --config Release

.PHONY: xcode
xcode:
	mkdir -p build
	cd build && cmake -GXcode ..
	open build/*.xcodeproj

.PHONY: xcode_release
xcode_release:
	mkdir -p build_release
	cd build_release && \
		cmake -GXcode -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
	open build_release/*.xcodeproj

.PHONY: qt
qt:
	mkdir -p build
	cd build && cmake -DHAVE_QT=true -DCMAKE_BUILD_TYPE=Release ..
	cmake --build build --config Release

.PHONY: qt_xcode
qt_xcode:
	mkdir -p build
	cd build && cmake -GXcode -DHAVE_QT=true ..
	open build/*.xcodeproj

.PHONY: clean
clean:
	rm -rf build*
