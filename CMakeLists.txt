cmake_minimum_required(VERSION 3.26)
project(JVM)

set(CMAKE_CXX_STANDARD 11)

add_executable(JVM main.cpp
        src/ClassLoader.cpp
        include/ClassLoader.h
        include/ClassFileStructs.h
        include/ClassFileEnums.h
        include/typedefs.h
        include/Attributes.h
        src/ClassPrinter.cpp
        include/ClassPrinter.h
)
