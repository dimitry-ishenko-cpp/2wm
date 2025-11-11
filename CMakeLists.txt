cmake_minimum_required(VERSION 3.16) # focal
project(2wm VERSION 0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_search_module(x11 REQUIRED IMPORTED_TARGET x11)
pkg_search_module(xrandr REQUIRED IMPORTED_TARGET xrandr)

include(GNUInstallDirs)

add_executable(2wm 2wm.cpp)
target_link_libraries(2wm PRIVATE PkgConfig::x11 PkgConfig::xrandr)
install(TARGETS 2wm DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})

configure_file(2wm.sh.in 2wm.sh)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/2wm.sh DESTINATION ${CMAKE_INSTALL_BINDIR} RENAME 2wm)
