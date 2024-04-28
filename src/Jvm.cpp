//
// Created by matheus on 4/11/24.
//

#include "../include/Jvm.h"

void Jvm::Run(){

    MethodArea = new std::map<char*,class_file*>;
    auto Loader = new ClassLoader(MethodArea);
    Loader->LoadMain(main_file);
    // so pras funcoes n reclamarem enquanto to definindo
    CurrentFrame = new Frame;
    CurrentFrame->localVariables = new std::vector<u4>;
    CurrentFrame->operandStack =  new std::stack<u4>;



}



void Jvm::InvokeMethod() {
    auto NewFrame = new Frame;
    NewFrame->localVariables = new std::vector<u4>;
    NewFrame->operandStack =  new std::stack<u4>;
    FrameStack.push(NewFrame);
    CurrentFrame = NewFrame;

}










// so bytecode


void Jvm::nop() {

}


void Jvm::aconst_null(){
  
}





void Jvm::iconst_m1(){

}




void Jvm::iconst_0(){

}




void Jvm::iconst_1(){

}




void Jvm::iconst_2(){

}




void Jvm::iconst_3(){

}




void Jvm::iconst_4(){

}




void Jvm::iconst_5(){

}




void Jvm::lconst_0(){

}




void Jvm::lconst_1(){

}




void Jvm::fconst_0(){

}




void Jvm::fconst_1(){

}




void Jvm::fconst_2(){

}




void Jvm::dconst_0(){

}




void Jvm::dconst_1(){

}




void Jvm::bipush(){

}




void Jvm::sipush(){

}




void Jvm::ldc(){

}




void Jvm::ldc_w(){

}




void Jvm::ldc2_w(){

}




void Jvm::iload(){

}




void Jvm::lload(){

}




void Jvm::fload(){

}




void Jvm::dload(){

}




void Jvm::aload(){

}




void Jvm::iload_0(){

}




void Jvm::iload_1(){

}




void Jvm::iload_2(){

}




void Jvm::iload_3(){

}




void Jvm::lload_0(){

}




void Jvm::lload_1(){

}




void Jvm::lload_2(){

}




void Jvm::lload_3(){

}




void Jvm::fload_0(){

}




void Jvm::fload_1(){

}




void Jvm::fload_2(){

}




void Jvm::fload_3(){

}




void Jvm::dload_0(){

}




void Jvm::dload_1(){

}




void Jvm::dload_2(){

}




void Jvm::dload_3(){

}




void Jvm::aload_0(){

}




void Jvm::aload_1(){

}




void Jvm::aload_2(){

}




void Jvm::aload_3(){

}




void Jvm::iaload(){

}




void Jvm::laload(){

}




void Jvm::faload(){

}




void Jvm::daload(){

}




void Jvm::aaload(){

}




void Jvm::baload(){

}




void Jvm::caload(){

}




void Jvm::saload(){

}




void Jvm::istore(){

}




void Jvm::lstore(){

}




void Jvm::fstore(){

}




void Jvm::dstore(){

}




void Jvm::astore(){

}




void Jvm::istore_0(){

}




void Jvm::istore_1(){

}




void Jvm::istore_2(){

}




void Jvm::istore_3(){

}




void Jvm::lstore_0(){

}




void Jvm::lstore_1(){

}




void Jvm::lstore_2(){

}




void Jvm::lstore_3(){

}




void Jvm::fstore_0(){

}




void Jvm::fstore_1(){

}




void Jvm::fstore_2(){

}




void Jvm::fstore_3(){

}




void Jvm::dstore_0(){

}




void Jvm::dstore_1(){

}




void Jvm::dstore_2(){

}




void Jvm::dstore_3(){

}




void Jvm::astore_0(){

}




void Jvm::astore_1(){

}




void Jvm::astore_2(){

}




void Jvm::astore_3(){

}




void Jvm::iastore(){

}




void Jvm::lastore(){

}




void Jvm::fastore(){

}




void Jvm::dastore(){

}




void Jvm::aastore(){

}




void Jvm::bastore(){

}




void Jvm::castore(){

}




void Jvm::sastore(){

}




void Jvm::pop(){

}




void Jvm::pop2(){

}




void Jvm::dup(){

}




void Jvm::dup_x1(){

}




void Jvm::dup_x2(){

}




void Jvm::dup2(){

}




void Jvm::dup2_x1(){

}




void Jvm::dup2_x2(){

}




void Jvm::swap(){

}




void Jvm::iadd(){
    int32_t value1;
    int32_t value2;
    value2 = static_cast<int>(CurrentFrame->operandStack->Pop());
    value1 = static_cast<int>(CurrentFrame->operandStack->Pop());
    int32_t sum = value1 + value2;
    CurrentFrame->operandStack->push(static_cast<u4>(sum));
}




void Jvm::ladd(){

}




void Jvm::fadd(){

}




void Jvm::dadd(){

}




void Jvm::isub(){

}




void Jvm::lsub(){

}




void Jvm::fsub(){

}




void Jvm::dsub(){

}




void Jvm::imul(){

}




void Jvm::lmul(){

}




void Jvm::fmul(){

}




void Jvm::dmul(){

}




void Jvm::idiv(){

}




void Jvm::ldiv(){

}




void Jvm::fdiv(){

}




void Jvm::ddiv(){

}




void Jvm::irem(){

}




void Jvm::lrem(){

}




void Jvm::frem(){

}




void Jvm::drem(){

}




void Jvm::ineg(){

}




void Jvm::lneg(){

}




void Jvm::fneg(){

}




void Jvm::dneg(){

}




void Jvm::ishl(){

}




void Jvm::lshl(){

}




void Jvm::ishr(){

}




void Jvm::lshr(){

}




void Jvm::iushr(){

}




void Jvm::lushr(){

}




void Jvm::iand(){

}




void Jvm::land(){

}




void Jvm::ior(){

}




void Jvm::lor(){

}




void Jvm::ixor(){

}




void Jvm::lxor(){

}




void Jvm::iinc(){

}




void Jvm::i2l(){

}




void Jvm::i2f(){

}




void Jvm::i2d(){

}




void Jvm::l2i(){

}




void Jvm::l2f(){

}




void Jvm::l2d(){

}




void Jvm::f2i(){

}




void Jvm::f2l(){

}




void Jvm::f2d(){

}




void Jvm::d2i(){

}




void Jvm::d2l(){

}




void Jvm::d2f(){

}




void Jvm::i2b(){

}




void Jvm::i2c(){

}




void Jvm::i2s(){

}




void Jvm::lcmp(){

}




void Jvm::fcmpl(){

}




void Jvm::fcmpg(){

}




void Jvm::dcmpl(){

}




void Jvm::dcmpg(){

}




void Jvm::ifeq(){

}




void Jvm::ifne(){

}




void Jvm::iflt(){

}




void Jvm::ifge(){

}




void Jvm::ifgt(){

}




void Jvm::ifle(){

}




void Jvm::if_icmpeq(){

}




void Jvm::if_icmpne(){

}




void Jvm::if_icmplt(){

}




void Jvm::if_icmpge(){

}




void Jvm::if_icmpgt(){

}




void Jvm::if_icmple(){

}




void Jvm::if_acmpeq(){

}




void Jvm::if_acmpne(){

}




void Jvm::goto_(){

}




void Jvm::jsr_(){

}




void Jvm::ret_(){

}




void Jvm::tableswitch(){

}




void Jvm::lookupswitch(){

}




void Jvm::ireturn(){

}




void Jvm::lreturn(){

}




void Jvm::freturn(){

}




void Jvm::dreturn(){

}




void Jvm::areturn(){

}




void Jvm::return_(){

}

void Jvm::getstatic(){

}

void Jvm::putstatic(){

}

void Jvm::getfield(){

}

void Jvm::putfield(){

}

void Jvm::invokevirtual(){

}

void Jvm::invokespecial(){

}

void Jvm::invokestatic(){

}

void Jvm::invokeinterface(){

}

void Jvm::invokedynamic(){

}

void Jvm::new_(){

}

void Jvm::newarray(){

}

void Jvm::anewarray(){

}

void Jvm::arraylength(){

}

void Jvm::athrow(){

}

void Jvm::checkcast(){

}

void Jvm::instanceof() {

}

void Jvm::monitorenter(){

}

void Jvm::monitorexit(){

}

void Jvm::wide(){

}

void Jvm::multianewarray(){

}

void Jvm::ifnull(){

}

void Jvm::ifnonnull(){

}

void Jvm::goto_w(){

}

void Jvm::jsr_w(){

}




