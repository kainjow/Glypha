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

.PHONY: mac_dist
mac_dist: clean game
	codesign --entitlements mac/Glypha.entitlements -fs "Developer ID Application" "build/Glypha III.app"
	cd build && zip -r "Glypha III.zip" "Glypha III.app"

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
