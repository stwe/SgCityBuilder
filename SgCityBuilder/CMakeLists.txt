cmake_minimum_required(VERSION 3.16)

project(SgCityBuilder)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE CITY_SRC_FILES
    "*.h"
    "*.cpp"
)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(${PROJECT_NAME} PUBLIC src)

add_executable(${PROJECT_NAME} ${CITY_SRC_FILES})

target_link_libraries(${PROJECT_NAME} SgOglLib)
