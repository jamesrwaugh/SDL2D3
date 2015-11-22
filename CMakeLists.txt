#SDL2D3 Short CMake File
cmake_minimum_required(VERSION 2.8)
project(SDL2D3)

set( SRCS
    src/main.cpp
    src/keyvalues.cpp
    src/ltbl/Math.cpp
    src/ltbl/lighting/LightPointEmission.cpp
    src/ltbl/lighting/LightSystem.cpp
    src/ltbl/lighting/LightShape.cpp
    src/ltbl/lighting/LightDirectionEmission.cpp
    src/ltbl/quadtree/StaticQuadtree.cpp
    src/ltbl/quadtree/QuadtreeNode.cpp
    src/ltbl/quadtree/Quadtree.cpp
    src/ltbl/quadtree/DynamicQuadtree.cpp
    src/ltbl/quadtree/QuadtreeOccupant.cpp
    src/entityx/systems/Box2DSystem.cpp
    src/entityx/systems/SFGUISystem.cpp
    src/entityx/systems/TextureSystem.cpp
    src/entityx/systems/LTBLSystem.cpp
    src/box2d/SFMLDebugDraw.cpp
    src/utility.cpp
)


# Output folders of the program
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/obj)

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}/src")

# Compiler flags
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "(GNU|.*Clang)")
    set(CMAKE_CXX_FLAGS "-Wall -std=c++14 -O2 -s")
endif()

#Executable
add_executable(SDL2D3 ${SRCS})
target_link_libraries(SDL2D3 sfml-system sfml-window sfml-graphics sfgui Box2D GL entityx)

