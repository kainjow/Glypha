cmake_minimum_required(VERSION 2.8)

if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.7")
endif()

project(Glypha)

set(SOURCES
  game/GLGame.cpp
  game/GLGame.h
  game/GLImage.cpp
  game/GLImage.h
  game/GLRect.cpp
  game/GLRect.h
  game/GLRenderer.cpp
  game/GLRenderer.h
  game/GLResources.cpp
  game/GLResources.h
  game/GLUtils.cpp
  game/GLUtils.h
  game/GLBufferReader.cpp
  game/GLBufferReader.h
  game/GLCursor.h
  game/GLSounds.h
)

include_directories(game)

if(BEOS)
  list(APPEND SOURCES
    haiku/main.cpp
    game/GLCursor.cpp
    game/GLImage_libpng.cpp
    game/GLSounds.cpp
  )
elseif(WIN32)
  list(APPEND SOURCES
    win/main.cpp
    win/resources.rc
    game/GLCursorWin32.cpp
    game/GLImage_Win32.cpp
    game/GLSoundsWin32.cpp
  )
  include_directories(win)
  add_definitions(-DUNICODE)
elseif(APPLE)
  list(APPEND SOURCES
    osx/main.mm
    game/GLCursorCocoa.mm
    game/GLImage_CoreImage.cpp
    game/GLSoundsCocoa.mm
  )
endif()

if(UNIX)
  add_definitions(
    -Wall
    -Wextra
    -Werror
  )
endif()

add_executable(Glypha WIN32 MACOSX_BUNDLE ${SOURCES})

set_target_properties(Glypha PROPERTIES OUTPUT_NAME "Glypha III")

list(FIND SOURCES game/GLImage_libpng.cpp HAVE_IMAGE_LIBPNG)
if(NOT HAVE_IMAGE_LIBPNG EQUAL -1)
  include(FindPNG)
  target_link_libraries(Glypha ${PNG_LIBRARIES})
  include_directories(${PNG_INCLUDE_DIRS})
endif()

include(FindOpenGL)
target_link_libraries(Glypha ${OPENGL_LIBRARIES})

if(BEOS)
  find_library(BE be)
  target_link_libraries(Glypha ${BE})
elseif(WIN32)
  target_link_libraries(Glypha shlwapi windowscodecs winmm)
elseif(APPLE)
  set_target_properties(Glypha PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/osx/Info.plist)
  target_link_libraries(Glypha
    "-framework Cocoa"
    "-framework CoreVideo"
    "-framework AVFoundation"
  )
endif()
