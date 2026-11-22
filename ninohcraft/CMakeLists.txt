cmake_minimum_required(VERSION 3.20)
project(NinohCraft3D LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

# Use Git checkout instead of URL archive: Android's ExternalProject
# was treating the URL archive entry as an invalid path list.
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

if(ANDROID)
    add_library(ninohcraft SHARED src/main.cpp)
    target_link_libraries(ninohcraft PRIVATE raylib android log EGL GLESv2 OpenSLES dl)
else()
    add_executable(ninohcraft src/main.cpp)
    target_link_libraries(ninohcraft PRIVATE raylib)
endif()
