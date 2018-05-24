cmake_minimum_required(VERSION 3.9)
project(lab6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(lab6
        spline.cpp
        main.cpp)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

if(APPLE)
    set(GLUI_INCLUDE_DIRS /usr/local/include)
else()
    set(GLUI_INCLUDE_DIRS /usr/include)
endif()

include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLUI_INCLUDE_DIRS})

if (APPLE)
    target_link_libraries(lab6 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} /usr/local/lib/glui.dylib)
else()
    target_link_libraries(lab6 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} /usr/lib/libglui.a)
endif()