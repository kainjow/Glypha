cmake_minimum_required(VERSION 2.8)

set(EXE_NAME "Glypha III")
set(EXE_IDENTIFIER "com.kainjow.GlyphaIII")

if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.7")
endif()

project(Glypha)

set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

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

set_target_properties(Glypha PROPERTIES OUTPUT_NAME ${EXE_NAME})

list(FIND SOURCES game/GLImage_libpng.cpp HAVE_IMAGE_LIBPNG)
if(NOT HAVE_IMAGE_LIBPNG EQUAL -1)
  include(FindPNG)
  target_link_libraries(Glypha ${PNG_LIBRARIES})
  include_directories(${PNG_INCLUDE_DIRS})
endif()

include(FindOpenGL)
if(NOT OPENGL_FOUND)
  message(FATAL_ERROR "OpenGL not found!")
endif()
target_link_libraries(Glypha ${OPENGL_LIBRARIES})

if(BEOS)
  find_library(BE be)
  target_link_libraries(Glypha ${BE})
elseif(WIN32)
  target_link_libraries(Glypha shlwapi windowscodecs winmm)
elseif(APPLE)
  set_target_properties(Glypha PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/osx/Info.plist
    MACOSX_BUNDLE_BUNDLE_NAME ${EXE_NAME}
    MACOSX_BUNDLE_GUI_IDENTIFIER ${EXE_IDENTIFIER}
  )
  if("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    set_target_properties(Glypha PROPERTIES
      XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Developer ID Application: Kevin Wojniak"
    )
  endif()
  target_link_libraries(Glypha
    "-framework Cocoa"
    "-framework CoreVideo"
    "-framework AVFoundation"
  )
endif()

install(TARGETS Glypha DESTINATION .)
if(APPLE)
  set(CPACK_GENERATOR "DragNDrop")
endif()
include(CPack)
