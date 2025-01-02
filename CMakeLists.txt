cmake_minimum_required(VERSION 3.25)

project(ClientServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Architecture and build configurations
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-x64)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DNDEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Windows-specific configuration
if(WIN32)
    set(PLATFORM_FILES ${CMAKE_SOURCE_DIR}/include/Socked/sockedwin32.cpp)
endif()

# Linux-specific configuration
if(UNIX AND NOT WIN32)
    set(PLATFORM_FILES ${CMAKE_SOURCE_DIR}/include/Socked/sockedlinux.cpp)
    # Uncomment below to enable additional options and links
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
    # target_link_libraries(<target_name> omp glfw GL m X11 pthread Xi Xrandr dl)
endif()

file(GLOB_RECURSE CLIENT_FILES
    ${CMAKE_SOURCE_DIR}"/Client/*.c"
    ${CMAKE_SOURCE_DIR}"/Client/*.cpp"
    ${CMAKE_SOURCE_DIR}"/Client/*.h" 
    ${CMAKE_SOURCE_DIR}"/Client/*.hpp" 
)

file (GLOB_RECURSE SERVER_FILES
    ${CMAKE_SOURCE_DIR}"/Server/*.c"
    ${CMAKE_SOURCE_DIR}"/Server/*.cpp"
    ${CMAKE_SOURCE_DIR}"/Server/*.h" 
    ${CMAKE_SOURCE_DIR}"/Server/*.hpp" 
)

# Client project
add_executable(Client
    ${CLIENT_FILES}
    ${PLATFORM_FILES}
)
target_include_directories(Client PRIVATE
    ${CMAKE_SOURCE_DIR}/Client
    ${CMAKE_SOURCE_DIR}/include
)

# Server project
add_executable(Server
    ${SERVER_FILES}
    ${PLATFORM_FILES}
)
target_include_directories(Server PRIVATE
    ${CMAKE_SOURCE_DIR}/Server
    ${CMAKE_SOURCE_DIR}/include
)
