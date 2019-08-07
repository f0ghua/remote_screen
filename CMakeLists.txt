cmake_minimum_required(VERSION 2.5)
project(capture_screen)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp -Wl,-rpath=.")
set(CMAKE_BUILD_TYPE Debug)

file(GLOB src "./*.c*" "./src/*.cpp")

find_package(OpenCV REQUIRED)
find_package(X11 REQUIRED)

include_directories(
  ./include/
  ${OpenCV_INCLUDE_DIRS}
  ${X11_INCLUDE_DIRS}
  )

add_executable(main ${src})

target_link_libraries(main 
  ${OpenCV_LIBS}
  ${X11_LIBRARIES}
  )
