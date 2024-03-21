cmake_minimum_required(VERSION 3.20)
project(scanner CXX)
message("-- Building project: " ${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CONAN_DISABLE_CHECK_COMPILER "1")
set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++")

set(SOURCES
    src/print.cpp
    src/reconnect.cpp
    src/main.cpp
    src/menu.cpp
    src/utility.cpp
    src/commands.cpp
    src/commands_sequencies.cpp
    src/converters.cpp
    src/logger.cpp
)

set(HEADERS
    src/print.h
    src/converters.h
    src/reconnect.h
    src/handler.h
    src/commands_sequencies.h
    src/commands.h
    src/menu.h
    src/utility.h
    src/CppConsoleTable.hpp
    src/logger.h
)

add_subdirectory(src/utfcpp)
set(BUILD_SHARED_LIBS OFF)
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
file(COPY ${CMAKE_BINARY_DIR}/FindBoost.cmake DESTINATION ${modules})
file(COPY ${CMAKE_BINARY_DIR}/FindBZip2.cmake DESTINATION ${modules})
file(COPY ${CMAKE_BINARY_DIR}/FindZLIB.cmake DESTINATION ${modules})

include(FindPkgConfig)
find_package(Threads REQUIRED)

list(APPEND CMAKE_FIND_ROOT_PATH ${CONAN_BOOST_ROOT})
set(Boost_USE_STATIC_LIBS ON)
add_compile_definitions(_WIN32_WINNT=0x0601)

find_package(Boost COMPONENTS json system log log_setup REQUIRED)
set(BUILD_SHARED_LIBS FALSE)

# -------- Firmware downloal library ---------- #
if(NOT DEFINED TARGET_SYSTEM)
    message(FATAL_ERROR "Specify target system first")
    return()
endif()

if(${TARGET_SYSTEM} STREQUAL DEBIAN)
    message(WARNING "-- Debian --")
    find_library(FWLIB_TO_INCLUDE appdownload.a src/lib_x64)
elseif(${TARGET_SYSTEM} STREQUAL CENTOS)
    message(WARNING "-- Centos --")
    find_library(FWLIB_TO_INCLUDE appdownload.a src/lib_x86)
elseif(${TARGET_SYSTEM} STREQUAL WINDOWS)
    message(WARNING "-- Windows --")
    find_library(FWLIB_TO_INCLUDE libfwdownload.lib src/win10)
endif()

# -------- Firmware downloal library ---------- #
find_path(FWLIB_INCLUDES fwdlinterface.h src/include)
include_directories(${FWLIB_INCLUDES})

include_directories(${Boost_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} utf8cpp hidapi::hidapi)
target_link_libraries(${PROJECT_NAME} Boost::thread Boost::system Boost::json Boost::log Boost::log_setup)
target_link_libraries(${PROJECT_NAME} ${FWLIB_TO_INCLUDE})

if(NOT DEFINED _DEBUG)
    message(WARNING "-- Release build --")
endif()

if(${_DEBUG})
    message(WARNING "-- Debug build --")
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG=TRUE)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wunused -g)
else()
    message(WARNING "-- Release build --")
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG=FALSE)
endif()

if(NOT DEFINED LANG)
    message(FATAL_ERROR "Specify the language first")
endif()

if(${LANG} STREQUAL ru_RU)
    configure_file(${CMAKE_SOURCE_DIR}/src/configure/ru_RU.h.in ${CMAKE_SOURCE_DIR}/src/phrases.h @ONLY)
    message(WARNING "-- Language: RU --")
elseif(${LANG} STREQUAL en_US)
    configure_file(${CMAKE_SOURCE_DIR}/src/configure/en_US.h.in ${CMAKE_SOURCE_DIR}/src/phrases.h @ONLY)
    message(WARNING "-- Language: EN --")
endif()

if(${TARGET_SYSTEM} STREQUAL Windows)
    if(STATIC_CRT)
        string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
        string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    endif()

    target_compile_definitions(${PROJECT_NAME} PRIVATE _WINDOWS_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

# conan install -pr .\conan_profiles\win10\win10_build -if build_win_x86_64 . --build=missing
# cmake -Bbuild_win_x86_64 . -DTARGET_SYSTEM=WINDOWS -D_DEBUG=TRUE -DLANG=RU
# cmake --build ./build_win_x86_64