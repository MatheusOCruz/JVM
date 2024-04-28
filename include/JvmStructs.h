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
    method_info*     frame_method;
};

// resolvi fazer uma stack
// pq o stack do cpp n retorna valor no pop




#endif //JVM_JVMSTRUCTS_H
