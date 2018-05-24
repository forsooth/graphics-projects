cmake_minimum_required(VERSION 3.9)
project(a4)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(a4
        Flattener.cpp
        Raycaster.cpp
        Cone.cpp
        Cube.cpp
        Cylinder.cpp
        Sphere.cpp
        Sphere.h
        Cube.h
        Camera.cpp
        Assignment4.cpp
        SceneParser.cpp
        tinystr.cpp
        tinyxml.cpp
        tinyxmlparser.cpp
        tinyxmlerror.cpp)

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

if(APPLE)
    set(GLUI_INCLUDE_DIRS /usr/local/include)
else()
    set(GLUI_INCLUDE_DIRS /usr/include)
endif()

include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLUI_INCLUDE_DIRS})

if (APPLE)
    target_link_libraries(a4 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} /usr/local/lib/glui.dylib)
else()
    target_link_libraries(a4 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} /usr/lib/libglui.a)
endif()