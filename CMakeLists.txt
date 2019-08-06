cmake_minimum_required(VERSION 2.5)
project(capture_screen)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp -Wl,-rpath .")

file(GLOB src "./*.c*")

find_package(OpenCV REQUIRED)
find_package(X11 REQUIRED)

include_directories(
  ${OpenCV_INCLUDE_DIRS}
  ${X11_INCLUDE_DIRS}
  )

add_executable(main ${src})

target_link_libraries(main 
  ${OpenCV_LIBS}
  ${X11_LIBRARIES}
  )
