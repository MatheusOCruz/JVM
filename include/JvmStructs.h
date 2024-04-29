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

    };
}


struct Frame{
    std::vector<u4>* localVariables;
    JVM::stack<u4>*  operandStack;
    method_info*     frameMethod;
    class_file*      frameClass;
    int32_t 	     nextPC; 	
};



struct Handle{
    std::vector<method_info*>* MethodTable;
    class_file* ClassObject;
};

struct ClassInstance{
    Handle*         HandlePointer;
   // std::unordered_map<std::string, > ObjectData;
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
