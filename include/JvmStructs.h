//
// Created by matheus on 4/11/24.
//

#ifndef JVM_JVMSTRUCTS_H
#define JVM_JVMSTRUCTS_H
#include "JvmEnums.h"
#include "typedefs.h"

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
    Handle*         HandlePointer;
    std::unordered_map<std::string, FieldEntry> ObjectData;
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



union Cat2Value{
    struct{
        u4 HighBytes;
        u4 LowBytes;
    };
    uint64_t  AsLong;
    double    AsDouble;

    float     AsFloat;
    s2    AsShort;

};

union IntToFloat{
    u4    Bytes;
    float AsFloat;
};




#endif //JVM_JVMSTRUCTS_H
