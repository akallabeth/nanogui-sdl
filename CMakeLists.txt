cmake_minimum_required (VERSION 3.18)

project("sdlgui")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# Required libraries for linking against nngui (all targets)
set(NNGUI_EXTRA_LIBS "")

# Platform-dependent files for nngui
set(NNGUI_EXTRA_SOURCE "")

include(CheckCXXCompilerFlag)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Quench annoying deprecation warnings when compiling GLFW on OSX
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
endif()

if (MSVC)
  # Disable annoying MSVC warnings (all targets)
  add_definitions(/D "_CRT_SECURE_NO_WARNINGS")

  # Parallel build on MSVC (all targets)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
  endif()
endif()

# Compile with compiler warnings turned on
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4100 /wd4101 /wd4018 /wd4201 /wd4189 /wd4457 /wd4456 /wd4245 /wd4838 /wd4389 /wd4702")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" HAS_CPP11_FLAG)

  if (HAS_CPP14_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  elseif (HAS_CPP11_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    message(FATAL_ERROR "Unsupported compiler -- requires C++11 support!")
  endif()
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_BINARY_DIR})

set(NNGUI_BASIC_SOURCE "")
list(APPEND NNGUI_BASIC_SOURCE
     sdlgui/button.h
     sdlgui/checkbox.h
     sdlgui/colorpicker.h
     sdlgui/colorwheel.h
     sdlgui/combobox.h
     sdlgui/common.h
     sdlgui/graph.h
     sdlgui/imagepanel.h
     sdlgui/imageview.h
     sdlgui/label.h
     sdlgui/layout.h
     sdlgui/messagedialog.h
     sdlgui/popup.h
     sdlgui/popupbutton.h
     sdlgui/progressbar.h
     sdlgui/screen.h
     sdlgui/slider.h
     sdlgui/stackedwidget.h
     sdlgui/tabheader.h
     sdlgui/tabwidget.h
     sdlgui/textbox.h
     sdlgui/theme.h
     sdlgui/vscrollpanel.h
     sdlgui/switchbox.h
     sdlgui/dropdownbox.h
     sdlgui/widget.h
     sdlgui/window.h
     sdlgui/nanovg.h
     sdlgui/nanovg_rt.h

     sdlgui/button.cpp
     sdlgui/checkbox.cpp
     sdlgui/colorpicker.cpp
     sdlgui/colorwheel.cpp
     sdlgui/combobox.cpp
     sdlgui/common.cpp
     sdlgui/graph.cpp
     sdlgui/imagepanel.cpp
     sdlgui/imageview.cpp
     sdlgui/label.cpp
     sdlgui/layout.cpp
     sdlgui/loadimages.cpp
     sdlgui/messagedialog.cpp
     sdlgui/resources.cpp
     sdlgui/popup.cpp
     sdlgui/popupbutton.cpp
     sdlgui/progressbar.cpp
     sdlgui/screen.cpp
     sdlgui/slider.cpp
     sdlgui/stackedwidget.cpp
     sdlgui/tabheader.cpp
     sdlgui/tabwidget.cpp
     sdlgui/textbox.cpp
     sdlgui/switchbox.cpp
     sdlgui/dropdownbox.cpp
     sdlgui/theme.cpp
     sdlgui/vscrollpanel.cpp
     sdlgui/widget.cpp
     sdlgui/window.cpp
     sdlgui/nanovg.c
)

add_library(${PROJECT_NAME}
     ${NNGUI_EXTRA_SOURCE} ${NNGUI_BASIC_SOURCE}
)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3-shared SDL3_image::SDL3_image-shared SDL3_ttf::SDL3_ttf-shared)
install(TARGETS ${PROJECT_NAME})

option(NANOGUI_BUILD_EXAMPLE "Build example application" ON)
if (NANOGUI_BUILD_EXAMPLE)
  # Build example application if desired
  add_executable(example1 example1.cpp )
  set_target_properties(
      example1 PROPERTIES
      VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build/debug")
		target_link_libraries(example1 ${PROJECT_NAME})

  # Copy icons for example application
  if (WIN32)
    file(COPY resources/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/debug)
  else()
    file(COPY resources/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  endif()
endif()
