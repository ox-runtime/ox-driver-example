cmake_minimum_required(VERSION 3.27)
project(ox-driver-example VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(OX_RUNTIME_REPO "" CACHE PATH "Local path to the ox-runtime repository for ox_driver_api; uses FetchContent when empty")

if(OX_RUNTIME_REPO)
    get_filename_component(OX_RUNTIME_SOURCE_DIR "${OX_RUNTIME_REPO}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    if(NOT EXISTS "${OX_RUNTIME_SOURCE_DIR}/include/CMakeLists.txt")
        message(FATAL_ERROR "OX_RUNTIME_REPO must point to the ox-runtime repository root: ${OX_RUNTIME_SOURCE_DIR}")
    endif()
    FetchContent_Declare(
        ox_driver_api
        SOURCE_DIR "${OX_RUNTIME_SOURCE_DIR}"
        SOURCE_SUBDIR include
    )
else()
    FetchContent_Declare(
        ox_driver_api
        GIT_REPOSITORY https://github.com/ox-runtime/ox-runtime.git
        GIT_TAG v0.2.0
        GIT_SHALLOW TRUE
        GIT_SPARSE_CHECKOUT include
        SOURCE_SUBDIR include
        EXCLUDE_FROM_ALL
    )
endif()
FetchContent_MakeAvailable(ox_driver_api)

set(OUTPUT_FOLDER "example_driver")

# Driver shared library - output as ox_driver.dll / libox_driver.so / libox_driver.dylib
add_library(example_driver SHARED example_driver.cpp)

target_link_libraries(example_driver PRIVATE ox_driver_api)

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
