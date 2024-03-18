cmake_minimum_required(VERSION 3.16)
project(zenohc_examples
        VERSION 0.1.0
        LANGUAGES CXX)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/zenohc-install/lib/cmake/")
find_package(zenohc REQUIRED)

add_executable(zenohc_example src/z_example.cxx)
#add_dependencies(zenohc_example zenohc::lib)
target_link_libraries(zenohc_example PRIVATE zenohc::lib)
