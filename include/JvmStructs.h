//
// Created by matheus on 4/11/24.
//

#ifndef JVM_JVMSTRUCTS_H
#define JVM_JVMSTRUCTS_H
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

        template<typename RefType>
        RefType* PopRef(){
            //ponteiro ocupa 2 espacos, (so e pra funcionar na pilha de u4 do frame)
            if (this->size() < 2 || !std::is_same<T, u4>::value){
                throw std::out_of_range("Pilha ta vazia meu rei\n");
            }

            T LowBytes  = this->Pop();
            T HighBytes = this->Pop();

            uintptr_t ArrayRef = (static_cast<uintptr_t>(HighBytes) << 32) | LowBytes;

            return reinterpret_cast<RefType*>(ArrayRef);
        }

        void PushRef(void* ArrayRef){
            u4 LowBytes  = reinterpret_cast<uintptr_t>(ArrayRef) & 0xFFFFFFFF;
            u4 HighBytes = reinterpret_cast<uintptr_t>(ArrayRef) >> 32;
            this->push(HighBytes);
            this->push(LowBytes);

        }

    };


    template<typename T>
    class vector : public std::vector<T>{
    public:
        template<typename RefType>
        RefType* GetRefAt(u2 index){
            if(index + 1 >=this->size())
                throw std::runtime_error("SegFault no vetor de varivaies\n");
            T LowBytes  = (*this)[index];
            T HighBytes = (*this)[index + 1];

            uintptr_t ArrayRef = (static_cast<uintptr_t>(HighBytes) << 32) | LowBytes;

            return reinterpret_cast<RefType*>(ArrayRef);
        }
        void StoreRefAt(void* ArrayRef, u2 index){
            if(index + 1 >=this->size())
                throw std::runtime_error("SegFault no vetor de varivaies\n");

            u4 LowBytes  = reinterpret_cast<uintptr_t>(ArrayRef) & 0xFFFFFFFF;
            u4 HighBytes = reinterpret_cast<uintptr_t>(ArrayRef) >> 32;
            (*this)[index]   = HighBytes;
            (*this)[index+1] = LowBytes;

        }
    };
}


struct Frame{
    JVM::vector<u4>* localVariables;
    JVM::stack<u4>*  OperandStack;
    method_info*     frameMethod;
    class_file*      frameClass;
    u4 	             nextPC;
};



struct Handle{
    std::vector<method_info*>* MethodTable;
    class_file* ClassObject;
};

union FieldEntry{
    u1 AsByte;
    u1 AsBoolean;

    u2 AsChar;

    u4 AsInt;
    float AsFloat;

    u8 AsLong;
    double AsDouble;

    uintptr_t AsRef;

};


struct ClassInstance{
    Handle*         HandlePointer;
    std::unordered_map<std::string, FieldEntry> ObjectData;
};


union Cat2Value{
  struct{
	u4 HighBytes;
	u4 LowBytes;
  };
  long long AsLong;
  double AsDouble;
};

#endif //JVM_JVMSTRUCTS_H
