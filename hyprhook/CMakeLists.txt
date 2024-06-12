cmake_minimum_required(VERSION 3.27)

project(hyprhook
    DESCRIPTION "Execute binarys on hyprland events"
    VERSION 0.1
)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SRC "*.cpp")

add_library(hyprhook SHARED ${SRC})

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET
hyprland
    libdrm
    libinput
    libudev
    pangocairo
    pixman-1
    wayland-server
    xkbcommon
    nlohmann_json
)
target_link_libraries(hyprhook PRIVATE rt PkgConfig::deps)

install(TARGETS hyprhook)
