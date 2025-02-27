//
// Created by matheus on 4/28/24.
//

#ifndef JVM_JVMENUMS_H
#define JVM_JVMENUMS_H

/**
 * Possíveis entradas de um Array
 */
enum class ArrayTypeCode{
    T_BOOLEAN = 4,
    T_CHAR    = 5,
    T_FLOAT   = 6,
    T_DOUBLE  = 7,
    T_BYTE    = 8,
    T_SHORT   = 9,
    T_INT     = 10,
    T_LONG    = 11,
    T_REF     = 12,
    T_ARRAY   = 13,
};

/**
 * Possíveis entradas em um Referece type
 */
enum class ReferenceType{
    NullReference,
    ClassType,
    ArrayType,
    InterfaceType
};


#endif //JVM_JVMENUMS_H
