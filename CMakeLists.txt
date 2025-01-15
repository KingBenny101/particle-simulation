cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/resources)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

find_package (Eigen3 3.3 REQUIRED NO_MODULE)
 
include_directories(include)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE SFML::Graphics Eigen3::Eigen)

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${RESOURCES_DIR}/ $<TARGET_FILE_DIR:main>/resources
)