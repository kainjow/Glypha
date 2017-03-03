.PHONY: xcode mac_release qt clean

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
		cmake --build . --config RelWithDebInfo && \
		cpack -C RelWithDebInfo

mac_release_code_sign:
	mkdir -p build_release
	cd build_release && \
		cmake -GXcode -DCMAKE_BUILD_TYPE=RelWithDebInfo -DGLYPHA_MAC_CODE_SIGN_IDENTITY="Developer ID Application: Kevin Wojniak" .. && \
		cmake --build . --config RelWithDebInfo && \
		cpack -C RelWithDebInfo

qt:
	mkdir -p build
	cd build && cmake -DHAVE_QT=true -DCMAKE_BUILD_TYPE=Release ..
	cmake --build build --config Release

clean:
	rm -rf build*
