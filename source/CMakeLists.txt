cmake_minimum_required(VERSION 3.10)
project(Settlers4Patch LANGUAGES CXX)

#add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DCMAKE_GENERATOR_PLATFORM=Win32)

add_subdirectory(utilities)

# Set the output as a DLL
add_library(${PROJECT_NAME} SHARED
	dllmain.cpp
    Config.cpp
    Patch.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE CppUtils)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/ddraw.def")

# Optional: set C++ standard
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Optional: set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    
    OUTPUT_NAME "ddraw"
	SUFFIX ".dll"
)

target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)
