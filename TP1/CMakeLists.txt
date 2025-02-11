# CMake entry point
cmake_minimum_required (VERSION 2.6)
project(SI_INFO4)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLEW REQUIRED glew)
pkg_search_module(GLM REQUIRED glm)

include_directories(./include)
include_directories(${OpenGL_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})

set(ALL_LIBS
	${OPENGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${GLFW_LIBRARIES}
	)

file(
        GLOB
        FichiersShader
        shader/*.glsl
    )

file(
	GLOB_RECURSE
	source_files
        src/*.cpp
        src/*.hpp
)

link_directories(${GLFW_LIBRARY_DIRS} ${GLEW_LIBRARY_DIRS})
add_executable(si_info4 ${source_files} )
target_link_libraries(si_info4 ${ALL_LIBS})

