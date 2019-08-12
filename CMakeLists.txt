cmake_minimum_required(VERSION 2.5)
project(capture_screen)
cmake_policy(SET CMP0003 NEW)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -fopenmp -Wl,-rpath=.")
set(CMAKE_BUILD_TYPE Debug)

file(GLOB src "./src/*.c*")

find_package(OpenCV REQUIRED)
find_package(X11 REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(
  ./include/
  ${OpenCV_INCLUDE_DIRS}
  ${X11_INCLUDE_DIRS}
  ${GLFW3_INCLUDE_DIRS}
  /usr/local/include/
  )

link_directories(
  /usr/local/lib/
  /usr/lib/x86_64-linux-gnu/
  )

add_library(mylib SHARED ${src})
add_executable(server Server.cpp)
add_executable(main main.cpp)
add_executable(client Client.cpp)
add_executable(test test_function.cpp)

target_link_libraries(mylib
  ${OpenCV_LIBS}
  ${X11_LIBRARIES}
  Xtst
  glfw
  dl
  )
target_link_libraries(server
  mylib
  )
target_link_libraries(main
  mylib
  )
target_link_libraries(client
  mylib
  )
target_link_libraries(test
  mylib
  )
