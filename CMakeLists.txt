cmake_minimum_required(VERSION 3.16)

project(glfw-demo)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD 11)

include_directories(glad/include)
include_directories(third-party/glfw/include)

link_directories(third-party/glfw/lib)

set(SRCS
	main/main.c
	main/vap-decoder.c
	main/vap-renderer.c
	main/vap-queue.c
	main/vap-window.c
	main/vap-dump.c
	glad/src/glad.c
)

add_executable(glfw-demo ${SRCS})
target_link_libraries(glfw-demo PUBLIC glfw3)