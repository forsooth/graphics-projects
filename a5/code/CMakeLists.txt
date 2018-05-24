cmake_minimum_required(VERSION 3.9)
project(a5)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -DONCURRENT -Ofast")

add_executable(a5
        Flattener.cpp
        ppm.cpp
        Box.cpp
        KDNode.cpp
        KDTree.cpp
        Raycaster.cpp
        Cone.cpp
        Cube.cpp
        Cylinder.cpp
        Sphere.cpp
        Camera.cpp
        Assignment5.cpp
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
    target_link_libraries(a5 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} /usr/local/lib/glui.dylib)
else()
    target_link_libraries(a5 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} /usr/lib/libglui.a)
endif()