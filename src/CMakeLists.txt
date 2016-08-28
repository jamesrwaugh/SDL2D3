set (CMAKE_CXX_STANDARD 14)

#For FindSFGUI.cmake and FindSFML.cmake
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/src//cmake)

#Smaller External libraries as submodules (Box2D, LTBL, EntityX)
add_subdirectory(extlibs)

#SFML / SFGUI
find_package(SFML 2 COMPONENTS system window graphics REQUIRED)
find_package(SFGUI REQUIRED )
include_directories(${SFML_INCLUDE_DIR} ${SFGUI_INCLUDE_DIR})

#SDL2D3 Executable
file(GLOB_RECURSE SDL2D3_SOURCES utility/*.cpp sdl2d3/*.cpp)
add_executable(${PROJECT_NAME} main.cpp ${SDL2D3_SOURCES})

#Include directores for extlibs
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR} #Allows #include "utiity/..." and etc includes
	${CMAKE_CURRENT_SOURCE_DIR}/extlibs 
	${CMAKE_CURRENT_SOURCE_DIR}/extlibs/LTBL2/LTBL2/source
	${CMAKE_CURRENT_SOURCE_DIR}/extlibs/Box2D/Box2D
	${CMAKE_CURRENT_SOURCE_DIR}/extlibs/entityx
)
set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)
target_link_libraries(${PROJECT_NAME} 
	${SFML_LIBRARY} 
	${SFGUI_LIBRARY}
	Box2D 
	GL 
	entityx 
	LTBL2
)

