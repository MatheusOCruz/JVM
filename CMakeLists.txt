cmake_minimum_required(VERSION 3.25)
project(JVM)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXXFLAGS "-Wall -Wextra -g")

add_executable(
        JVM main.cpp

        include/ClassLoader.h
        include/ClassFileStructs.h
        include/ClassFileEnums.h
        include/typedefs.h
        include/AttributeStructs.h
        include/ClassPrinter.h
        include/ClassLoaderErrors.h
        include/JvmStructs.h
        include/Jvm.h

        src/ClassLoader.cpp
        src/ClassPrinter.cpp
        src/ClassLoaderErrors.cpp
        src/Jvm.cpp
        src/OpcodePrinter.cpp
        include/OpcodePrinter.h
        include/JvmEnums.h
        src/RunTimeExceptions.cpp
        include/RunTimeExceptions.h

)
