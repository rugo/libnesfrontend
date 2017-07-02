cmake_minimum_required(VERSION 3.5)
project(libnesfrontend)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
find_package(PkgConfig REQUIRED)
pkg_check_modules(ALLEGRO REQUIRED allegro-5)

include_directories(${ALLEGRO_INCLUDE_DIRS})
link_directories(${ALLEGRO_LIBRARY_DIRS} ./quicknes/)

set(SOURCE_FILES frontend_lib.c frontend_lib.h)
add_library(nesfrontend SHARED ${SOURCE_FILES})
target_link_libraries(nesfrontend ${ALLEGRO_LIBRARIES} retrones)

add_executable(main_test test.c)
target_link_libraries(main_test nesfrontend)
