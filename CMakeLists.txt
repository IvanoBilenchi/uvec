#################
# Configuration #
#################

# CMake

cmake_minimum_required(VERSION 3.12)

# Project

set(VEC_NAME "Vector(T)")

project(vector_t
        VERSION 0.1.0
        DESCRIPTION "A type safe, generic C vector."
        HOMEPAGE_URL "https://github.com/IvanoBilenchi/vector_t"
        LANGUAGES C)

# Files and directories

set(VEC_PROJECT_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(VEC_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(VEC_INCLUDE_DIR "${VEC_PROJECT_DIR}/include")
set(VEC_DOCS_DIR "${VEC_PROJECT_DIR}/docs")
set(VEC_TEST_DIR "${VEC_PROJECT_DIR}/test")
set(VEC_HEADER "${VEC_INCLUDE_DIR}/vector.h")

###############
### Targets ###
###############

# Interface library

add_library(vector INTERFACE)
target_sources(vector INTERFACE "${VEC_HEADER}")
target_include_directories(vector INTERFACE "${VEC_INCLUDE_DIR}")

# Subprojects

add_subdirectory("${VEC_TEST_DIR}")
add_subdirectory("${VEC_DOCS_DIR}")
