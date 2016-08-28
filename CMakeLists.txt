#SDL2D3 CMake File
cmake_minimum_required(VERSION 2.8)
project(SDL2D3)

if(CMAKE_VERSION VERSION_LESS "3.1")
	if(CMAKE_COMPILER_IS_GNUCXX)
		set(CMAKE_CXX_FLAGS "--std=c++14 ${CMAKE_CXX_FLAGS}")
	elseif(MSVC)
	  	#TODO
	else()
		message(Warning: Unknown compiler; Please set C++14 support in CMake)
	endif()	
else()
	set (CMAKE_CXX_STANDARD 14)
endif()

add_subdirectory(src)

