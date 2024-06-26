//
// Created by matheus on 4/11/24.
//

#ifndef JVM_JVMSTRUCTS_H
#define JVM_JVMSTRUCTS_H
#include <stack>
#include "JvmEnums.h"
#include "../typedefs.h"
#include <stdexcept>

namespace JVM{

    template<typename T>
    class stack : public std::stack<T>{
    public:

        T Pop(){
            if(this->empty()){
                throw std::out_of_range("Pilha ta vazia meu rei\n");
            }
            T topValue = std::stack<T>::top();
            std::stack<T>::pop();
            return topValue;
        }

    };
 
}


struct Frame{
    std::vector<u4>* localVariables;
    JVM::stack<u4>*  OperandStack;
    method_info*     frameMethod;
    class_file*      frameClass;
    u4 	             nextPC;
};



// definicao pra resolver definicao circular
struct ClassInstance;
struct ArrayInstance;
struct Reference;


union FieldEntry{
    u1     AsByte;
    u1     AsBoolean;
    u2     AsChar;
    u4     AsInt;
    float  AsFloat;
    u8     AsLong;
    double AsDouble;

    Reference* AsRef;

};


struct Handle{
    std::vector<method_info*>* MethodTable;
    class_file* ClassObject;
};

struct ClassInstance{
    Handle* ClassHandle;
    std::unordered_map<std::string, FieldEntry>* ObjectData;
};


struct ArrayInstance{
    ArrayTypeCode ComponentType;
    union {
        void**    ArrayVec;
        void*     DataVec;
    };
    int           size;
};





struct Reference{
    ReferenceType Type;
    union{
        void* Nullref = nullptr;
        ArrayInstance* ArrayRef;
        ClassInstance* ClassRef;
    };
};


/*
 *   pilha -> u4
 *   value.UBytes
 *   value.float
 *
 *
 *
 */




/**
 * @union U4ToType
 * @brief Union auxiliar para representar valores cat2 (32 bits)
 */
union Cat2Value{
    struct{
        u4 LowBytes;
        u4 HighBytes;
    };
	s4 AsInt;
    u8    Bytes;
    int64_t  AsLong;
    double    AsDouble;
};

/**
 * @union U4ToType
 * @brief Union auxiliar para converter e truncar valores de/para u4 (uint32_t)
 * 
 * @var 
 * effv
 * sfvfdv
 *  
 * @def Bytes lkmlkm
 * todo explain each member
 * 
 */
union U4ToType{
    u4    UBytes;
    float AsFloat;
    s4 AsInt;
    s2 AsShort;
    s1 AsByte;
    u2 AsChar;
};




#endif //JVM_JVMSTRUCTS_H
