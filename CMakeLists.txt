cmake_minimum_required(VERSION 4.4.3)
project(SnakeGame)

find_package(glfw3 3.5.1 REQUIRED)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable("${PROJECT_NAME}" "${MY_SOURCES}" external/glad/src/glad.c)

target_include_directories("${PROJECT_NAME}" PUBLIC external include external/glad/include)
target_link_libraries("${PROJECT_NAME}" PRIVATE glfw)
