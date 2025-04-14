cmake_minimum_required(VERSION 3.14)
project(ChatAPI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find packages
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(fmt REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

# Includes
include_directories(${PROJECT_SOURCE_DIR}/src)

# Collect all .cpp files in src/ recursively
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Sources
add_executable(rest_api ${SOURCES})

# Includes
target_include_directories(rest_api PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${PQXX_INCLUDE_DIRS}
)

# Link
target_link_libraries(rest_api
    ${Boost_LIBRARIES}
    ${PQXX_LIBRARIES}
    pthread
    fmt::fmt
)