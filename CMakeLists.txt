cmake_minimum_required(VERSION 3.13)

project(minecraft)

# find src/* -name "*.cpp" -printf "%p\n"
add_executable(minecraft "src/main.cpp" "src/window.h" "src/window.cpp" "src/states/state.h" "src/states/state.cpp" "src/states/game_state.h" "src/states/game_state.cpp" "src/gl/shader.h" "src/gl/shader.cpp" "src/utils/file.h" "src/utils/file.cpp" "src/gl/texture.h" "src/gl/texture.cpp" "src/text/font.h" "src/text/font.cpp" "src/text/text.h" "src/text/text.cpp" "src/utils/timer.h" "src/utils/timer.cpp" "src/utils/rect_pack.h" "src/utils/rect_pack.cpp" "src/utils/math.h" "src/utils/math.cpp" "src/rendering/camera.h" "src/rendering/camera.cpp" "src/entity.h" "src/entity.cpp" "src/world/chunk.h" "src/world/chunk.cpp" "src/utils/hash.h" "src/world/chunk_manager.h" "src/world/chunk_manager.cpp" "src/utils/debug.h")

target_compile_features(minecraft PUBLIC cxx_std_17)
target_compile_options(minecraft PUBLIC
	# $<$<CXX_COMPILER_ID:MSVC>:/W4> # /WX
	# $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic> # -Werror
)

add_subdirectory(lib/glfw)
add_subdirectory(lib/glad)
add_subdirectory(lib/glm)
add_subdirectory(lib/stb)
add_subdirectory(lib/freetype)

target_include_directories(minecraft
	PRIVATE .
)

target_link_libraries(minecraft
	PRIVATE glfw glad glm stb freetype
)

# Copy data files to build directory
#add_custom_command(TARGET minecraft POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
#)
