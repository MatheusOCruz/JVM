//
// Created by matheus on 4/11/24.
//

#ifndef JVM_JVM_H
#define JVM_JVM_H

#include <stack>
#include <utility>

#include "../ClassLoader/ClassLoader.h"
#include "JvmStructs.h"
#include "JvmEnums.h"
#include "RunTimeExceptions.h"
#include "../ClassPrinter/OpcodePrinter.h"

class Jvm {
public:
     explicit Jvm(std::string _main_file) : MainClass(std::move(_main_file)) {}
     ~Jvm() = default;

     void     Run();

	static int numberOfEntriesFromString(const std::string & Descriptor);

    // tableswich
    uint32_t read_u4(const std::vector<uint8_t>& code, size_t& index);

private:
    // tbleswitch
    std::vector<u1> code;
    void printInstruction(const std::string& instr);
    //

    int readInt(size_t& index);

    //funcoes da jvm
    void SaveFrameState();
    void NewFrame();
    void PopFrameStack();

    int  GetMethod(const std::string& MethodName);
    void GetCurrentMethodCode();

    u1   NextCodeByte();
    void ExecBytecode();
    u4   PopOpStack();


    class_file* GetClass(std::string class_name);
    cp_info*    GetConstantPoolEntryAt(u2 index);
    void        CheckStaticInit(std::string class_name);

    //vars jvm
    std::string MainClass;
     //int pc = 0;
     //size_t &pc = 0;
    size_t pc = 0;


    JVM::stack<Frame*>                            FrameStack;
    std::unordered_map<std::string, class_file*>* MethodArea;


    Frame*          CurrentFrame;
    class_file*     CurrentClass;
    method_info*    CurrentMethod;
    attribute_info* CurrentCode;

    ClassLoader*    Loader;


    void JavaPrint(std::string& MethodDescriptor);

    //funcoes auxiliares pro bytecode
    void return_u4();
    void return_u8();

    u2 GetIndex2();
    
    u8 popU8FromOpStack(); //pop 2 u4 em big endian e retorna um u8
    void pushU8ToOpStack(u4 HighBytes, u4 LowBytes); //push um u8 em big endian
    
    u8 getU8FromLocalVars(u4 startingIndex); //pega um u8 das variaveis locais em big endian

    void invoke(std::string ClassName,
                std::string MethodName,
                std::string Descriptor,
                bool isStatic = false);
    void LoadLocalVariables(std::string &Descriptor, JVM::stack<u4> *CallerOperandStack, bool isStatic = false);

    void fconst(float value);
    void dconst(double value);

    //funcoes do bytecode

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
    void dload(uint index);
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
    void jsr();
    void ret();

    void tableswitch();

    uint32_t read_u4();

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


    using BytecodeFunc = void (Jvm::*)();

// Vetor de ponteiros para as funções
    std::vector<BytecodeFunc> bytecodeFuncs = {
            &Jvm::nop,           &Jvm::aconst_null,  &Jvm::iconst_m1,
            &Jvm::iconst_0,      &Jvm::iconst_1,     &Jvm::iconst_2,
            &Jvm::iconst_3,      &Jvm::iconst_4,     &Jvm::iconst_5,
            &Jvm::lconst_0,      &Jvm::lconst_1,     &Jvm::fconst_0,
            &Jvm::fconst_1,      &Jvm::fconst_2,     &Jvm::dconst_0,
            &Jvm::dconst_1,      &Jvm::bipush,       &Jvm::sipush,
            &Jvm::ldc,           &Jvm::ldc_w,        &Jvm::ldc2_w,
            &Jvm::iload,         &Jvm::lload,        &Jvm::fload,
            &Jvm::dload,         &Jvm::aload,        &Jvm::iload_0,
            &Jvm::iload_1,       &Jvm::iload_2,      &Jvm::iload_3,
            &Jvm::lload_0,       &Jvm::lload_1,      &Jvm::lload_2,
            &Jvm::lload_3,       &Jvm::fload_0,      &Jvm::fload_1,
            &Jvm::fload_2,       &Jvm::fload_3,      &Jvm::dload_0,
            &Jvm::dload_1,       &Jvm::dload_2,      &Jvm::dload_3,
            &Jvm::aload_0,       &Jvm::aload_1,      &Jvm::aload_2,
            &Jvm::aload_3,       &Jvm::iaload,       &Jvm::laload,
            &Jvm::faload,        &Jvm::daload,       &Jvm::aaload,
            &Jvm::baload,        &Jvm::caload,       &Jvm::saload,
            &Jvm::istore,        &Jvm::lstore,       &Jvm::fstore,
            &Jvm::dstore,        &Jvm::astore,       &Jvm::istore_0,
            &Jvm::istore_1,      &Jvm::istore_2,     &Jvm::istore_3,
            &Jvm::lstore_0,      &Jvm::lstore_1,     &Jvm::lstore_2,
            &Jvm::lstore_3,      &Jvm::fstore_0,     &Jvm::fstore_1,
            &Jvm::fstore_2,      &Jvm::fstore_3,     &Jvm::dstore_0,
            &Jvm::dstore_1,      &Jvm::dstore_2,     &Jvm::dstore_3,
            &Jvm::astore_0,      &Jvm::astore_1,     &Jvm::astore_2,
            &Jvm::astore_3,      &Jvm::iastore,      &Jvm::lastore,
            &Jvm::fastore,       &Jvm::dastore,      &Jvm::aastore,
            &Jvm::bastore,       &Jvm::castore,      &Jvm::sastore,
            &Jvm::pop,           &Jvm::pop2,         &Jvm::dup,
            &Jvm::dup_x1,        &Jvm::dup_x2,       &Jvm::dup2,
            &Jvm::dup2_x1,       &Jvm::dup2_x2,      &Jvm::swap,
            &Jvm::iadd,          &Jvm::ladd,         &Jvm::fadd,
            &Jvm::dadd,          &Jvm::isub,         &Jvm::lsub,
            &Jvm::fsub,          &Jvm::dsub,         &Jvm::imul,
            &Jvm::lmul,          &Jvm::fmul,         &Jvm::dmul,
            &Jvm::idiv,          &Jvm::ldiv,         &Jvm::fdiv,
            &Jvm::ddiv,          &Jvm::irem,         &Jvm::lrem,
            &Jvm::frem,          &Jvm::drem,         &Jvm::ineg,
            &Jvm::lneg,          &Jvm::fneg,         &Jvm::dneg,
            &Jvm::ishl,          &Jvm::lshl,         &Jvm::ishr,
            &Jvm::lshr,          &Jvm::iushr,        &Jvm::lushr,
            &Jvm::iand,          &Jvm::land,         &Jvm::ior,
            &Jvm::lor,           &Jvm::ixor,         &Jvm::lxor,
            &Jvm::iinc,          &Jvm::i2l,          &Jvm::i2f,
            &Jvm::i2d,           &Jvm::l2i,          &Jvm::l2f,
            &Jvm::l2d,           &Jvm::f2i,          &Jvm::f2l,
            &Jvm::f2d,           &Jvm::d2i,          &Jvm::d2l,
            &Jvm::d2f,           &Jvm::i2b,          &Jvm::i2c,
            &Jvm::i2s,           &Jvm::lcmp,         &Jvm::fcmpl,
            &Jvm::fcmpg,         &Jvm::dcmpl,        &Jvm::dcmpg,
            &Jvm::ifeq,          &Jvm::ifne,         &Jvm::iflt,
            &Jvm::ifge,          &Jvm::ifgt,         &Jvm::ifle,
            &Jvm::if_icmpeq,     &Jvm::if_icmpne,    &Jvm::if_icmplt,
            &Jvm::if_icmpge,     &Jvm::if_icmpgt,    &Jvm::if_icmple,
            &Jvm::if_acmpeq,     &Jvm::if_acmpne,    &Jvm::goto_,
            &Jvm::jsr,           &Jvm::ret,          &Jvm::tableswitch,
            &Jvm::lookupswitch,  &Jvm::ireturn,      &Jvm::lreturn,
            &Jvm::freturn,       &Jvm::dreturn,      &Jvm::areturn,
            &Jvm::return_,       &Jvm::getstatic,    &Jvm::putstatic,
            &Jvm::getfield,      &Jvm::putfield,     &Jvm::invokevirtual,
            &Jvm::invokespecial, &Jvm::invokestatic, &Jvm::invokeinterface,
            &Jvm::invokedynamic, &Jvm::new_,         &Jvm::newarray,
            &Jvm::anewarray,     &Jvm::arraylength,  &Jvm::athrow,
            &Jvm::checkcast,     &Jvm::instanceof,   &Jvm::monitorenter,
            &Jvm::monitorexit,   &Jvm::wide,         &Jvm::multianewarray,
            &Jvm::ifnull,        &Jvm::ifnonnull,    &Jvm::goto_w,
            &Jvm::jsr_w
        
    };


    int IsLoaded(std::string class_name);
};




#endif //JVM_JVM_H
