//
// Created by matheus on 4/2/24.
//

#ifndef JVM_TYPEDEFS_H
#define JVM_TYPEDEFS_H
#include<cstdint>
#include<vector>
typedef uint8_t  u1;
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;

typedef uintptr_t u4p;

typedef int16_t s2;
typedef int32_t s4;
typedef int64_t s8; 




typedef std::vector<uint8_t>::iterator buffer_iterator;
#endif //JVM_TYPEDEFS_H
