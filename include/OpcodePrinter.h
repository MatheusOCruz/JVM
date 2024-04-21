//
// Created by matheus on 4/20/24.
//

#ifndef JVM_OPCODEPRINTER_H
#define JVM_OPCODEPRINTER_H

#include <string>
#include "typedefs.h"
#include <iostream>
class OpcodePrinter {
public:
    OpcodePrinter() = default;

    std::string CodeToString(u1 code[],const u4 code_lenght);
private:

    // Funções vazias para os bytecodes
    void nop();
    void aconst_null();
    void iconst_m1();
    void iconst_0();
    void iconst_1();
    void iconst_2();
    void iconst_3();
    void iconst_4();
    void iconst_5();
    void lconst_0();
    void lconst_1();
    void fconst_0();
    void fconst_1();
    void fconst_2();
    void dconst_0();
    void dconst_1();
    void bipush();
    void sipush();
    void ldc();
    void ldc_w();
    void ldc2_w();
    void iload();
    void lload();
    void fload();
    void dload();
    void aload();
    void iload_0();
    void iload_1();
    void iload_2();
    void iload_3();
    void lload_0();
    void lload_1();
    void lload_2();
    void lload_3();
    void fload_0();
    void fload_1();
    void fload_2();
    void fload_3();
    void dload_0();
    void dload_1();
    void dload_2();
    void dload_3();
    void aload_0();
    void aload_1();
    void aload_2();
    void aload_3();
    void iaload();
    void laload();
    void faload();
    void daload();
    void aaload();
    void baload();
    void caload();
    void saload();
    void istore();
    void lstore();
    void fstore();
    void dstore();
    void astore();
    void istore_0();
    void istore_1();
    void istore_2();
    void istore_3();
    void lstore_0();
    void lstore_1();
    void lstore_2();
    void lstore_3();
    void fstore_0();
    void fstore_1();
    void fstore_2();
    void fstore_3();
    void dstore_0();
    void dstore_1();
    void dstore_2();
    void dstore_3();
    void astore_0();
    void astore_1();
    void astore_2();
    void astore_3();
    void iastore();
    void lastore();
    void fastore();
    void dastore();
    void aastore();
    void bastore();
    void castore();
    void sastore();
    void pop();
    void pop2();
    void dup();
    void dup_x1();
    void dup_x2();
    void dup2();
    void dup2_x1();
    void dup2_x2();
    void swap();
    void iadd();
    void ladd();
    void fadd();
    void dadd();
    void isub();
    void lsub();
    void fsub();
    void dsub();
    void imul();
    void lmul();
    void fmul();
    void dmul();
    void idiv();
    void ldiv();
    void fdiv();
    void ddiv();
    void irem();


    using BytecodeFunc = void (OpcodePrinter::*)();

// Vetor de ponteiros para as funções
    std::vector<BytecodeFunc> bytecodeFuncs = {
            &OpcodePrinter::nop, &OpcodePrinter::aconst_null,   &OpcodePrinter::iconst_m1,
            &OpcodePrinter::iconst_0, &OpcodePrinter::iconst_1, &OpcodePrinter::iconst_2,
            &OpcodePrinter::iconst_3, &OpcodePrinter::iconst_4, &OpcodePrinter::iconst_5,
            &OpcodePrinter::lconst_0, &OpcodePrinter::lconst_1, &OpcodePrinter::fconst_0,
            &OpcodePrinter::fconst_1, &OpcodePrinter::fconst_2, &OpcodePrinter::dconst_0,
            &OpcodePrinter::dconst_1, &OpcodePrinter::bipush,   &OpcodePrinter::sipush,
            &OpcodePrinter::ldc,      &OpcodePrinter::ldc_w,    &OpcodePrinter::ldc2_w,
            &OpcodePrinter::iload,    &OpcodePrinter::lload,    &OpcodePrinter::fload,
            &OpcodePrinter::dload,    &OpcodePrinter::aload,    &OpcodePrinter::iload_0,
            &OpcodePrinter::iload_1,  &OpcodePrinter::iload_2,  &OpcodePrinter::iload_3,
            &OpcodePrinter::lload_0,  &OpcodePrinter::lload_1,  &OpcodePrinter::lload_2,
            &OpcodePrinter::lload_3,  &OpcodePrinter::fload_0,  &OpcodePrinter::fload_1,
            &OpcodePrinter::fload_2,  &OpcodePrinter::fload_3,  &OpcodePrinter::dload_0,
            &OpcodePrinter::dload_1,  &OpcodePrinter::dload_2,  &OpcodePrinter::dload_3,
            &OpcodePrinter::aload_0,  &OpcodePrinter::aload_1,  &OpcodePrinter::aload_2,
            &OpcodePrinter::aload_3,  &OpcodePrinter::iaload,   &OpcodePrinter::laload,
            &OpcodePrinter::faload,   &OpcodePrinter::daload,   &OpcodePrinter::aaload,
            &OpcodePrinter::baload,   &OpcodePrinter::caload,   &OpcodePrinter::saload,
            &OpcodePrinter::istore,   &OpcodePrinter::lstore,   &OpcodePrinter::fstore,
            &OpcodePrinter::dstore,   &OpcodePrinter::astore,   &OpcodePrinter::istore_0,
            &OpcodePrinter::istore_1, &OpcodePrinter::istore_2, &OpcodePrinter::istore_3,
            &OpcodePrinter::lstore_0, &OpcodePrinter::lstore_1, &OpcodePrinter::lstore_2,
            &OpcodePrinter::lstore_3, &OpcodePrinter::fstore_0, &OpcodePrinter::fstore_1,
            &OpcodePrinter::fstore_2, &OpcodePrinter::fstore_3, &OpcodePrinter::dstore_0,
            &OpcodePrinter::dstore_1, &OpcodePrinter::dstore_2, &OpcodePrinter::dstore_3,
            &OpcodePrinter::astore_0, &OpcodePrinter::astore_1, &OpcodePrinter::astore_2,
            &OpcodePrinter::astore_3, &OpcodePrinter::iastore,  &OpcodePrinter::lastore,
            &OpcodePrinter::fastore,  &OpcodePrinter::dastore,  &OpcodePrinter::aastore,
            &OpcodePrinter::bastore,  &OpcodePrinter::castore,  &OpcodePrinter::sastore,
            &OpcodePrinter::pop,      &OpcodePrinter::pop2,     &OpcodePrinter::dup,
            &OpcodePrinter::dup_x1,   &OpcodePrinter::dup_x2,   &OpcodePrinter::dup2,
            &OpcodePrinter::dup2_x1,  &OpcodePrinter::dup2_x2,  &OpcodePrinter::swap,
            &OpcodePrinter::iadd,     &OpcodePrinter::ladd,     &OpcodePrinter::fadd,
            &OpcodePrinter::dadd,     &OpcodePrinter::isub,     &OpcodePrinter::lsub,
            &OpcodePrinter::fsub,     &OpcodePrinter::dsub,     &OpcodePrinter::imul,
            &OpcodePrinter::lmul,     &OpcodePrinter::fmul,     &OpcodePrinter::dmul,
            &OpcodePrinter::idiv,     &OpcodePrinter::ldiv,     &OpcodePrinter::fdiv,
            &OpcodePrinter::ddiv,     &OpcodePrinter::irem
    };

    std::string StringBuffer;
    u4 code_iterator;
    u1* code;
};


#endif //JVM_OPCODEPRINTER_H




