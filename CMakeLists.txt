#
# Copyright (c) 2017-2019 by Ilya Barykin
# Released under the MIT License.
# See the provided LICENSE.TXT file for details.
#

cmake_minimum_required(VERSION 3.3)
project(PhotosBrowser CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/vendor")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(P_S
        res/res.rc

        main.cpp
        Resizer.cpp
        ImageLoader.cpp
        GifLoader.cpp
        ImageProvider.cpp
        Pio.cpp Window.cpp
        )

set(P_H
        Resizer.h
        ImageLoader.h
        ImageProvider.h
        Pio.h Window.h
        )

include(cotire/CMake/cotire)

qt5_wrap_cpp(GENERATED_SOURCES ${P_H})
qt5_add_resources(RES res/res.qrc)

add_executable(PhotosBrowser ${P_S} ${P_H} ${RES})
target_compile_features(PhotosBrowser PRIVATE cxx_range_for)
set_target_properties(PhotosBrowser PROPERTIES COTIRE_ENABLE_PRECOMPILED_HEADER OFF)
target_link_libraries(PhotosBrowser Qt5::Core Qt5::Widgets)

cotire(PhotosBrowser)
