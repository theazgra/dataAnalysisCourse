cmake_minimum_required(VERSION 3.0.0)
project(app)


add_executable(${PROJECT_NAME} main.cpp
        tutorials/tutorial_1.h)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
target_link_libraries(${PROJECT_NAME} sub::azgra)