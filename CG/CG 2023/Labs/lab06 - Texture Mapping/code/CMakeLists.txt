cmake_minimum_required(VERSION 3.18)
project(lab6)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_VERBOSE_MAKEFILE 1)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)

set(APP_TARGET ${CMAKE_PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
#link_libraries(${GLFW_LIBRARY_DIRS})

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(SOURCES
	"main.cpp"
	"imgui/imgui_impl_glfw.cpp"
	"imgui/imgui_impl_opengl3.cpp"
	"imgui/imgui.cpp"
	"imgui/imgui_demo.cpp"
	"imgui/imgui_draw.cpp"
	"imgui/imgui_widgets.cpp"
	)
set(HEADERS 
         "stb/stb_image.h"
         "stb/stb_image_write.h"
        )
         
add_executable(${APP_TARGET} ${SOURCES} ${HEADERS})

target_include_directories(${APP_TARGET} PRIVATE
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/imgui
	${CMAKE_SOURCE_DIR}/stb
	)
	

target_link_libraries( ${APP_TARGET}
	glfw
	${OPENGL_LIBRARIES}
	GLEW::GLEW
)
