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
    StringBuffer.append(" aconst_null\n");
}
void OpcodePrinter::aconst_null() {
    StringBuffer.append(" aconst_null\n");
}
void OpcodePrinter::iconst_m1() {
    StringBuffer.append(" iconst_m1\n");
}
void OpcodePrinter::iconst_0() {
    StringBuffer.append(" iconst_0\n");
}
void OpcodePrinter::iconst_1() {
    StringBuffer.append(" iconst_1\n");
}
void OpcodePrinter::iconst_2() {
    StringBuffer.append(" iconst_2\n");
}
void OpcodePrinter::iconst_3() {
    StringBuffer.append(" iconst_3\n");
}
void OpcodePrinter::iconst_4() {
    StringBuffer.append(" iconst_4\n");
}
void OpcodePrinter::iconst_5() {
    StringBuffer.append(" iconst_5\n");
}
void OpcodePrinter::lconst_0() {
    StringBuffer.append(" lconst_0\n");
}
void OpcodePrinter::lconst_1() {
    StringBuffer.append(" lconst_1\n");
}
void OpcodePrinter::fconst_0() {
    StringBuffer.append(" fconst_0\n");
}
void OpcodePrinter::fconst_1() {
    StringBuffer.append(" fconst_1\n");
}
void OpcodePrinter::fconst_2() {
    StringBuffer.append(" fconst_2\n");
}
void OpcodePrinter::dconst_0() {
    StringBuffer.append(" dconst_0\n");
}
void OpcodePrinter::dconst_1() {
    StringBuffer.append(" dconst_1\n");
}

void OpcodePrinter::bipush() {
    u1 byte = code[code_iterator++];
    StringBuffer.append(" bipush ");
    StringBuffer.append(std::to_string(byte));
    StringBuffer.append("\n");
}

void OpcodePrinter::sipush() {
    u2 short_ = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" sipush ");
    StringBuffer.append(std::to_string(short_));
    StringBuffer.append("\n");
}
void OpcodePrinter::ldc() {
    u1 index = code[code_iterator++];
    StringBuffer.append(" ldc #");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
//TODO: incluir constant_pool como argumento da classe pra pegar valor no indice
void OpcodePrinter::ldc_w() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" ldc_w #");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::ldc2_w() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" ldc2_w #");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::iload() {
    u1 index = code[code_iterator++];
    StringBuffer.append(" iload ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::lload() {
    u1 index = code[code_iterator++];
    StringBuffer.append(" lload ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::fload() {
    u1 index = code[code_iterator++];
    StringBuffer.append(" fload ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::dload() {
    u1 index = code[code_iterator++];
    StringBuffer.append(" dload ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::aload() {
    u1 index = code[code_iterator++];
    StringBuffer.append(" aload ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::iload_0() {
    StringBuffer.append(" iload_0\n");
}
void OpcodePrinter::iload_1() {
    StringBuffer.append(" iload_1\n");
}
void OpcodePrinter::iload_2() {
    StringBuffer.append(" iload_2\n");
}
void OpcodePrinter::iload_3() {
    StringBuffer.append(" iload_3\n");
}
void OpcodePrinter::lload_0() {
    StringBuffer.append(" lload_0\n");
}
void OpcodePrinter::lload_1() {
    StringBuffer.append(" lload_1n");
}
void OpcodePrinter::lload_2() {
    StringBuffer.append(" lload_2\n");
}
void OpcodePrinter::lload_3() {
    StringBuffer.append(" lload_3\n");
}
void OpcodePrinter::fload_0() {
    StringBuffer.append(" fload_0\n");
}
void OpcodePrinter::fload_1() {
    StringBuffer.append(" fload_1\n");
}
void OpcodePrinter::fload_2() {
    StringBuffer.append(" fload_2\n");
}
void OpcodePrinter::fload_3() {
    StringBuffer.append(" fload_3\n");
}
void OpcodePrinter::dload_0() {
    StringBuffer.append(" dload_0\n");
}
void OpcodePrinter::dload_1() {
    StringBuffer.append(" dload_1\n");
}
void OpcodePrinter::dload_2() {
    StringBuffer.append(" dload_2\n");
}

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
void OpcodePrinter::lrem() {}
void OpcodePrinter::frem() {}
void OpcodePrinter::drem() {}
void OpcodePrinter::ineg() {}
void OpcodePrinter::lneg() {}
void OpcodePrinter::fneg() {}
void OpcodePrinter::dneg() {}
void OpcodePrinter::ishl() {}
void OpcodePrinter::lshl() {}
void OpcodePrinter::ishr() {}
void OpcodePrinter::lshr() {}
void OpcodePrinter::iushr() {}
void OpcodePrinter::lushr() {}
void OpcodePrinter::iand() {}
void OpcodePrinter::land() {}
void OpcodePrinter::ior() {}
void OpcodePrinter::lor() {}
void OpcodePrinter::ixor() {}
void OpcodePrinter::lxor() {}
void OpcodePrinter::iinc() {}
void OpcodePrinter::i2l() {}
void OpcodePrinter::i2f() {}
void OpcodePrinter::i2d() {}
void OpcodePrinter::l2i() {}
void OpcodePrinter::l2f() {}
void OpcodePrinter::l2d() {}
void OpcodePrinter::f2i() {}
void OpcodePrinter::f2l() {}
void OpcodePrinter::f2d() {}
void OpcodePrinter::d2i() {}
void OpcodePrinter::d2l() {}
void OpcodePrinter::d2f() {}
void OpcodePrinter::i2b() {}
void OpcodePrinter::i2c() {}
void OpcodePrinter::i2s() {}
void OpcodePrinter::lcmp() {}
void OpcodePrinter::fcmpl() {}
void OpcodePrinter::fcmpg() {}
void OpcodePrinter::dcmpl() {}
void OpcodePrinter::dcmpg() {}
void OpcodePrinter::ifeq() {}
void OpcodePrinter::ifne() {}
void OpcodePrinter::iflt() {}
void OpcodePrinter::ifge() {}
void OpcodePrinter::ifgt() {}
void OpcodePrinter::ifle() {}
void OpcodePrinter::if_icmpeq() {}
void OpcodePrinter::if_icmpne() {}
void OpcodePrinter::if_icmplt() {}
void OpcodePrinter::if_icmpge() {}
void OpcodePrinter::if_icmpgt() {}
void OpcodePrinter::if_icmple() {}
void OpcodePrinter::if_acmpeq() {}
void OpcodePrinter::if_acmpne() {}
void OpcodePrinter::goto_() {} // Usando goto_ para evitar conflito com a palavra reservada goto
void OpcodePrinter::jsr_() {} // Usando jsr_ para evitar conflito com a palavra reservada jsr
void OpcodePrinter::ret_() {} // Usando ret_ para evitar conflito com a palavra reservada ret
void OpcodePrinter::tableswitch() {}
void OpcodePrinter::lookupswitch() {}
void OpcodePrinter::ireturn() {}
void OpcodePrinter::lreturn() {}
void OpcodePrinter::freturn() {}
void OpcodePrinter::dreturn() {}
void OpcodePrinter::areturn() {}
void OpcodePrinter::return_() {}
void OpcodePrinter::getstatic() {}
void OpcodePrinter::putstatic() {}
void OpcodePrinter::getfield() {}
void OpcodePrinter::putfield() {}
void OpcodePrinter::invokevirtual() {}
void OpcodePrinter::invokespecial() {}
void OpcodePrinter::invokestatic() {}
void OpcodePrinter::invokeinterface() {}
void OpcodePrinter::invokedynamic() {}
void OpcodePrinter::new_() {}
void OpcodePrinter::newarray() {}
void OpcodePrinter::anewarray() {}
void OpcodePrinter::arraylength() {}
void OpcodePrinter::athrow() {}
void OpcodePrinter::checkcast() {}
void OpcodePrinter::instanceof() {}
void OpcodePrinter::monitorenter() {}
void OpcodePrinter::monitorexit() {}
void OpcodePrinter::wide() {}
void OpcodePrinter::multianewarray() {}
void OpcodePrinter::ifnull() {}
void OpcodePrinter::ifnonnull() {}
void OpcodePrinter::goto_w() {}
void OpcodePrinter::jsr_w() {}

