cmake_minimum_required(VERSION 3.0)
project(JVM)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXXFLAGS "-Wall -Wextra -g")
set(CMAKE_CXXFLAGS "fsanitize=address,undefined")

# configuração pra usar ponteiro de 32 bits
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -g")
endif()


add_executable(
        JVM main.cpp

        include/ClassLoader/ClassLoader.h
        include/ClassLoader/ClassFileStructs.h
        include/ClassLoader/ClassFileEnums.h
        include/typedefs.h
        include/ClassLoader/AttributeStructs.h
        include/ClassPrinter/ClassPrinter.h
        include/ClassLoader/ClassLoaderErrors.h
        include/Jvm/JvmStructs.h
        include/Jvm/Jvm.h

        src/ClassLoader/ClassLoader.cpp
        src/ClassPrinter/ClassPrinter.cpp
        src/ClassLoader/ClassLoaderErrors.cpp
        src/Jvm/Jvm.cpp
        src/ClassPrinter/OpcodePrinter.cpp
        include/ClassPrinter/OpcodePrinter.h
        include/Jvm/JvmEnums.h
        src/Jvm/RunTimeExceptions.cpp
        include/Jvm/RunTimeExceptions.h
        src/utils.cpp
        include/utils.h

)
