cmake_minimum_required(VERSION 3.0)
project(JVM)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXXFLAGS "-Wall -Wextra -g")
set(CMAKE_CXXFLAGS "fsanitize=address,undefined")

# configuração pra usar ponteiro de 32 bits
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    #compilador 32 bits para windows
    if (WIN32)
        set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -g")
endif()


add_executable(
        JVM main.cpp

        include/ClassLoader/ClassLoader.h
        include/ClassLoader/ClassFileStructs.h
        include/ClassLoader/ClassFileEnums.h
        include/ClassLoader/AttributeStructs.h
        include/ClassLoader/ClassLoaderErrors.h
        include/typedefs.h
        include/ClassPrinter/ClassPrinter.h
        include/ClassPrinter/ConstantPoolPrinter.h
        include/ClassPrinter/OpcodePrinter.h
        include/Jvm/JvmStructs.h
        include/Jvm/Jvm.h

        include/ClassPrinter/OpcodePrinter.h
        include/Jvm/JvmEnums.h
        include/Jvm/RunTimeExceptions.h
        include/utils.h
  
        include/ClassPrinter/ConstantPoolPrinter.h

        src/ClassPrinter/ConstantPoolPrinter.cpp
        src/ClassLoader/ClassLoader.cpp
        src/ClassLoader/ClassLoaderErrors.cpp
        src/ClassPrinter/ClassPrinter.cpp
        src/ClassPrinter/ConstantPoolPrinter.cpp
        src/ClassPrinter/OpcodePrinter.cpp
        src/Jvm/Jvm.cpp
        src/Jvm/RunTimeExceptions.cpp
        src/utils.cpp

)
