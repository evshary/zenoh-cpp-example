cmake_minimum_required(VERSION 3.16)
project(zenohc_routers
        VERSION 0.1.0
        LANGUAGES CXX)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/zenohc-install/lib/cmake/")
find_package(zenohc REQUIRED)

add_executable(zenoh_router src/zenohc/z_router.cxx)
target_link_libraries(zenoh_router PRIVATE zenohc::lib)

add_executable(zenoh_subscriber src/zenohc/z_sub.cxx)
target_link_libraries(zenoh_subscriber PRIVATE zenohc::lib)

add_executable(zenoh_get src/zenohc/z_get.cxx)
target_link_libraries(zenoh_get PRIVATE zenohc::lib)

add_executable(zenoh_put src/zenohc/z_put.cxx)
target_link_libraries(zenoh_put PRIVATE zenohc::lib)
