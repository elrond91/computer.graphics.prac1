cmake_minimum_required(VERSION 2.8...3.13)

project(practice_1)

find_package(OpenGL REQUIRED)
# install sudo apt-get install freeglut3-dev
find_package(GLUT REQUIRED)

if (APPLE)
  set(CMAKE_MACOSX_RPATH 1)
endif()

if (UNIX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=gnu++17")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-variable")
  # recommended but not set by default
  # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
elseif(MSVC)
  # recommended but not set by default
  # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -WX")
endif()

set (practice_1_LIBS ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )

include_directories( 
  include
  computer.graphics.vecmath/include
  ${OPENGL_INCLUDE_DIRS}  
  ${GLUT_INCLUDE_DIRS} )


# vecmath include directory
# git clone https://github.com/elrond91/computer.graphics.vecmath.git
add_subdirectory(computer.graphics.vecmath)
list (APPEND practice_1_SRC
  src/main.cpp
  src/camera.cpp
  src/curve.cpp
  src/parse.cpp
  src/surf.cpp
)

add_executable(pract1 ${practice_1_SRC})
target_link_libraries(pract1 ${practice_1_LIBS} vecmath)
