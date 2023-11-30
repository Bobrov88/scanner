cmake_minimum_required(VERSION 3.2)
project(scanner CXX)

set(CMAKE_CXX_STANDARD 17)
set(CONAN_DISABLE_CHECK_COMPILER "1")
set(SOURCES main.cpp)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        conan_basic_setup()
    else()
        message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
    endif()

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS filesystem regex REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
    target_link_libraries(${PROJECT_NAME} Boost::filesystem Boost::regex)
endif()
