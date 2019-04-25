# Set the name and the supported language of the project
PROJECT(NetworkWizard CXX)
# Set the minimum version of cmake required to build this project
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# Use the package PkgConfig to detect GTK+ headers/library files
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    message("ENABLED OpenMP")
endif()


include_directories(${GTK3_INCLUDE_DIRS} include)
link_directories(${GTK3_LIBRARY_DIRS})

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})

# Add an executable compiled from hello.c
add_executable(${CMAKE_PROJECT_NAME} main.cpp
    src/app_signals.cpp
    src/network_wizard_app.cpp)

# Link the target to the GTK+ libraries
target_link_libraries(${CMAKE_PROJECT_NAME} ${GTK3_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} sub::NetworkWizard)