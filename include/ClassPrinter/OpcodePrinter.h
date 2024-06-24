//
// Created by matheus on 4/20/24.
//

#ifndef JVM_OPCODEPRINTER_H
#define JVM_OPCODEPRINTER_H

#include <string>
#include "../typedefs.h"
#include "../ClassLoader/ClassFileEnums.h"
#include <iostream>
class OpcodePrinter {
public:
    OpcodePrinter() = default;

    std::string CodeToString(u1 code[],const u4 code_lenght);
    // Declaração do metodo set code <tableswitch>
    void setCode(u1* code, size_t code_size); // Novo metodo para configurar o código e o tamanho
    u4 readU4();

    void printInstruction(const std::string &instr);

private:
    size_t code_size; // Adicione isso
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
    void lrem();
    void frem();
    void drem();
    void ineg();
    void lneg();
    void fneg();
    void dneg();
    void ishl();
    void lshl();
    void ishr();
    void lshr();
    void iushr();
    void lushr();
    void iand();
    void land();
    void ior();
    void lor();
    void ixor();
    void lxor();
    void iinc();
    void i2l();
    void i2f();
    void i2d();
    void l2i();
    void l2f();
    void l2d();
    void f2i();
    void f2l();
    void f2d();
    void d2i();
    void d2l();
    void d2f();
    void i2b();
    void i2c();
    void i2s();
    void lcmp();
    void fcmpl();
    void fcmpg();
    void dcmpl();
    void dcmpg();
    void ifeq();
    void ifne();
    void iflt();
    void ifge();
    void ifgt();
    void ifle();
    void if_icmpeq();
    void if_icmpne();
    void if_icmplt();
    void if_icmpge();
    void if_icmpgt();
    void if_icmple();
    void if_acmpeq();
    void if_acmpne();
    void goto_();
    void jsr_();
    void ret_();
    void tableswitch();
    void lookupswitch();
    void ireturn();
    void lreturn();
    void freturn();
    void dreturn();
    void areturn();
    void return_();
    void getstatic();
    void putstatic();
    void getfield();
    void putfield();
    void invokevirtual();
    void invokespecial();
    void invokestatic();
    void invokeinterface();
    void invokedynamic();
    void new_();
    void newarray();
    void anewarray();
    void arraylength();
    void athrow();
    void checkcast();
    void instanceof();
    void monitorenter();
    void monitorexit();
    void wide();
    void multianewarray();
    void ifnull();
    void ifnonnull();
    void goto_w();
    void jsr_w();


    using BytecodeFunc = void (OpcodePrinter::*)();

// Vetor de ponteiros para as funções
    std::vector<BytecodeFunc> bytecodeFuncs = {
            &OpcodePrinter::nop,        &OpcodePrinter::aconst_null,   &OpcodePrinter::iconst_m1,
            &OpcodePrinter::iconst_0,   &OpcodePrinter::iconst_1,      &OpcodePrinter::iconst_2,
            &OpcodePrinter::iconst_3,   &OpcodePrinter::iconst_4,      &OpcodePrinter::iconst_5,
            &OpcodePrinter::lconst_0,   &OpcodePrinter::lconst_1,      &OpcodePrinter::fconst_0,
            &OpcodePrinter::fconst_1,   &OpcodePrinter::fconst_2,      &OpcodePrinter::dconst_0,
            &OpcodePrinter::dconst_1,   &OpcodePrinter::bipush,        &OpcodePrinter::sipush,
            &OpcodePrinter::ldc,        &OpcodePrinter::ldc_w,         &OpcodePrinter::ldc2_w,
            &OpcodePrinter::iload,      &OpcodePrinter::lload,         &OpcodePrinter::fload,
            &OpcodePrinter::dload,      &OpcodePrinter::aload,         &OpcodePrinter::iload_0,
            &OpcodePrinter::iload_1,    &OpcodePrinter::iload_2,       &OpcodePrinter::iload_3,
            &OpcodePrinter::lload_0,    &OpcodePrinter::lload_1,       &OpcodePrinter::lload_2,
            &OpcodePrinter::lload_3,    &OpcodePrinter::fload_0,       &OpcodePrinter::fload_1,
            &OpcodePrinter::fload_2,    &OpcodePrinter::fload_3,       &OpcodePrinter::dload_0,
            &OpcodePrinter::dload_1,    &OpcodePrinter::dload_2,       &OpcodePrinter::dload_3,
            &OpcodePrinter::aload_0,    &OpcodePrinter::aload_1,       &OpcodePrinter::aload_2,
            &OpcodePrinter::aload_3,    &OpcodePrinter::iaload,        &OpcodePrinter::laload,
            &OpcodePrinter::faload,     &OpcodePrinter::daload,        &OpcodePrinter::aaload,
            &OpcodePrinter::baload,     &OpcodePrinter::caload,        &OpcodePrinter::saload,
            &OpcodePrinter::istore,     &OpcodePrinter::lstore,        &OpcodePrinter::fstore,
            &OpcodePrinter::dstore,     &OpcodePrinter::astore,        &OpcodePrinter::istore_0,
            &OpcodePrinter::istore_1,   &OpcodePrinter::istore_2,      &OpcodePrinter::istore_3,
            &OpcodePrinter::lstore_0,   &OpcodePrinter::lstore_1,      &OpcodePrinter::lstore_2,
            &OpcodePrinter::lstore_3,   &OpcodePrinter::fstore_0,      &OpcodePrinter::fstore_1,
            &OpcodePrinter::fstore_2,   &OpcodePrinter::fstore_3,      &OpcodePrinter::dstore_0,
            &OpcodePrinter::dstore_1,   &OpcodePrinter::dstore_2,      &OpcodePrinter::dstore_3,
            &OpcodePrinter::astore_0,   &OpcodePrinter::astore_1,      &OpcodePrinter::astore_2,
            &OpcodePrinter::astore_3,   &OpcodePrinter::iastore,       &OpcodePrinter::lastore,
            &OpcodePrinter::fastore,    &OpcodePrinter::dastore,       &OpcodePrinter::aastore,
            &OpcodePrinter::bastore,    &OpcodePrinter::castore,       &OpcodePrinter::sastore,
            &OpcodePrinter::pop,        &OpcodePrinter::pop2,          &OpcodePrinter::dup,
            &OpcodePrinter::dup_x1,     &OpcodePrinter::dup_x2,        &OpcodePrinter::dup2,
            &OpcodePrinter::dup2_x1,    &OpcodePrinter::dup2_x2,       &OpcodePrinter::swap,
            &OpcodePrinter::iadd,       &OpcodePrinter::ladd,          &OpcodePrinter::fadd,
            &OpcodePrinter::dadd,       &OpcodePrinter::isub,          &OpcodePrinter::lsub,
            &OpcodePrinter::fsub,       &OpcodePrinter::dsub,          &OpcodePrinter::imul,
            &OpcodePrinter::lmul,       &OpcodePrinter::fmul,          &OpcodePrinter::dmul,
            &OpcodePrinter::idiv,       &OpcodePrinter::ldiv,          &OpcodePrinter::fdiv,
            &OpcodePrinter::ddiv,       &OpcodePrinter::irem,          &OpcodePrinter::lrem,
            &OpcodePrinter::frem,       &OpcodePrinter::drem,          &OpcodePrinter::ineg,
            &OpcodePrinter::lneg,       &OpcodePrinter::fneg,          &OpcodePrinter::dneg,
            &OpcodePrinter::ishl,       &OpcodePrinter::lshl,          &OpcodePrinter::ishr,
            &OpcodePrinter::lshr,       &OpcodePrinter::iushr,         &OpcodePrinter::lushr,
            &OpcodePrinter::iand,       &OpcodePrinter::land,          &OpcodePrinter::ior,
            &OpcodePrinter::lor,        &OpcodePrinter::ixor,          &OpcodePrinter::lxor,
            &OpcodePrinter::iinc,       &OpcodePrinter::i2l,           &OpcodePrinter::i2f,
            &OpcodePrinter::i2d,        &OpcodePrinter::l2i,           &OpcodePrinter::l2f,
            &OpcodePrinter::l2d,        &OpcodePrinter::f2i,           &OpcodePrinter::f2l,
            &OpcodePrinter::f2d,        &OpcodePrinter::d2i,           &OpcodePrinter::d2l,
            &OpcodePrinter::d2f,        &OpcodePrinter::i2b,           &OpcodePrinter::i2c,
            &OpcodePrinter::i2s,        &OpcodePrinter::lcmp,          &OpcodePrinter::fcmpl,
            &OpcodePrinter::fcmpg,      &OpcodePrinter::dcmpl,         &OpcodePrinter::dcmpg,
            &OpcodePrinter::ifeq,       &OpcodePrinter::ifne,          &OpcodePrinter::iflt,
            &OpcodePrinter::ifge,       &OpcodePrinter::ifgt,          &OpcodePrinter::ifle,
            &OpcodePrinter::if_icmpeq,  &OpcodePrinter::if_icmpne,     &OpcodePrinter::if_icmplt,
            &OpcodePrinter::if_icmpge,  &OpcodePrinter::if_icmpgt,     &OpcodePrinter::if_icmple,
            &OpcodePrinter::if_acmpeq,  &OpcodePrinter::if_acmpne,     &OpcodePrinter::goto_,
            &OpcodePrinter::jsr_,       &OpcodePrinter::ret_,          &OpcodePrinter::tableswitch,
            &OpcodePrinter::lookupswitch,&OpcodePrinter::ireturn,       &OpcodePrinter::lreturn,
            &OpcodePrinter::freturn,    &OpcodePrinter::dreturn,       &OpcodePrinter::areturn,
            &OpcodePrinter::return_,    &OpcodePrinter::getstatic,     &OpcodePrinter::putstatic,
            &OpcodePrinter::getfield,   &OpcodePrinter::putfield,      &OpcodePrinter::invokevirtual,
            &OpcodePrinter::invokespecial,&OpcodePrinter::invokestatic,&OpcodePrinter::invokeinterface,
            &OpcodePrinter::invokedynamic,&OpcodePrinter::new_,        &OpcodePrinter::newarray,
            &OpcodePrinter::anewarray,    &OpcodePrinter::arraylength,   &OpcodePrinter::athrow,
            &OpcodePrinter::checkcast,    &OpcodePrinter::instanceof,    &OpcodePrinter::monitorenter,
            &OpcodePrinter::monitorexit,  &OpcodePrinter::wide,          &OpcodePrinter::multianewarray,
            &OpcodePrinter::ifnull,       &OpcodePrinter::ifnonnull,     &OpcodePrinter::goto_w,
            &OpcodePrinter::jsr_w
    };

    std::string StringBuffer;
    u4 code_iterator;
    u1* code;
};



#endif //JVM_OPCODEPRINTER_H




