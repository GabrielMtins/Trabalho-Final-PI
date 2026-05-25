cmake_minimum_required(VERSION 3.5)
project(projeto)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE all_SRCS
		"${PROJECT_SOURCE_DIR}/src/*.cpp"
        )

file(GLOB IMGUI_SRCS
		"${PROJECT_SOURCE_DIR}/external/imgui/imgui*.cpp"
		"${PROJECT_SOURCE_DIR}/external/imgui/backends/imgui_impl_sdlrenderer2.cpp"
		"${PROJECT_SOURCE_DIR}/external/imgui/backends/imgui_impl_sdl2.cpp"
		)

add_executable(${PROJECT_NAME} ${all_SRCS} ${IMGUI_SRCS})

target_compile_options(${PROJECT_NAME} PRIVATE 
    -Wall
    -Wextra
    -Wpedantic
)

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)

find_library(MATH_LIBRARY m)
find_package(OpenMP REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
	${SDL2_IMAGE_INCLUDE_DIRS}
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/external/imgui/"
	"${PROJECT_SOURCE_DIR}/external/imgui/backends/"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL2_LIBRARIES}
	${SDL2_IMAGE_LIBRARIES}
	${MATH_LIBRARY}
	OpenMP::OpenMP_CXX
)
