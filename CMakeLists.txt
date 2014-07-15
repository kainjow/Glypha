cmake_minimum_required(VERSION 2.8)

project(Glypha)

set(SOURCES
    game/GLGame.cpp
    game/GLImage.cpp
    game/GLRect.cpp
    game/GLRenderer.cpp
    game/GLResources.cpp
    game/GLUtils.cpp
)

include_directories(game)

if (BEOS)
	list(APPEND SOURCES
		haiku/main.cpp
		game/GLBufferReader.cpp
		game/GLCursor.cpp
		game/GLImage_libpng.cpp
		game/GLSounds.cpp
	)
elseif (WIN32)
	list(APPEND SOURCES
		win/main.cpp
		win/resources.rc
		game/GLBufferReader.cpp
		game/GLCursorWin32.cpp
		game/GLImage_Win32.cpp
		game/GLSoundsWin32.cpp
	)
	include_directories(win)
	add_definitions(-DUNICODE)
endif()

add_executable(Glypha WIN32 ${SOURCES})

set_target_properties(Glypha PROPERTIES OUTPUT_NAME "Glypha III")

list(FIND SOURCES game/GLImage_libpng.cpp HAVE_IMAGE_LIBPNG)
if (NOT HAVE_IMAGE_LIBPNG EQUAL -1)
	include(FindPNG)
	target_link_libraries(Glypha ${PNG_LIBRARIES})
	include_directories(${PNG_INCLUDE_DIRS})
endif()

include(FindOpenGL)
target_link_libraries(Glypha ${OPENGL_LIBRARIES})

if (BEOS)
    find_library(BE be)
    target_link_libraries(Glypha ${BE})
elseif (WIN32)
	target_link_libraries(Glypha shlwapi windowscodecs winmm)
endif()
