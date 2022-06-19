cmake_minimum_required(VERSION 3.13)

project(minecraft)

# find src/* -name "*.cpp" -printf "%p\n"
add_executable(minecraft
	src/main.cpp
)

add_subdirectory(lib/glfw)
add_subdirectory(lib/glad)

target_include_directories(minecraft
	PRIVATE .
)

target_link_libraries(minecraft
	PRIVATE glfw glad
)
