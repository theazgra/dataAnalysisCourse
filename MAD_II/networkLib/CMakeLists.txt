cmake_minimum_required(VERSION 3.0.0)
project(DataAnalysis VERSION 0.1.0)
set(CMAKE_CXX_FLAGS "-Wall")

find_package(OpenMP)
if (OPENMP_FOUND)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories( ./ include)

######################## LIBRARY #######################################

add_library(NetworkWizard STATIC 
        src/network_matrix.cpp
        src/simple_string.cpp
        src/io.cpp
        src/graph_component.cpp
        src/network_generator.cpp
        src/network_layer.cpp
        src/multilayer_network.cpp
        src/network_filtering.cpp
        src/Stopwatch.cpp
    )
target_include_directories(NetworkWizard PUBLIC ${PROJECT_SOURCE_DIR}/include)
# add_library(sub::NetworkWizard ALIAS NetworkWizard)

##################################################################################

add_executable(DataAnalysis main.cpp
    src/network_matrix.cpp
    src/simple_string.cpp
    src/io.cpp
    src/graph_component.cpp
    src/network_generator.cpp
    src/network_layer.cpp
    src/multilayer_network.cpp
    src/network_filtering.cpp
    src/Stopwatch.cpp
    )