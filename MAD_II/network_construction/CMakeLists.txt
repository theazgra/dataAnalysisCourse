cmake_minimum_required(VERSION 3.0.0)
project(DataAnalysis VERSION 0.1.0)
set(CMAKE_CXX_FLAGS "-Wall")

######################## LIBRARY #######################################



add_library(NetworkWizard STATIC 
        src/network_matrix.cpp
        src/simple_string.cpp
        src/io.cpp
        src/graph_component.cpp
    )
target_include_directories(NetworkWizard PUBLIC ${PROJECT_SOURCE_DIR}/include)

#add_library(sub::azgra ALIAS ${PROJECT_NAME})
#target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

##################################################################################

find_package(OpenMP)
if (OPENMP_FOUND)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories( ./ include)

add_executable(DataAnalysis main.cpp
    src/network_matrix.cpp
    src/simple_string.cpp
    src/io.cpp
    src/graph_component.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
