//jvm
// Created by matheus on 4/11/24.
//

#include "../include/Jvm.h"
#include <cmath>
#include <limits>

void Jvm::Run(){
    MethodArea = new std::unordered_map<std::string,class_file*>;
    Loader     = new ClassLoader(MethodArea);

    std::cout<<"main file "<<main_file<<"\n";
    Loader->LoadClass(main_file);
    CurrentClass = GetClass(main_file);
    //create method map by name

    std::string MethodName = "main";
    GetMethod(MethodName);
    std::cout<<"method found\n";
    GetCurrentMethodCode();
    std::cout<<"code found\n";

    NewFrame();
    std::cout<<"frame created\n";

    //check for <clinit>
    //exec 
    //check for <init>
    //exec
    //check for <main>
    //exec

    ExecBytecode();

}

// big endian
u8 Jvm::popU8FromOpStack(){
    u4 LowBytes = CurrentFrame->OperandStack->Pop();
    u4 HighBytes = CurrentFrame->OperandStack->Pop();
    return (static_cast<u8>(HighBytes) << 32) | LowBytes;
}
// big endian
void Jvm::pushU8ToOpStack(u4 HighBytes, u4 LowBytes){
    CurrentFrame->OperandStack->push(HighBytes);
    CurrentFrame->OperandStack->push(LowBytes);
}

// big endian
u8 Jvm::getU8FromLocalVars(u4 startingIndex){
    u4 LowBytes = (*CurrentFrame->localVariables)[startingIndex];
    u4 HighBytes = (*CurrentFrame->localVariables)[startingIndex + 1];
    return (static_cast<u8>(HighBytes) << 32) | LowBytes;
}

u1 inline Jvm::NextCodeByte(){
    return (*CurrentCode->code)[pc++];
} 

void Jvm::ExecBytecode(){
    while(pc != CurrentCode->code_length){
        u1 bytecode =  NextCodeByte();
        (this->*bytecodeFuncs[bytecode])();
    }
}

//salva valores atuais pro retorno ao frame
void Jvm::SaveFrameState(){
    CurrentFrame->nextPC      = pc;
    CurrentFrame->frameClass  = CurrentClass;
    CurrentFrame->frameMethod = CurrentMethod;
}

void Jvm::NewFrame(){

    //instancia novo frame
    auto NewFrame = new Frame;
    NewFrame->localVariables = new JVM::vector<u4>;
    NewFrame->OperandStack   = new JVM::stack<u4>;

    u2 MaxLocals = CurrentCode->max_locals;
    NewFrame->localVariables->resize(MaxLocals);

    //substitui frame atual por novo frame
    FrameStack.push(NewFrame);
    CurrentFrame = NewFrame;
    pc = 0;

}

void Jvm::PopFrameStack(){
    
    Frame* OldFrame = FrameStack.Pop();
    delete OldFrame;
    if(FrameStack.empty())
        return;
    CurrentFrame    = FrameStack.top();
    CurrentClass    = CurrentFrame->frameClass;
    CurrentMethod   = CurrentFrame->frameMethod;
    pc		        = CurrentFrame->nextPC; // Aponta pra instrucao seguinte a que chamou um novo frame

    GetCurrentMethodCode();
}





void Jvm::GetMethod(const std::string& MethodName){

    for(auto Method: *CurrentClass->methods)
        if((*CurrentClass->constant_pool)[Method->name_index]->AsString() == MethodName){
            CurrentMethod = Method;
            return;
        }
    throw std::runtime_error("no method with given name "+ MethodName);
}
//TODO: ter um jeito de manter o current class do frame antigo( num sei se precisa ainda )
void Jvm::GetCurrentMethodCode(){
      
    for(auto Attribute : *CurrentMethod->attributes){
        u2 NameIndex = Attribute->attribute_name_index;
        cp_info* AttributeName = (*CurrentClass->constant_pool)[NameIndex];
        if(AttributeName->AsString() == "Code"){
            CurrentCode = Attribute;
            return;
        }
    }
}  



class_file* Jvm::GetClass(std::string class_name){
    if(MethodArea->find(class_name) == MethodArea->end()) {
        std::cout<<"loading class "<<class_name<<"\n";
        Loader->LoadClass(class_name);
    //TODO: EXECUTAR METODO ESTATICO DESSA CLASSE
    }

    return (*MethodArea)[class_name];
}

// get class file and create instance

void Jvm::NewClassInstance(std::string class_name){
    ClassInstance* NewClassInstance;
    class_file* ClassFile = GetClass(class_name);
    // magia da criacao
    CurrentFrame->OperandStack->PushRef(NewClassInstance);
}




// funcoes auxiliares pros bytecode


u2 Jvm::GetIndex2() {
    u1 indexbyte1 = (*CurrentCode->code)[pc++];
    u1 indexbyte2 = (*CurrentCode->code)[pc++];
    return (indexbyte1 << 8) | indexbyte2;
}

void Jvm::return_u4(){
    u4 ReturnValue = CurrentFrame->OperandStack->Pop();
    PopFrameStack();
    CurrentFrame->OperandStack->push(ReturnValue);
}

void Jvm::return_u8(){

    u4 ReturnValue1 = CurrentFrame->OperandStack->Pop();
    u4 ReturnValue2 = CurrentFrame->OperandStack->Pop();
    PopFrameStack();
    CurrentFrame->OperandStack->push(ReturnValue2);
    CurrentFrame->OperandStack->push(ReturnValue1);

}




// so bytecode




// Do nothing
void Jvm::nop() {

}
// An aconst_null instruction is type safe if one can validly push the type null onto the incoming operand stack yielding the outgoing type state.
void Jvm::aconst_null(){
    // !TODO checar se a versao cpp especificada possui nullptr
    CurrentFrame->OperandStack->PushRef(nullptr);
}

// Push the int constant <i> (-1, 0, 1, 2, 3, 4 or 5) onto the operand
// stack.
// Notes Each of this family of instructions is equivalent to bipush <i> for
// the respective value of <i>, except that the operand <i> is implicit.
void Jvm::iconst_m1(){
    CurrentFrame->OperandStack->push(static_cast<const u4>(-1));
}

void Jvm::iconst_0(){
    CurrentFrame->OperandStack->push(static_cast<const u4>(0));
}

void Jvm::iconst_1(){
    CurrentFrame->OperandStack->push(static_cast<const u4>(1));
}

void Jvm::iconst_2(){
    CurrentFrame->OperandStack->push(static_cast<const u4>(2));
}

void Jvm::iconst_3(){
    CurrentFrame->OperandStack->push(static_cast<const u4>(3));
}

void Jvm::iconst_4(){
    CurrentFrame->OperandStack->push(static_cast<const u4>(4));
}

void Jvm::iconst_5(){
    CurrentFrame->OperandStack->push(static_cast<const u4>(5));
}

void Jvm::lconst_0(){
    // todo: check if anything wrong, was it the right cast??
    CurrentFrame->OperandStack->push(static_cast<const s8>(0.0));
}

void Jvm::lconst_1(){
    CurrentFrame->OperandStack->push(static_cast<const s8>(1.0));
}
 // big-endian order, where the high bytes come first
// Push the float constant <f> (0.0, 1.0, or 2.0) onto the operand stack
void Jvm::fconst_0(){
    const float value = 0.0;
    //transforma de float pra uint32_t
    // todo:checa se devia ta pushando dois u8
    Cat2Value Cat2Value;
    Cat2Value.AsFloat = value;  
    const u4 HighBytes = Cat2Value.HighBytes; 
    CurrentFrame->OperandStack->push(HighBytes);
}

void Jvm::fconst_1(){
    const float value = 1.0;
    //transforma de float pra uint32_t
    Cat2Value Cat2Value;
    Cat2Value.AsFloat = value;  
    const u4 HighBytes = Cat2Value.HighBytes; 
    CurrentFrame->OperandStack->push(HighBytes);
}

void Jvm::fconst_2(){
    const float value = 2.0;
    //transforma de float pra uint32_t
    Cat2Value Cat2Value;
    Cat2Value.AsFloat = value;  
    const u4 HighBytes = Cat2Value.HighBytes; 
    CurrentFrame->OperandStack->push(HighBytes);
// pra checar se pusha high ou low
/*
const uint32_t LowBytes = Cat2Value.LowBytes; 
    for (int i = 31; i >= 0; i--) {
    std::cout << ((LowBytes >> i) & 1);
  }

std::cout << " ";
    for (int i = 31; i >= 0; i--) {
    std::cout << ((HighBytes >> i) & 1);
  }
  */

   //era pra dar pc++ ao final de cada funcao??
}

void Jvm::dconst_0(){
    const double value = 0.0;
    //pusha dois u4, big endian (most significant first)
    Cat2Value Cat2Value;
    Cat2Value.AsDouble = value;  
    const u4 HighBytes = Cat2Value.HighBytes; 
    const u4 LowBytes = Cat2Value.LowBytes; 
    CurrentFrame->OperandStack->push(HighBytes);
    CurrentFrame->OperandStack->push(LowBytes);
}

void Jvm::dconst_1(){
    const double value = 1.0;
    //pusha dois u4, big endian (most significant first)
    Cat2Value Cat2Value;
    Cat2Value.AsDouble = value;  
    const u4 HighBytes = Cat2Value.HighBytes; 
    const u4 LowBytes = Cat2Value.LowBytes; 
    CurrentFrame->OperandStack->push(HighBytes);
    CurrentFrame->OperandStack->push(LowBytes);
}
// The immediate byte is sign-extended to an int value. That value
// is pushed onto the operand stac
// !TODO: checar se implmentacao ta certa


void Jvm::bipush(){
CurrentFrame->OperandStack->push((*CurrentCode->code)[++pc]);
}

void Jvm::sipush(){

}


void Jvm::ldc(){
    u1 index = (*CurrentCode->code)[pc++];
    cp_info* RunTimeConstant = (*CurrentClass->constant_pool)[index];
    switch (RunTimeConstant->tag) {
        case ConstantPoolTag::CONSTANT_Integer:
        case ConstantPoolTag::CONSTANT_Float:{
            u4 value = RunTimeConstant->bytes;
        }
        case ConstantPoolTag::CONSTANT_String:{
            void* StringRef = (*CurrentClass->constant_pool)[RunTimeConstant->string_index];
            CurrentFrame->OperandStack->PushRef(StringRef);
        }
        default:{
            //deu ruim
        }
    }

}

void Jvm::ldc_w(){

}

void Jvm::ldc2_w(){
    // Cat2Value converter;
    u1 indexbyte1 = (*CurrentCode->code)[pc++];
    u1 indexbyte2 = (*CurrentCode->code)[pc++];
    u4 index = (indexbyte1 << 8) | indexbyte2;

    cp_info* cpEntry = (*CurrentClass->constant_pool)[index];

    switch (cpEntry->tag) {
        case ConstantPoolTag::CONSTANT_Long:
        case ConstantPoolTag::CONSTANT_Double:{
            std::cout<<"pushing long or double\n";
            pushU8ToOpStack(cpEntry->high_bytes, cpEntry->low_bytes);
        }
        default:{
            //deu ruim
        }
    }
    

}

void Jvm::iload(){
// !TODO: fix, ta errado/incompleto-- 1 index ta errado,2 pode usar wide() pra pegar index
    int index = GetIndex2();
    u4 value = (*CurrentFrame->localVariables)[index];
    CurrentFrame->OperandStack->push(value);

}

void Jvm::lload(){
    u1 index = (*CurrentCode->code)[pc++];
    Cat2Value converter;
// todo should it throw error if index not valid
    converter.AsLong = getU8FromLocalVars(index);
    converter.HighBytes = converter.AsLong;
    converter.LowBytes = converter.AsLong;

    pushU8ToOpStack(converter.HighBytes, converter.LowBytes);

}

void Jvm::fload(){
    u1 index = (*CurrentCode->code)[pc++];
    Cat2Value converter;
    
    converter.AsFloat = (*CurrentFrame->localVariables)[index];
    CurrentFrame->OperandStack->push(converter.AsFloat);

}

void Jvm::dload(){
    u1 index = (*CurrentCode->code)[pc++];
    Cat2Value converter;

    converter.AsDouble = getU8FromLocalVars(index);
    converter.HighBytes = converter.AsLong;
    converter.LowBytes = converter.AsLong;

    pushU8ToOpStack(converter.HighBytes, converter.LowBytes);

}
// todo aload, check docs to ensure dload fload etc r alright
void Jvm::aload(){
    u1 index = (*CurrentCode->code)[pc++];
    u4 objectref = (*CurrentFrame->localVariables)[index];
    CurrentFrame->OperandStack->push(objectref);

}

void Jvm::iload_0(){
    u4 value;
    value = (*CurrentFrame->localVariables)[0];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::iload_1(){
    u4 value;
    value = (*CurrentFrame->localVariables)[1];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::iload_2(){
    u4 value;
    value = (*CurrentFrame->localVariables)[2];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::iload_3(){
    u4 value;
    value = (*CurrentFrame->localVariables)[3];
    CurrentFrame->OperandStack->push(value);
}


// !todo: implement these ez loads next
// todo mb exception if index out of bounds
// todo check
void Jvm::lload_0(){
    Cat2Value converter;

    converter.AsLong = getU8FromLocalVars(0);
    converter.HighBytes = converter.AsLong;
    converter.LowBytes = converter.AsLong;

    pushU8ToOpStack(converter.HighBytes, converter.LowBytes);
}



// Both <n> and <n>+1 must be indices into the local variable array
void Jvm::lload_1(){

}




void Jvm::lload_2(){

}




void Jvm::lload_3(){

}




void Jvm::fload_0(){
    Cat2Value converter;
    
    converter.AsFloat = (*CurrentFrame->localVariables)[0];
    CurrentFrame->OperandStack->push(converter.AsFloat);

}




void Jvm::fload_1(){

}




void Jvm::fload_2(){

}




void Jvm::fload_3(){

}



// todo test
void Jvm::dload_0(){
    Cat2Value converter;

    converter.AsDouble = getU8FromLocalVars(0);
    converter.HighBytes = converter.AsLong;
    converter.LowBytes = converter.AsLong;

    pushU8ToOpStack(converter.HighBytes, converter.LowBytes);

}




void Jvm::dload_1(){

}




void Jvm::dload_2(){

}




void Jvm::dload_3(){

}




void Jvm::aload_0(){
    u4 value;
    value = (*CurrentFrame->localVariables)[0];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::aload_1(){
    u4 value;
    value = (*CurrentFrame->localVariables)[1];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::aload_2(){
    u4 value;
    value = (*CurrentFrame->localVariables)[2];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::aload_3(){
    u4 value;
    value = (*CurrentFrame->localVariables)[3];
    CurrentFrame->OperandStack->push(value);
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
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[0] = value;
}




void Jvm::istore_1(){
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[1] = value;
}




void Jvm::istore_2(){
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[2] = value;
}




void Jvm::istore_3(){
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[3] = value;
}



// todo do these after next mb ez
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
    u8 value = popU8FromOpStack();
    (*CurrentFrame->localVariables)[1] = value;
}




void Jvm::dstore_2(){

}




void Jvm::dstore_3(){

}




void Jvm::astore_0(){
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[0] = value;
}




void Jvm::astore_1(){
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[1] = value;
}




void Jvm::astore_2(){
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[2] = value;
}




void Jvm::astore_3(){
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[3] = value;
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
    CurrentFrame->OperandStack->pop();
}




void Jvm::pop2(){
    CurrentFrame->OperandStack->pop();
    CurrentFrame->OperandStack->pop();
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
    value2 = static_cast<int>(CurrentFrame->OperandStack->Pop());
    value1 = static_cast<int>(CurrentFrame->OperandStack->Pop());
    int32_t result = value1 + value2;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
}



// Both value1 and value2 must be of type long. The values are popped from the operand stack. The long result is value1 + value2. The result is pushed onto the operand stack. The result is the 64 low-order bits of the true mathematical result in a sufficiently wide two's-complement format, represented as a value of type long. If overflow occurs, the sign of the result may not be the same as the sign of the mathematical sum of the two values. Despite the fact that overflow may occur, execution of an ladd instruction never throws a run-time exception.
void Jvm::ladd(){
    // !TODO checar: implementacao erradisima
    long value1;
    long value2;
    value2 = static_cast<int>(CurrentFrame->OperandStack->Pop());
    value1 = static_cast<int>(CurrentFrame->OperandStack->Pop());
    int32_t result = value1 + value2;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
}



// Both value1 and value2 must be of type float. The values are popped from the operand stack and undergo value set conversion (§2.8.3), resulting in value1' and value2'. The float result is value1' + value2'. The result is pushed onto the operand stack. The result of an fadd instruction is governed by the rules of IEEE arithmetic: • If either value1' or value2' is NaN, the result is NaN. • The sum of two infinities of opposite sign is NaN. • The sum of two infinities of the same sign is the infinity of that sign. • The sum of an infinity and any finite value is equal to the infinity. • The sum of two zeroes of opposite sign is positive zero. • The sum of two zeroes of the same sign is the zero of that sign. • The sum of a zero and a nonzero finite value is equal to the nonzero value. • The sum of two nonzero finite values of the same magnitude and opposite sign is positive zero. • In the remaining cases, where neither operand is an infinity, a zero, or NaN and the values have the same sign or have different magnitudes, the sum is computed and rounded to the nearest representable value using IEEE 754 round to nearest mode. If THE JAVA VIRTUAL MACHINE INSTRUCTION SET Instructions 6.5 421 the magnitude is too large to represent as a float, we say the operation overflows; the result is then an infinity of appropriate sign. If the magnitude is too small to represent as a float, we say the operation underflows; the result is then a zero of appropriate sign. The Java Virtual Machine requires support of gradual underflow as defined by IEEE 754. Despite the fact that overflow, underflow, or loss of precision may occur, execution of an fadd instruction never throws a run-time ex
void Jvm::fadd(){
    // !TODO checar: essa implementacao tava em iadd, colocada aqui, spec acima
    float value1;
    float value2; //todo fix this static cast
    value2 = static_cast<float>(CurrentFrame->OperandStack->Pop());
    value1 = static_cast<float>(CurrentFrame->OperandStack->Pop());

    float result = value1 + value2;
    // !todo: checar se necessita dos ifs pra nan, etc ali nos •  •  acima
    // !TODO check these static casts pode da erro precisa dos if f2i provavelmente
    CurrentFrame->OperandStack->push(static_cast<u4>(result));

}



// !todo: fix, erradasso
void Jvm::dadd(){
    // !TODO deve ta errado, como tratar double na op stack?
    double value1;
    double value2;
    value2 = static_cast<int>(CurrentFrame->OperandStack->Pop());
    value1 = static_cast<int>(CurrentFrame->OperandStack->Pop());

    float result = value1 + value2;
    // !todo: checar se necessita dos ifs pra nan, etc ali nos •  •  acima
    // !TODO: fix static cast, e isso n ta double
    CurrentFrame->OperandStack->push(static_cast<float>(result));

}




void Jvm::isub(){
    int32_t value1;
    int32_t value2;
    value2 = static_cast<int>(CurrentFrame->OperandStack->Pop());
    value1 = static_cast<int>(CurrentFrame->OperandStack->Pop());
    int32_t result = value1 - value2;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
}




void Jvm::lsub(){

}




void Jvm::fsub(){

}




void Jvm::dsub(){

}




void Jvm::imul(){
    int32_t value1;
    int32_t value2;
    value2 = static_cast<int>(CurrentFrame->OperandStack->Pop());
    value1 = static_cast<int>(CurrentFrame->OperandStack->Pop());
    int32_t result = value1 * value2;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
}




void Jvm::lmul(){

}




void Jvm::fmul(){

}




void Jvm::dmul(){

}




void Jvm::idiv(){
    auto value2 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());
    auto value1 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());
    if (value2 == 0)
        throw ArithmeticException();
    int32_t result = value1 / value2;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
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
    int32_t value;
    value = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());
    int32_t result = - value;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
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

    int32_t value2 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());
    int32_t value1 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());
    int32_t result =  value1 & value2;

    CurrentFrame->OperandStack->push(static_cast<u4>(result));

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



// idk todo test
void Jvm::i2l(){
    Cat2Value converter;
    converter.AsLong = CurrentFrame->OperandStack->Pop();
    converter.HighBytes = converter.AsLong;
    converter.LowBytes = converter.AsLong;
    
    // push pro operand stack em big endian
    pushU8ToOpStack(converter.HighBytes, converter.LowBytes);
}




void Jvm::i2f(){
    int value = static_cast<int>(CurrentFrame->OperandStack->Pop());
    float result = static_cast<float>(value);
    CurrentFrame->OperandStack->push(result);
}


//Converte int pra double
void Jvm::i2d(){
    Cat2Value converter;
    
    converter.AsDouble = CurrentFrame->OperandStack->Pop();
    converter.HighBytes = converter.AsDouble;
    converter.LowBytes = converter.AsDouble;
    
    // push em big endian
    CurrentFrame->OperandStack->push(converter.HighBytes);
    CurrentFrame->OperandStack->push(converter.HighBytes);
    CurrentFrame->OperandStack->push(converter.LowBytes );
}




void Jvm::l2i(){
    Cat2Value converter;

    converter.AsLong = popU8FromOpStack();
    converter.HighBytes = converter.AsLong;
    converter.LowBytes = converter.AsLong;
    // push pro stack de operandos em big endian
    CurrentFrame->OperandStack->push(converter.LowBytes);

}




void Jvm::l2f(){
    Cat2Value converter;

    converter.AsLong = popU8FromOpStack();
    // todo test
    converter.AsFloat = converter.AsLong;
    CurrentFrame->OperandStack->push(converter.AsFloat);

}




void Jvm::l2d(){
    Cat2Value converter;

    converter.AsLong = popU8FromOpStack();
    converter.AsDouble = converter.AsLong;
// todo: test if this is indeed the correct big endian low and high bytes
    converter.HighBytes = converter.AsDouble;
    converter.LowBytes = converter.AsDouble;
    
    // push pro operand stack em big endian
    pushU8ToOpStack(converter.HighBytes, converter.LowBytes);

}




void Jvm::f2i(){
    //converte u4 pra float, float pra int
    float value = static_cast<float>(CurrentFrame->OperandStack->Pop());
    u4 intValue = static_cast<u4>(value);
    u4 result;

    if(std::isnan(intValue)){ 
        result = 0;
        // todo checar o que aqui devia ser comparacao com signed ou unsigned
    } else if( intValue >= INT32_MAX || intValue == std::numeric_limits<u4>::infinity()){ // todo test these infinities
        result = INT32_MAX;
    } else if( intValue <= INT32_MIN || intValue == -1 * std::numeric_limits<u4>::infinity()) { 
        result = INT32_MIN;
        // todo como converte negativo pra uint ? dependendo do compilador que carinha de erro
    } else { 
        result = static_cast<u4>(intValue);
    }
    
    CurrentFrame->OperandStack->push(result);

}



// testado (hardcode, n cm file) ok
// !todo: excluir prints ez
void Jvm::f2l(){
    //converte u4 pra float, float pra long
    Cat2Value converter;
    float value = static_cast<float>(CurrentFrame->OperandStack->Pop());

    converter.AsLong = value;

    if(std::isnan(converter.AsLong)){ 
        converter.HighBytes = 0;
        converter.LowBytes = 0;
         std::cout<<"0\n";

    } else if(  converter.AsLong >= INT64_MAX ||  converter.AsLong == std::numeric_limits<float>::infinity()){ // todo test these infinities                         
        // !todo test if this rlly makes int64 in high & low bytes
        converter.LowBytes = static_cast<u4>((INT64_MAX & 0xffffffff00000000) >> 32);
        converter.HighBytes = static_cast<u4>(INT64_MAX & 0x00000000ffffffff);
        std::cout<<"max\n";

    } else if(  converter.AsLong <= INT64_MIN ||  converter.AsLong == -1 * std::numeric_limits<float>::infinity()) {         
        converter.LowBytes = static_cast<u4>((INT64_MIN & 0xffffffff00000000) >> 32);
        converter.HighBytes = static_cast<u4>(INT64_MIN & 0x00000000ffffffff);
        std::cout<<"min\n";
        
    } else { 
        converter.HighBytes = converter.AsLong;
        converter.LowBytes = converter.AsLong;
         std::cout<<"ok\n";
    }
    
    pushU8ToOpStack(converter.HighBytes, converter.LowBytes);

}



// todo test
void Jvm::f2d(){
    //converte u4 pra float, float pra double
    Cat2Value converter;
    converter.AsFloat = static_cast<float>(CurrentFrame->OperandStack->Pop());
    // todo check if static cast da problema e se sem ele ta ok

    converter.AsDouble = converter.AsFloat ;
    converter.HighBytes = converter.AsDouble;
    converter.LowBytes = converter.AsDouble;

    pushU8ToOpStack(converter.HighBytes, converter.LowBytes);

}



void Jvm::d2i(){
    //big endian: highbytes first
    double value;
    u4 result;

    u4 LowBytes  = CurrentFrame->OperandStack->Pop();
    u4 HighBytes = CurrentFrame->OperandStack->Pop();
    memcpy(&value, &HighBytes, sizeof(u4));
    memcpy(&value, reinterpret_cast<void*>(&LowBytes), sizeof(u4));

    // converte double pra int
    if(std::isnan(value)){ 
        result = 0;
    } else if( value >= INT32_MAX || value == std::numeric_limits<double>::infinity()){ // todo test these infinities
        result = INT32_MAX;
    } else if( value <= INT32_MIN || value == -1 * std::numeric_limits<double>::infinity()) { 
        result = INT32_MIN;
    } else { // IEEE 754 round towards zero mode: check if rounded to zero 
    // !todo testa parece errado
        result = static_cast<u4>(value);
    }
    // push pro stack de operandos
    CurrentFrame->OperandStack->push(result);

}




void Jvm::d2l(){
    Cat2Value converter;

    converter.AsDouble = popU8FromOpStack();
    converter.AsLong =  converter.AsDouble;
    long long value = converter.AsLong;

    
    // converte Long pra double
    if(std::isnan(value)){ 
        converter.HighBytes = 0;
        converter.LowBytes = 0; 
    } else if( value >= __LONG_LONG_MAX__ || value == std::numeric_limits<long long>::infinity()){ // todo test these infinities
        converter.LowBytes = static_cast<u4>((__LONG_LONG_MAX__ & 0xffffffff00000000) >> 32);
        converter.HighBytes = static_cast<u4>(__LONG_LONG_MAX__ & 0x00000000ffffffff); 
   
    } else if( value <=  std::numeric_limits<long long>::min() || value == -1 * std::numeric_limits<long long>::infinity()) { // n achei min pra long long em cpp (que funcionasse) mas como vai virar unsigned p min tá 0. long specs: In Java SE 8 and later, you can use the long data type to represent an unsigned 64-bit long, which has a minimum value of 0 and a maximum value of 264-1
        converter.LowBytes = static_cast<u4>((std::numeric_limits<long long>::min() & 0xffffffff00000000) >> 32);
        converter.HighBytes = static_cast<u4>(std::numeric_limits<long long>::min() & 0x00000000ffffffff); 

    } else { // IEEE 754 round towards zero mode: check if rounded to zero 
        converter.HighBytes = converter.AsLong;
        converter.LowBytes = converter.AsLong;
    }


}



// todo test
void Jvm::d2f(){
    Cat2Value converter;

    converter.AsDouble = popU8FromOpStack();
    double value = converter.AsDouble;

    // converte Long pra double
    if(std::isnan(value)){ 
        // float nan
        converter.AsFloat = std::nanf("");
    } else if( value >= __FLT_MAX__ || value == std::numeric_limits<float>::infinity()){ // todo test these infinities
        converter.AsFloat = std::numeric_limits<float>::max();
    } else if( value <=  __FLT_MIN__ || value == -1 * std::numeric_limits<float>::infinity()) { // n achei min pra long long em cpp (que funcionasse) mas como vai virar unsigned p min tá 0. long specs: In Java SE 8 and later, you can use the long data type to represent an unsigned 64-bit long, which has a minimum value of 0 and a maximum value of 264-1
        converter.AsFloat = std::numeric_limits<float>::min();
    } else { 
        converter.AsFloat = converter.AsDouble;
    }

    CurrentFrame->OperandStack->push(converter.AsFloat);

}


void Jvm::i2b(){
    FieldEntry converter;
    // trunca int (u4, 4 bytes) pra byte (u1)
    converter.AsByte = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(converter.AsByte);
}




void Jvm::i2c(){
    FieldEntry converter;
    converter.AsChar = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(converter.AsChar);

}



// short is signed int16_t (s2)
void Jvm::i2s(){
    Cat2Value converter;
    // trunca e faz sign extension
    converter.AsShort = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(converter.AsShort);
}



// !TODO: fix, n eh assim q se trata long
void Jvm::lcmp(){
    long value1 = static_cast<long>(CurrentFrame->OperandStack->Pop());
    long value2 = static_cast<long>(CurrentFrame->OperandStack->Pop());
    int intValue = 9;
    if(value1 > value2)
        intValue = 1;
    else if(value1 == value2)
        intValue = 0;
    else // (value1 < value2)
        intValue = -1;
        
    CurrentFrame->OperandStack->push(static_cast<u1>(intValue));

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




void Jvm::jsr(){

}




void Jvm::ret(){
    u1 index = (*CurrentCode->code)[pc++];
    uint32_t NewPc = (*CurrentFrame->localVariables)[index];
    pc = NewPc;
}


// Description

//     A tableswitch is a variable-length instruction. Immediately after the tableswitch opcode, between 0 and 3 null bytes (zeroed bytes, not the null object) are inserted as padding. The number of null bytes is chosen so that the following byte begins at an address that is a multiple of 4 bytes from the start of the current method (the opcode of its first instruction). Immediately after the padding follow bytes constituting three signed 32-bit values: default, low, and high. Immediately following those bytes are bytes constituting a series of high - low + 1 signed 32-bit offsets. The value low must be less than or equal to high. The high - low + 1 signed 32-bit offsets are treated as a 0-based jump table. Each of these signed 32-bit values is constructed as (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4.

//     The index must be of type int and is popped from the operand stack. If index is less than low or index is greater than high, then a target address is calculated by adding default to the address of the opcode of this tableswitch instruction. Otherwise, the offset at position index - low of the jump table is extracted. The target address is calculated by adding that offset to the address of the opcode of this tableswitch instruction. Execution then continues at the target address.

//     The target address that can be calculated from each jump table offset, as well as the ones that can be calculated from default, must be the address of an opcode of an instruction within the method that contains this tableswitch instruction.

// Notes

    // The alignment required of the 4-byte operands of the tableswitch instruction guarantees 4-byte alignment of those operands if and only if the method that contains the tableswitch starts on a 4-byte boundary.
void Jvm::tableswitch() {

}


void Jvm::lookupswitch(){
  std::cout<<"tableswitch\n";  std::cout<<"tableswitch\n";
}




void Jvm::ireturn(){
//check return == int
    return_u4();
}




void Jvm::lreturn(){
//check return == long
    return_u8();
}





void Jvm::freturn(){

//check return == float
    return_u4();
}




void Jvm::dreturn(){
//check return == double
    return_u8();
}




void Jvm::areturn(){
//check return == ref
    return_u4();
}


void Jvm::return_(){
//check return == void
	PopFrameStack();		 
}

void Jvm::getstatic(){
    u2 index = GetIndex2();
}
// todo mb ez
void Jvm::putstatic(){

}
// todo mb ez
void Jvm::getfield(){
    u2 index = GetIndex2();
    cp_info* Fieldref = (*CurrentClass->constant_pool)[index];
    cp_info* NameAndTypeEntry = (*CurrentClass->constant_pool)[Fieldref->name_and_type_index];


}

void Jvm::putfield(){
    u2 index = GetIndex2();

    cp_info* Fieldref = (*CurrentClass->constant_pool)[index];
    cp_info* NameAndType = (*CurrentClass->constant_pool)[Fieldref->name_and_type_index];

    std::string Name            = (*CurrentClass->constant_pool)[NameAndType->name_index]->AsString();
    std::string FieldDescriptor = (*CurrentClass->constant_pool)[NameAndType->descriptor_index]->AsString();
    //std::string Type = FieldDescriptor.substr(1,FieldDescriptor.size() - 2);

    if(FieldDescriptor == "D" or FieldDescriptor == "L"){
	u4 lowBytes  = CurrentFrame->OperandStack->Pop();
	u4 highBytes = CurrentFrame->OperandStack->Pop();

	//u4 objectRef = CurrentFrame->OperandStack->Pop();

    }
    else{
    	u4 value = CurrentFrame->OperandStack->Pop();
	    //u4 objectRef = CurrentFrame->OperandStack->Pop();
    }



}
//TODO: to fazendo so pro hello world por enquanto
void Jvm::invokevirtual(){
    u2 index = GetIndex2();

    cp_info* MethodRef = (*CurrentClass->constant_pool)[index];

    cp_info* NameAndType = (*CurrentClass->constant_pool)[MethodRef->name_and_type_index];
    // formato <valor>
    std::string Name = (*CurrentClass->constant_pool)[NameAndType->name_index]->AsString();
    std::string MethodDescriptor = (*CurrentClass->constant_pool)[NameAndType->descriptor_index]->AsString();

    if(MethodDescriptor == "(Ljava/lang/String;)V"){
        auto String = CurrentFrame->OperandStack->PopRef<cp_info>()->AsString();
        std::cout<<String<<"\n";
    }



}

void Jvm::invokespecial(){
    u2 index = GetIndex2();
    cp_info* MethodOrInterfaceMethod = (*CurrentClass->constant_pool)[index];


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

    auto Count = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());
    if(Count < 0)
        throw NegativeArraySizeException();

    u1 ArrayTypeCodeValue = (*CurrentCode->code)[pc++];

    u1 ArrayEntrySize;
    switch (static_cast<ArrayTypeCode>(ArrayTypeCodeValue)) {
        case ArrayTypeCode::T_BOOLEAN:
        case ArrayTypeCode::T_BYTE: {
            ArrayEntrySize = 1;
            break;
        }
        case ArrayTypeCode::T_CHAR:
        case ArrayTypeCode::T_SHORT:{
            ArrayEntrySize = 2;
            break;
        }

        case ArrayTypeCode::T_FLOAT:
        case ArrayTypeCode::T_INT:{
            ArrayEntrySize = 4;
            break;
        }

        case ArrayTypeCode::T_DOUBLE:
        case ArrayTypeCode::T_LONG:{
            ArrayEntrySize = 8;
            break;
        }

    }

    void* ArrayRef = new char[Count * ArrayEntrySize];
    CurrentFrame->OperandStack->PushRef(ArrayRef);
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
    // u2 index      = GetIndex2();
    // u1 dimensions =  
}

void Jvm::ifnull(){

}

void Jvm::ifnonnull(){

}

void Jvm::goto_w(){

}

void Jvm::jsr_w(){

}




