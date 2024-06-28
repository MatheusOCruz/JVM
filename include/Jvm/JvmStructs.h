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
    /**
     * @brief Versão custom da std::stack
     * @tparam T tipo da pilha
     */
    template<typename T>
    class stack : public std::stack<T>{
    public:
        /**
         * @brief retorna o elemento no topo da pilha e o desempilha
         * @return Elemento no topo da pilha
         */
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

/**
 * @brief Representa o Frame de execução da jvm
 */
struct Frame{
    std::vector<u4>* localVariables;
    JVM::stack<u4>*  OperandStack;
    method_info*     frameMethod;
    class_file*      frameClass;
    u4 	             nextPC;
};



struct ClassInstance;
struct ArrayInstance;
struct Reference;

/**
 * @brief representa as possiveis entradas de um field de uma classe
 */
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

/**
 * @brief Responsável pela representação interna de uma instância de classe
 * possui um ponteiro para um Handle e um mapa que armazena seus fields
 */
struct ClassInstance{
    Handle* ClassHandle;
    std::unordered_map<std::string, FieldEntry>* ObjectData;
};

/**
 * @brief Responsável pela representação interna de uma instância de Array
 *  usa uma union entre um
 *  void** que armazena outros arrays, caso seja n dimensional
 *  e void* que armazena size entrads de tipo ComponenteType
 */
struct ArrayInstance{
    ArrayTypeCode ComponentType;
    union {
        void**    ArrayVec;
        void*     DataVec;
    };
    int           size;
};




/**
 * Struct reponsável por armazenar referências na pilha de operandos,
 * que devem ser desemcapsulados após a verificação de tipo
 */
struct Reference{
    ReferenceType Type;
    union{
        void* Nullref = nullptr;
        ArrayInstance* ArrayRef;
        ClassInstance* ClassRef;
    };
};




/**
 * @brief Union auxiliar para representar valores cat2 (64 bits)
 *
 */
union Cat2Value{
    struct{
        u4 LowBytes;
        u4 HighBytes;
    };
    u8 Bytes
    int64_t  AsLong;
    double    AsDouble;
};

/**
 * @brief Union auxiliar para converter e truncar valores de/para u4 (uint32_t)
 * 
 * @var 
 * effv
 * sfvfdv
 *  
 * @def Bytes lkmlkm
 *
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
