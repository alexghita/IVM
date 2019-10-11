# cmake version to be used
cmake_minimum_required(VERSION 3.10.2)

# project name
project(ivm)

# flags

# files

# include
include_directories(./include ./src)

# target
add_executable(ivm-bin ./src/deltaprocessor.cpp ./src/helperfunctions.cpp 
    ./src/ivmprocessor.cpp ./src/main.cpp ./src/naiveprocessor.cpp 
    ./src/relation.cpp ./src/skewprocessor.cpp ./src/skewrelation.cpp 
    ./src/view.cpp ./src/viewprocessor.cpp)

# external libs