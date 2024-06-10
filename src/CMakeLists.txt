cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

project(GrpExperiment)

# find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
# find_package(PythonLibs 3 REQUIRED)
# set(PYTHON_CMD "${Python3_EXECUTABLE}")

# add_subdirectory(extern)
add_subdirectory(proto)
add_subdirectory(app)
