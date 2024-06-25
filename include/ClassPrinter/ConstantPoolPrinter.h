//
// Created by matheus on 6/25/24.
//

#ifndef JVM_CONSTANTPOOLPRINTER_H
#define JVM_CONSTANTPOOLPRINTER_H
#include <string>
#include "../typedefs.h"
#include "../ClassLoader/ClassFileStructs.h"

class ConstantPoolPrinter {

public:
    explicit ConstantPoolPrinter(std::vector<cp_info*>* constantPool) : ConstantPool(*constantPool) {};
    std::string CpEntryAsString(unsigned short i);

private:
    std::vector<cp_info*> ConstantPool;
};


#endif //JVM_CONSTANTPOOLPRINTER_H
