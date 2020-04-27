cmake_minimum_required (VERSION 3.6)
project(pdf-source LANGUAGES C VERSION 0.0.1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${pdf-source_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR true)

find_package(PkgConfig REQUIRED)
find_package(OBS REQUIRED)
find_package(ImageMagick 6.9.10 EXACT REQUIRED COMPONENTS MagickCore)

pkg_check_modules(ImageMagick REQUIRED IMPORTED_TARGET ImageMagick)

include_directories(SYSTEM ${OBS_INCLUDE_DIR} ${ImageMagick_INCLUDE_DIRS})

if(MSVC)
    set(pdf-source_PLATFORM_DEPS
        w32-pthreads)
endif()

set(pdf-source_SOURCES
    graphics-magick.c
    pdf-file.c
    pdf-source.c)

if(WIN32)
    set(MODULE_DESCRIPTION "OBS PDF module")
    configure_file(${CMAKE_SOURCE_DIR}/cmake/winrc/obs-module.rc.in pdf-source.rc)
    list(APPEND pdf-source_SOURCES
        pdf-source.rc)
endif()

add_library(pdf-source MODULE
    ${pdf-source_SOURCES})
target_link_libraries(pdf-source PUBLIC PkgConfig::ImageMagick ${OBS_LIBRARIES} ${pdf-source_PLATFORM_DEPS})
