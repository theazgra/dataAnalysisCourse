cmake_minimum_required(VERSION 3.0.0)
project(app)

# NOTE(Moravec): We are building only project files.
add_executable(${PROJECT_NAME} main.cpp
        project/csv_data_column.cpp
        project/ecsv_loader.cpp
        project/ecsv.cpp
        project/dataframe.cpp
        project/regression_tree.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

target_link_libraries(${PROJECT_NAME} sub::azgra)
# target_link_libraries(${PROJECT_NAME} sub::NetworkWizard)
