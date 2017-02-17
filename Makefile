.PHONY: xcode mac_release qt clean

xcode:
	mkdir -p build
	cd build && cmake -GXcode ..
	open build/*.xcodeproj

mac_release:
	mkdir -p build_release
	cd build_release && \
		cmake -GXcode -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && \
		cmake --build . --config RelWithDebInfo && \
		cpack -C RelWithDebInfo

qt:
	mkdir -p build
	cd build && cmake -DHAVE_QT=true -DCMAKE_BUILD_TYPE=Release ..
	cmake --build build --config Release

clean:
	rm -rf build*
