cmake_minimum_required(VERSION 2.8)

project(Glypha)

set(SOURCES
    game/GLGame.cpp
    game/GLImage.cpp
    game/GLRect.cpp
    game/GLRenderer.cpp
    game/GLResources.cpp
    game/GLSounds.cpp
    game/GLUtils.cpp
)

include_directories(game)

if (BEOS)
	list(APPEND SOURCES
		haiku/main.cpp
		game/GLBufferReader.cpp
		game/GLCursor.cpp
		game/GLImage_libpng.cpp
	)
endif()

add_executable(Glypha ${SOURCES})

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
endif()
