.PHONY: xcode mac_release qt clean

game:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Release ..
	cd build && cmake --build . --config Release

xcode:
	mkdir -p build
	cd build && cmake -GXcode ..
	open build/*.xcodeproj

xcode_release:
	mkdir -p build_release
	cd build_release && \
		cmake -GXcode -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
	open build_release/*.xcodeproj

qt:
	mkdir -p build
	cd build && cmake -DHAVE_QT=true -DCMAKE_BUILD_TYPE=Release ..
	cmake --build build --config Release

qt_xcode:
	mkdir -p build
	cd build && cmake -GXcode -DHAVE_QT=true ..
	open build/*.xcodeproj

clean:
	rm -rf build*
