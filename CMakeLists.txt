cmake_minimum_required(VERSION 3.25)
project(C_C___Learning_Application C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

# Keep the raylib build small: no bundled examples
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 6.2
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

add_executable(C_C___Learning_Application main.c)

target_link_libraries(C_C___Learning_Application PRIVATE raylib)