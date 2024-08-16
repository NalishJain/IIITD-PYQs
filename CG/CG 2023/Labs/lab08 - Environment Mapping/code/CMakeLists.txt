cmake_minimum_required(VERSION 3.18)
project(Lab07)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_VERBOSE_MAKEFILE 1)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)

set(APP_TARGET ${CMAKE_PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(SOURCES
	"src/main.cpp"
	"src/skybox.cpp"
	"src/utils.cpp"
	"src/sphere.cpp"
	"depends/imgui/imgui_impl_glfw.cpp"
	"depends/imgui/imgui_impl_opengl3.cpp"
	"depends/imgui/imgui.cpp"
	"depends/imgui/imgui_demo.cpp"
	"depends/imgui/imgui_draw.cpp"
	"depends/imgui/imgui_widgets.cpp"
	)
set(HEADERS 
         "depends/stb/stb_image.h"
        )
         
add_executable(${APP_TARGET} ${SOURCES} ${HEADERS})

target_include_directories(${APP_TARGET} PRIVATE
	${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/depends
	${CMAKE_SOURCE_DIR}/depends/imgui
	${CMAKE_SOURCE_DIR}/depends/stb
	)
	

target_link_libraries( ${APP_TARGET}
	glfw
	${OPENGL_LIBRARIES}
	GLEW::GLEW
)
