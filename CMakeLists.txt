#################
# Configuration #
#################

# CMake

cmake_minimum_required(VERSION 3.13)

# Project

project(uVec
        VERSION 0.1.0
        DESCRIPTION "A type safe, generic C vector."
        HOMEPAGE_URL "https://github.com/IvanoBilenchi/uvec"
        LANGUAGES C)

###############
### Targets ###
###############

# Interface library

add_library(uvec INTERFACE)
target_sources(uvec INTERFACE "include/uvec.h")
target_include_directories(uvec INTERFACE "include")

# Subprojects

add_subdirectory("test")
add_subdirectory("docs")
