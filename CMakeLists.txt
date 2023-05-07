cmake_minimum_required(VERSION 3.7)
project(blaster)

# Set compiler flags for build types
if (WIN32)
set(CMAKE_CXX_FLAGS "-Wall -std=c++17 -static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -std=c++17 -static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -std=c++17 -static-libgcc -static-libstdc++")
else()
set(CMAKE_CXX_FLAGS "-Wall -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -std=c++17")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -std=c++17")
endif(WIN32)

# Include custom cmake modules used to search for SDL libraries
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

# Find SDL libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_net REQUIRED)

# Glob local project sources and headers
file(GLOB_RECURSE blaster_SOURCES "src/*.cpp")
file(GLOB_RECURSE blaster_HEADERS "src/*.h")
file(GLOB_RECURSE blaster_FBS "src/*.fbs")

# Set include directories by looping through the header files
set (blaster_INCLUDE_DIRS "")
foreach (_headerFile ${blaster_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND blaster_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES blaster_INCLUDE_DIRS)

# Define executable, local includes and linking
add_executable(${PROJECT_NAME} ${blaster_SOURCES} ${blaster_FBS})
target_include_directories(${PROJECT_NAME} PRIVATE ${blaster_INCLUDE_DIRS} include/)
target_link_libraries(${PROJECT_NAME} SDL2::Main SDL2::TTF SDL2::Net)

# Copy .ini to binary output folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/options.ini
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Add custom make commands for command line usage
ADD_CUSTOM_TARGET(debug
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Creating the executable in the debug mode.")

ADD_CUSTOM_TARGET(release
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Creating the executable in the release mode.")
