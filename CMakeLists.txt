cmake_minimum_required(VERSION 3.2)
project(scanner CXX)
message("Building project: " ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CONAN_DISABLE_CHECK_COMPILER "1")
set(SOURCES
    src/main.cpp
    src/utility.cpp)

set(HEADERS
    src/utility.h
    src/CppConsoleTable/CppConsoleTable.hpp
    src/enums/conf-1.enums.h
    src/enums/conf-2.enums.h
    src/enums/conf-3.enums.h
    src/config1_data.h
    src/config2_data.h
    src/config3_data.h)

add_subdirectory(src/hidapi)
add_subdirectory(src/utfcpp)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

find_package(Boost REQUIRED)
include_directories( ${Boost_INCLUDE_DIR} )

list(APPEND CMAKE_FIND_ROOT_PATH ${CONAN_BOOST_ROOT})
set(Boost_USE_STATIC_LIBS ON)
set(BUILD_SHARED_LIBS FALSE)
add_compile_definitions(_WIN32_WINNT=0x0601)

find_package(Boost COMPONENTS REQUIRED)

if(Boost_FOUND)
    message("Boost found... Linking...")
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} hidapi::hidapi utf8cpp)
endif()
