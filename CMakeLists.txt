cmake_minimum_required(VERSION 3.10) # Min cmake version 3.10 else throw error

project(HelloWorld) # Set project name to HelloWorld

set(CMAKE_PREFIX_PATH "~/.local/bin/vcpkg-git/installed/x64-linux/share") # Look for vcpkg installed packages here

find_package(CURL CONFIG REQUIRED) # Looks for the CURL library using its CMake configuration file. REQUIRED keyword ensures that an error is generated if the package is not found

add_executable(MAIN scraper.cpp) # Adds an executable target named "HelloWorld," built from the source file helloworld.cpp

target_link_libraries(MAIN PRIVATE CURL::libcurl) # link CURL to HelloWorld and PRIVATE tells CURL is only needed for building HelloWorld