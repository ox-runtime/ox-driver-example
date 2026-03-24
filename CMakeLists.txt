cmake_minimum_required(VERSION 3.27)
project(ox-driver-example VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(OX_DRIVER_HEADER_LOCAL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ox_driver.h")
if(EXISTS "${OX_DRIVER_HEADER_LOCAL_PATH}")
    set(OX_DRIVER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
else()
    FetchContent_Declare(
        ox_driver_header
        URL https://github.com/ox-runtime/ox-runtime/releases/latest/download/ox_driver.h
        DOWNLOAD_NO_EXTRACT TRUE
        DOWNLOAD_NAME ox_driver.h
    )
    FetchContent_GetProperties(ox_driver_header)
    if(NOT ox_driver_header_POPULATED)
        FetchContent_Populate(ox_driver_header)
    endif()
    set(OX_DRIVER_INCLUDE_DIR "${ox_driver_header_SOURCE_DIR}")
endif()

set(OUTPUT_FOLDER "example_driver")

# Driver shared library - output as ox_driver.dll / libox_driver.so / libox_driver.dylib
add_library(example_driver SHARED example_driver.cpp)

target_include_directories(example_driver PRIVATE
    ${OX_DRIVER_INCLUDE_DIR}
)

# Rename to standardized driver library name
set_target_properties(example_driver PROPERTIES
    OUTPUT_NAME "ox_driver"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER}
)

# Print build completion message
add_custom_command(TARGET example_driver POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "====================================================================="
    COMMAND ${CMAKE_COMMAND} -E echo "Build complete!"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Copy ${CMAKE_BINARY_DIR}/${OUTPUT_FOLDER} to your ox runtime's 'drivers' folder"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "====================================================================="
    VERBATIM
)
