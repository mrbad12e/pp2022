cmake_minimum_required(VERSION 3.0)
project(acopath)
set(SOURCE main.cpp antsystem.cpp adaptivesystem.cpp)
add_executable(${PROJECT_NAME} ${SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -stdlib=libc++ -Wall")

