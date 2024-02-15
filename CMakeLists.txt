cmake_minimum_required(VERSION 3.20)
project(scanner CXX)
message("Building project: " ${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CONAN_DISABLE_CHECK_COMPILER "1")

set(SOURCES
    src/reconnect.cpp
    src/main.cpp
    src/menu.cpp
    src/utility.cpp
    src/commands.cpp
    src/commands_sequencies.cpp
)

set(HEADERS
    src/reconnect.h
    src/handler.h
    src/enums/function_name.h
    src/commands_sequencies.h
    src/commands.h
    src/menu.h
    src/utility.h
    src/CppConsoleTable.hpp
)

add_subdirectory(src/utfcpp)
add_subdirectory(src/hidapi)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

set(modules "${CMAKE_SOURCE_DIR}/cmake/Modules")
file(MAKE_DIRECTORY ${modules})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${modules}")
file(COPY ${CMAKE_BINARY_DIR}/Findspdlog.cmake DESTINATION ${modules})
file(COPY ${CMAKE_BINARY_DIR}/Findfmt.cmake DESTINATION ${modules})

# file(COPY ${CMAKE_BINARY_DIR}/Findhidapi.cmake DESTINATION ${modules})
# file(COPY ${CMAKE_BINARY_DIR}/Findlibusb.cmake DESTINATION ${modules})
# file(COPY ${CMAKE_BINARY_DIR}/Findlibudev.cmake DESTINATION ${modules})
include(FindPkgConfig)
find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)

# find_package(hidapi REQUIRED)
# find_package(libusb REQUIRED)
# find_package(libudev REQUIRED)
list(APPEND CMAKE_FIND_ROOT_PATH ${CONAN_BOOST_ROOT})
set(Boost_USE_STATIC_LIBS ON)
add_compile_definitions(_WIN32_WINNT=0x0601)

find_package(Boost COMPONENTS json log system REQUIRED)
set(BUILD_SHARED_LIBS FALSE)

# -------- Firmware downloal library ---------- #
find_library(FWLIB_TO_INCLUDE appdownload.a src/lib)
find_path(FWLIB_INCLUDES fwdlinterface.h src/include)
include_directories(${FWLIB_INCLUDES})

# -------- Firmware downloal library ---------- #

# include_directories(${hidapi_INCLUDES})
if(Boost_FOUND)
    message("-- Boost found... Linking...")
    include_directories(${Boost_INCLUDE_DIRS})

    # include_directories(${hidapi_INCLUDE_DIRS})
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

    target_link_libraries(${PROJECT_NAME} utf8cpp hidapi::hidapi spdlog::spdlog)
    target_link_libraries(${PROJECT_NAME} Boost::thread Boost::system Boost::json Boost::log)
    target_link_libraries(${PROJECT_NAME} ${FWLIB_TO_INCLUDE})
endif()