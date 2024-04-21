//
// Created by matheus on 4/20/24.
//

#include "../include/OpcodePrinter.h"

std::string OpcodePrinter::CodeToString(u1 _code[], const u4 code_lenght) {
    StringBuffer = "";
    code_iterator = 0;
    code = _code;
    while(code_iterator != code_lenght){
        StringBuffer.append(std::to_string(code_iterator));
        u1 bytecode = code[code_iterator++];
        (this->*bytecodeFuncs[bytecode])();
    }
    return StringBuffer;
}



void OpcodePrinter::nop() {
    StringBuffer.append(" nop\n");
}
void OpcodePrinter::aconst_null() {}
void OpcodePrinter::iconst_m1() {}
void OpcodePrinter::iconst_0() {}
void OpcodePrinter::iconst_1() {}
void OpcodePrinter::iconst_2() {}
void OpcodePrinter::iconst_3() {}
void OpcodePrinter::iconst_4() {}
void OpcodePrinter::iconst_5() {}
void OpcodePrinter::lconst_0() {}
void OpcodePrinter::lconst_1() {}
void OpcodePrinter::fconst_0() {}
void OpcodePrinter::fconst_1() {}
void OpcodePrinter::fconst_2() {}
void OpcodePrinter::dconst_0() {}
void OpcodePrinter::dconst_1() {}
void OpcodePrinter::bipush() {
    u1 byte = code[code_iterator++];
    StringBuffer.append(" bipush ");
    StringBuffer.append(std::to_string(byte));
    StringBuffer.append("\n");
}
void OpcodePrinter::sipush() {}
void OpcodePrinter::ldc() {}
void OpcodePrinter::ldc_w() {}
void OpcodePrinter::ldc2_w() {}
void OpcodePrinter::iload() {}
void OpcodePrinter::lload() {}
void OpcodePrinter::fload() {}
void OpcodePrinter::dload() {}
void OpcodePrinter::aload() {}
void OpcodePrinter::iload_0() {}
void OpcodePrinter::iload_1() {}
void OpcodePrinter::iload_2() {}
void OpcodePrinter::iload_3() {}
void OpcodePrinter::lload_0() {}
void OpcodePrinter::lload_1() {}
void OpcodePrinter::lload_2() {}
void OpcodePrinter::lload_3() {}
void OpcodePrinter::fload_0() {}
void OpcodePrinter::fload_1() {}
void OpcodePrinter::fload_2() {}
void OpcodePrinter::fload_3() {}
void OpcodePrinter::dload_0() {}
void OpcodePrinter::dload_1() {}
void OpcodePrinter::dload_2() {}
void OpcodePrinter::dload_3() {}
void OpcodePrinter::aload_0() {}
void OpcodePrinter::aload_1() {}
void OpcodePrinter::aload_2() {}
void OpcodePrinter::aload_3() {}
void OpcodePrinter::iaload() {}
void OpcodePrinter::laload() {}
void OpcodePrinter::faload() {}
void OpcodePrinter::daload() {}
void OpcodePrinter::aaload() {}
void OpcodePrinter::baload() {}
void OpcodePrinter::caload() {}
void OpcodePrinter::saload() {}
void OpcodePrinter::istore() {}
void OpcodePrinter::lstore() {}
void OpcodePrinter::fstore() {}
void OpcodePrinter::dstore() {}
void OpcodePrinter::astore() {}
void OpcodePrinter::istore_0() {}
void OpcodePrinter::istore_1() {}
void OpcodePrinter::istore_2() {}
void OpcodePrinter::istore_3() {}
void OpcodePrinter::lstore_0() {}
void OpcodePrinter::lstore_1() {}
void OpcodePrinter::lstore_2() {}
void OpcodePrinter::lstore_3() {}
void OpcodePrinter::fstore_0() {}
void OpcodePrinter::fstore_1() {}
void OpcodePrinter::fstore_2() {}
void OpcodePrinter::fstore_3() {}
void OpcodePrinter::dstore_0() {}
void OpcodePrinter::dstore_1() {}
void OpcodePrinter::dstore_2() {}
void OpcodePrinter::dstore_3() {}
void OpcodePrinter::astore_0() {}
void OpcodePrinter::astore_1() {}
void OpcodePrinter::astore_2() {}
void OpcodePrinter::astore_3() {}
void OpcodePrinter::iastore() {}
void OpcodePrinter::lastore() {}
void OpcodePrinter::fastore() {}
void OpcodePrinter::dastore() {}
void OpcodePrinter::aastore() {}
void OpcodePrinter::bastore() {}
void OpcodePrinter::castore() {}
void OpcodePrinter::sastore() {}
void OpcodePrinter::pop() {}
void OpcodePrinter::pop2() {}
void OpcodePrinter::dup() {}
void OpcodePrinter::dup_x1() {}
void OpcodePrinter::dup_x2() {}
void OpcodePrinter::dup2() {}
void OpcodePrinter::dup2_x1() {}
void OpcodePrinter::dup2_x2() {}
void OpcodePrinter::swap() {}
void OpcodePrinter::iadd() {}
void OpcodePrinter::ladd() {}
void OpcodePrinter::fadd() {}
void OpcodePrinter::dadd() {}
void OpcodePrinter::isub() {}
void OpcodePrinter::lsub() {}
void OpcodePrinter::fsub() {}
void OpcodePrinter::dsub() {}
void OpcodePrinter::imul() {}
void OpcodePrinter::lmul() {}
void OpcodePrinter::fmul() {}
void OpcodePrinter::dmul() {}
void OpcodePrinter::idiv() {}
void OpcodePrinter::ldiv() {}
void OpcodePrinter::fdiv() {}
void OpcodePrinter::ddiv() {}
void OpcodePrinter::irem() {}

