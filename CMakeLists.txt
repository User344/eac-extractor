cmake_minimum_required(VERSION 3.21)
project(eac-extractor LANGUAGES CXX VERSION 1.0)

# For VIM
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
endif()

add_executable(${PROJECT_NAME} src/main.cpp src/eac.cpp src/parser.cpp src/dumper.cpp src/common/io.cpp src/common/pe.cpp src/common/memory.cpp src/common/net.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23 CXX_EXTENSIONS OFF CXX_STANDARD_REQUIRED ON)

# curl

include(FetchContent)

FetchContent_Declare(curl
    URL https://curl.se/download/curl-8.5.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP true
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(curl)

find_package(curl)

target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)
