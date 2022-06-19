cmake_minimum_required(VERSION 3.13)

project(minecraft)

# find src/* -name "*.cpp" -printf "%p\n"
add_executable(minecraft
	src/main.cpp
)

target_compile_features(minecraft PUBLIC cxx_std_17)
target_compile_options(minecraft PUBLIC
	$<$<CXX_COMPILER_ID:MSVC>:/W4> # /WX
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic> # -Werror
)

add_subdirectory(lib/glfw)
add_subdirectory(lib/glad)

target_include_directories(minecraft
	PRIVATE .
)

target_link_libraries(minecraft
	PRIVATE glfw glad
)
