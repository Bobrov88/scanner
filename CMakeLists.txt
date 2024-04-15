cmake_minimum_required(VERSION 3.20)
project(scanner CXX)
message("-- Building project: " ${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CONAN_DISABLE_CHECK_COMPILER "1")

if(NOT ${TARGET_SYSTEM} STREQUAL WINDOWS)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
endif()

set(SOURCES
    src/print.cpp

    # src/reconnect.cpp
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

    # src/reconnect.h
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

# -------- Firmware downloal library ---------- #
find_path(LIB_INCLUDES fwdlinterface.h src/include)
include_directories(${LIB_INCLUDES})

include_directories(${Boost_INCLUDE_DIRS})

if(${LANG} STREQUAL en_US)
    set(OUTPUT_FILE ${PROJECT_NAME}_en_US)
elseif(${LANG} STREQUAL ru_RU)
    set(OUTPUT_FILE ${PROJECT_NAME}_ru_RU)
endif()

add_executable(${OUTPUT_FILE} ${SOURCES} ${HEADERS})

if(${TARGET_SYSTEM} STREQUAL DEBIAN)
    message(WARNING "-- Debian --")
    find_library(FWLIB_TO_INCLUDE appdownload.a src/lib_x64)
    target_compile_definitions(${OUTPUT_FILE} PUBLIC __DEBIAN__)
elseif(${TARGET_SYSTEM} STREQUAL CENTOS)
    message(WARNING "-- Centos --")
    find_library(FWLIB_TO_INCLUDE appdownload.a src/lib_x86)
    target_compile_definitions(${OUTPUT_FILE} PUBLIC __CENTOS__)
elseif(${TARGET_SYSTEM} STREQUAL WINDOWS)
    message(WARNING "-- Windows --")
    find_library(FWLIB_TO_INCLUDE libfwdownload.lib src/win10)
    target_compile_definitions(${OUTPUT_FILE} PUBLIC __WIN__)
endif()

if(NOT DEFINED _DEBUG)
    message(WARNING "-- Release build --")
endif()

if(${_DEBUG})
    message(WARNING "-- Debug build --")

    if(${TARGET_SYSTEM} STREQUAL DEBIAN OR ${TARGET_SYSTEM} STREQUAL DEBIAN)
        target_compile_options(${OUTPUT_FILE} PRIVATE -Wall -Wextra -Wunused -g)
    endif()

    if(MSVC)
        target_compile_options(${OUTPUT_FILE} PRIVATE /Wall /W4)
    endif()
else()
    message(WARNING "-- Release build --")
endif()

if(NOT DEFINED LANG)
    message(FATAL_ERROR "Specify the language first")
endif()

if(${LANG} STREQUAL ru_RU)
    configure_file(${CMAKE_SOURCE_DIR}/src/configure/ru_RU.h.in ${CMAKE_SOURCE_DIR}/src/phrases.h @ONLY)
    target_compile_definitions(${OUTPUT_FILE} PUBLIC RUS)
    message(WARNING "-- Language: RU --")
elseif(${LANG} STREQUAL en_US)
    configure_file(${CMAKE_SOURCE_DIR}/src/configure/en_US.h.in ${CMAKE_SOURCE_DIR}/src/phrases.h @ONLY)
    target_compile_definitions(${OUTPUT_FILE} PUBLIC ENG)
    message(WARNING "-- Language: EN --")
endif()

if(${TARGET_SYSTEM} STREQUAL WINDOWS)
    if(STATIC_CRT)
        string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
        string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    endif()

    target_compile_definitions(${OUTPUT_FILE} PRIVATE _WINDOWS_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    target_compile_options(${OUTPUT_FILE} PRIVATE /utf-8)
endif()

target_link_libraries(${OUTPUT_FILE} PRIVATE utf8cpp hidapi::hidapi)
target_link_libraries(${OUTPUT_FILE} PRIVATE Boost::thread Boost::system Boost::json Boost::log Boost::log_setup)
target_link_libraries(${OUTPUT_FILE} PRIVATE ${FWLIB_TO_INCLUDE})


# docker build -t scanner_x64 -f ./debian12-x64.Dockerfile .
# docker build -t scanner -f ./centos7-x86.Dockerfile .
# docker build -t scanner_debian_11_x64 -f ./debian11-x64.Dockerfile .

# Windows10 x64 build
# conan install -pr .\conan_profiles\win10\win10_build -if build_win_x86_64 . -s build_type=Debug -s compiler="Visual Studio" -s compiler.runtime=MDd --build=missing
# cmake -Bbuild_win_x86_64 . -DTARGET_SYSTEM=WINDOWS -D_DEBUG=TRUE -DLANG=ru_RU -G "Visual Studio 16" -A x64
# cmake --build ./build_win_x86_64
# Debian12 x64 build

# Centos7 x32 build
# docker run --rm -it --volume=./:/home/project/scanner scanner
# conan install -pr linux_x86_build -if build_centos_x86 /home/project/scanner/ --build=missing
# cmake -Bbuild_centos_x86 . -DTARGET_SYSTEM=CENTOS -D_DEBUG=TRUE -DLANG=ru_RU
# cmake --build ./build_centos_x86

# Debian12 x64 build
# docker run --rm -it --volume=./:/home/project/scanner scanner_x64
# conan install -pr linux_x86_64_build -if build_debian_x86_64 /home/project/scanner/ --build=missing 
# cmake -Bbuild_debian_x86_64 . -DTARGET_SYSTEM=DEBIAN -D_DEBUG=FALSE -DLANG=en_US
# cmake --build ./build_debian_x86_64

# Debian11 x64 build
# docker run --rm -it --volume=./:/home/project/scanner scanner_debian_11_x64
# conan install -pr linux_x86_64_build -if build_debian11_x86_64 /home/project/scanner/ --build=missing 
# cmake -Bbuild_debian11_x86_64 . -DTARGET_SYSTEM=DEBIAN -D_DEBUG=FALSE -DLANG=en_US -Dstatic-libsystemd=true -Dstatic-libudev=true
# cmake --build ./build_debian11_x86_64

# -DTARGET_SYSTEM=WINDOWS
# -DTARGET_SYSTEM=DEBIAN
# -DTARGET_SYSTEM=CENTOS
# -D_DEBUG=TRUE
# -D_DEBUG=FALSE
# -DLANG=ru_RU
# -DLANG=en_US