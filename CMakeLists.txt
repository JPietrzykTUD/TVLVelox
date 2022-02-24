cmake_minimum_required(VERSION 3.16)
project(VeloxPoc)

set(CMAKE_BUILD_TYPE "Release")

if(UNIX AND NOT APPLE)
    # codegen linker flags, -export-dynamic for rtti
    add_link_options("-Wl,-export-dynamic")
endif()
set(POSITION_INDEPENDENT_CODE True)

set(CMAKE_CXX_STANDARD 20)

execute_process(COMMAND uname -p WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE PLATFORM OUTPUT_STRIP_TRAILING_WHITESPACE)
if("${PLATFORM}" STREQUAL "aarch64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNEON -mabi=lp64 -march=armv8.2-a+lse -O3")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mavx512f -mavx512dq -mavx2 -mavx -msse4.2 -O3")
endif()
message(STATUS "Platform: ${PLATFORM}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D HAS_UNCAUGHT_EXCEPTIONS")

set(Boost_USE_MULTITHREADED TRUE)
find_package(
        Boost
        1.66.0
        REQUIRED
        program_options
        context
        filesystem
        regex
        thread
        system
        date_time
        atomic)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# Range-v3 will be enable when the codegen code actually lands keeping it here
# for reference. find_package(range-v3)

find_package(GTest REQUIRED)
find_package(gflags COMPONENTS shared)

find_library(GMock gmock)

#find_package(glog REQUIRED)
include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})

find_library(FMT fmt)

find_library(EVENT event)

find_library(DOUBLE_CONVERSION double-conversion)

find_library(LZ4 lz4)
find_library(LZO lzo2)
find_library(RE2 re2 REQUIRED)
find_library(ZSTD zstd)
find_package(ZLIB)
find_library(SNAPPY snappy)


find_package(folly CONFIG REQUIRED)
set(FOLLY_WITH_DEPENDENCIES
        ${FOLLY_LIBRARIES} ${Boost_LIBRARIES} ${DOUBLE_CONVERSION_LIBRARIES}
        ${EVENT} ${SNAPPY} ${CMAKE_DL_LIBS})

set(FOLLY ${FOLLY_LIBRARIES})
set(FOLLY_BENCHMARK Folly::follybenchmark)

find_package(BZip2 MODULE)
if(BZIP2_FOUND)
    list(APPEND FOLLY_WITH_DEPENDENCIES ${BZIP2_LIBRARIES})
endif()

include_directories(SYSTEM ${FOLLY_INCLUDE_DIRS})

find_package(Protobuf 3.0.0 REQUIRED)

# GCC needs to link a library to enable std::filesystem.
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(FILESYSTEM "stdc++fs")

    # Ensure we have gcc at least 8+.
    if(CMAKE_CXX_COMPILER_VERSION LESS 8.0)
        message(
                FATAL_ERROR "VELOX requires gcc > 8. Found ${CMAKE_CXX_COMPILER_VERSION}")
    endif()
else()
    set(FILESYSTEM "")
endif()
include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/libs)


file(GLOB SOURCES
    src/*.cpp
)

message(STATUS "files: ${SOURCES}")
add_executable(VeloxPoc main.cpp ${SOURCES})

target_link_libraries(
        VeloxPoc
        ${FOLLY}
        ${FOLLY_BENCHMARK}
        ${DOUBLE_CONVERSION}
        ${GTEST_BOTH_LIBRARIES}
        ${gflags_LIBRARIES}
        )
