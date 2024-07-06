cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

project(GrpExperiment)

# find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
# find_package(PythonLibs 3 REQUIRED)
# set(PYTHON_CMD "${Python3_EXECUTABLE}")

option(BUILD_TESTS "" OFF)

add_subdirectory(extern)
add_subdirectory(messages)
add_subdirectory(carpet)
add_subdirectory(app)
