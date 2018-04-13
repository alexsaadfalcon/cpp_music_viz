cmake_minimum_required(VERSION 3.9)
project(p3)

set(CMAKE_CXX_STANDARD 11)

# don't want to build the GLFW tests, examples and documentation
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)


include_directories(lib/glad/include)

# Add the executable
add_executable(p3 main.cpp glad.c)

# glfw
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
link_libraries(${GLFW_LIBRARY_DIRS})

# opengl
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# glew
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

target_link_libraries(
        p3
        glfw
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${EXTRA_LIBS}
)
#cmake_minimum_required(VERSION 3.9)
#project(p3)
#include_directories(
#        lib/glad/include
#        lib/glfw/include
#        lib/glew/include)
#
#find_package(GLEW REQUIRED)
#if (GLEW_FOUND)
#    include_directories(${GLEW_INCLUDE_DIRS})
#    link_libraries(${GLEW_LIBRARIES})
#endif()
#
#set(CMAKE_CXX_STANDARD 11)
#add_subdirectory(lib/glfw)
#add_executable(p3 main.cpp glad.c)
#target_link_libraries(p3 glfw)