cmake_minimum_required(VERSION 3.3)
project(PhotosBrowser)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Network)
find_package(Qt5Declarative)

# -fPIC ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}
set(P_S main.cpp)
set(P_H main.h)

qt5_wrap_cpp(GENERATED_SOURCES ${P_H})

add_executable(PhotosBrowser ${P_S} ${P_H})
qt5_use_modules(PhotosBrowser Core Widgets Network)
target_compile_features(PhotosBrowser PRIVATE cxx_range_for)
target_link_libraries(PhotosBrowser ${Qt5Core_QTMAIN_LIBRARIES})