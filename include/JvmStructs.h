//
// Created by matheus on 4/11/24.
//

#ifndef JVM_JVMSTRUCTS_H
#define JVM_JVMSTRUCTS_H
#include "typedefs.h"

struct frame{
    std::vector<u4>* localVariables;
    std::stack<u4>*  operandStack;
};
/*
union Jlong{
    struct{
        u4 high_bytes;
        u4 low_bytes;
    };
    i64 value;
};
// num sei se so com isso ja vai funfar
union Jdouble{
    u1 bytes[8];
    double value;
};

*/
#endif //JVM_JVMSTRUCTS_H
