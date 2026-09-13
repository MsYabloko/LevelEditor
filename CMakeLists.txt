cmake_minimum_required(VERSION 3.26)
project(LevelEditor)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
        "src/*.h"
        "src/*.cpp"
        "src/*.c"
)

file(GLOB IMGUI CONFIGURE_DEPENDS
        "imgui/*.cpp"
)

include_directories(include)
include_directories(imgui)

add_executable(LevelEditor ${SRC} ${IMGUI}
        src/ViewPort.cpp
        src/ViewPort.h)

add_custom_target(
        copy_data ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data"
        "${CMAKE_CURRENT_BINARY_DIR}/data"
)

add_dependencies(LevelEditor copy_data)

target_link_libraries(LevelEditor glfw)
target_link_libraries(LevelEditor glm::glm)