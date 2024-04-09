cmake_minimum_required(VERSION 3.26)
project(JVM)

set(CMAKE_CXX_STANDARD 11)

add_executable(JVM main.cpp
        ClassLoader.cpp
        ClassLoader.h
        ClassFileStructs.h
        ClassFileEnums.h
        typedefs.h
)
