cmake_minimum_required(VERSION 3.2)
project(scanner CXX)
message("Building project: " ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CONAN_DISABLE_CHECK_COMPILER "1")
set(SOURCES
    src/main.cpp
    src/utility.cpp
)

set(HEADERS
    src/utility.h
    src/CppConsoleTable/CppConsoleTable.hpp
    src/enums/conf-1.enums.h
    src/enums/conf-2.enums.h
    src/enums/conf-3.enums.h
    src/config1_data.h
    src/config2_data.h
    src/config3_data.h
)

add_subdirectory(src/utfcpp)
add_subdirectory(src/hidapi)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
#find_package(libusb REQUIRED)

list(APPEND CMAKE_FIND_ROOT_PATH ${CONAN_BOOST_ROOT})
set(Boost_USE_STATIC_LIBS ON)
add_compile_definitions(_WIN32_WINNT=0x0601)

find_package(Boost COMPONENTS REQUIRED)
set(BUILD_SHARED_LIBS FALSE)

if(libusb_FOUND)
include_directories(${libusb_INCLUDE_DIR})
message("libusb found... Linking...")
endif()

if(Boost_FOUND)
message("Boost found... Linking...")
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} Threads::Threads)
    target_link_libraries(${PROJECT_NAME} utf8cpp hidapi::hidapi)
endif()
