add_subdirectory(LTBL2/LTBL2)

set(ENTITYX_BUILD_TESTING false CACHE BOOL "Enable building of tests.")
set(ENTITYX_RUN_BENCHMARKS false CACHE BOOL "Run benchmarks (in conjunction with -DENTITYX_BUILD_TESTING=1).")
set(ENTITYX_MAX_COMPONENTS 64 CACHE STRING "Set the maximum number of components.")
set(ENTITYX_DT_TYPE double CACHE STRING "The type used for delta time in EntityX update methods.")
set(ENTITYX_BUILD_SHARED false CACHE BOOL "Build shared libraries?")
add_subdirectory(entityx)

set(BOX2D_INSTALL_DOC OFF)
set(BOX2D_BUILD_EXAMPLES OFF)
set(BOX2D_INSTALL OFF CACHE BOOL "Install Box2D libs, includes, and CMake scripts")
set(BOX2D_INSTALL_DOC OFF CACHE BOOL "Install Box2D documentation")
set(BOX2D_BUILD_SHARED OFF CACHE BOOL "Build Box2D shared libraries")
set(BOX2D_BUILD_EXAMPLES OFF CACHE BOOL "Build Box2D examples")
add_subdirectory(Box2D/Box2D)

