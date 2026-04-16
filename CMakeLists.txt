cmake_minimum_required(VERSION 3.16)
project(zenohc_routers
        VERSION 0.1.0
        LANGUAGES CXX)

option(BUILD_ZENOHC "Build zenoh-c examples" ON)
option(BUILD_ZENOHCPP_ZENOHC "Build zenoh-cpp examples with zenoh-c backend" OFF)
option(BUILD_ZENOHCPP_ZENOHPICO "Build zenoh-cpp examples with zenoh-pico backend" OFF)

# Build zenohc example
if(BUILD_ZENOHC)
    find_package(zenohc REQUIRED)

    add_executable(zenoh_router src/zenohc/z_router.cxx)
    target_link_libraries(zenoh_router PRIVATE zenohc::lib)

    add_executable(zenoh_subscriber src/zenohc/z_sub.cxx)
    target_link_libraries(zenoh_subscriber PRIVATE zenohc::lib)

    add_executable(zenoh_get src/zenohc/z_get.cxx)
    target_link_libraries(zenoh_get PRIVATE zenohc::lib)

    add_executable(zenoh_put src/zenohc/z_put.cxx)
    target_link_libraries(zenoh_put PRIVATE zenohc::lib)
endif()

# Build zenohc++ example with zenoh-c backend
if(BUILD_ZENOHCPP_ZENOHC)
    find_package(zenohc REQUIRED)
    find_package(zenohcxx REQUIRED)

    add_executable(zenoh_put_cpp_zc src/zenohcpp/z_put.cxx)
    target_link_libraries(zenoh_put_cpp_zc PRIVATE zenohc::lib zenohcxx::zenohc)
endif()

# Build zenohc++ example with zenoh-pico backend
if(BUILD_ZENOHCPP_ZENOHPICO)
    find_package(zenohpico REQUIRED)
    find_package(zenohcxx REQUIRED)

    add_executable(zenoh_put_cpp_zpico src/zenohcpp/z_put.cxx)
    target_link_libraries(zenoh_put_cpp_zpico PRIVATE zenohpico::lib zenohcxx::zenohpico)
endif()
