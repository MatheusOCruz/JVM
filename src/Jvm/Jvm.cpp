//jvm.c
// Created by matheus on 4/11/24.
//
// !todo: excluir prints , ctr f

#include "../../include/Jvm/Jvm.h"
#include <cmath>
#include <limits>
#include <unordered_set>



void Jvm::Run(){    
    std::cout<<"!Run\n";
    MethodArea = new std::unordered_map<std::string,class_file*>;
    Loader     = new ClassLoader(MethodArea);
    Loader->LoadClass(MainClass);
    CurrentClass = GetClass(MainClass);



    std::string MethodName = "<clinit>";
    if(GetMethod(MethodName)){
        GetCurrentMethodCode();
        NewFrame();
        CurrentClass->StaticFields = new std::unordered_map<std::string, FieldEntry*>;
        SaveFrameState();
        ExecBytecode();
    }

    MethodName = "main";
    GetMethod(MethodName);
    GetCurrentMethodCode();
    NewFrame();
    SaveFrameState();
    ExecBytecode();

}


/**
 * @brief Desmpilha um valor de 32 bits na pilha de operandos em big endian (mais significativos primeiro).
 *
 * Desempilha os 2 últimos valores na pilha de operandos e concatena-os
 * Método chamado em opcodes que desempilham valores cat. 2 (32 bits) na pilha de operandos: 
 * lastore, dastore, ladd... * 
 * 
 * @return valor de 32 bits
 */
u8 Jvm::popU8FromOpStack(){
    u4 LowBytes = CurrentFrame->OperandStack->Pop();
    u4 HighBytes = CurrentFrame->OperandStack->Pop();
    return (static_cast<u8>(HighBytes) << 32) | LowBytes;
}

/**
 * @brief Empilha um valor de 32 bits na pilha de operandos em big endian (mais significativos primeiro).
 *
 * Método chamado em opcodes que empurram valores cat. 2 (32 bits) na pilha de operandos, big endian (bits mais significativos primeiro): 
 * lconst_<i> , ldc2_w, lload, dload, lload_<i>
 * laload daload ladd dadd lsub dsub lmul ...
 * 
 * @param HighBytes bytes mais significativos do valor (empilhados primeiro)  
 * @param LowBytes bytes menos significativos do valor
 * @return void
 */
void Jvm::pushU8ToOpStack(u4 HighBytes, u4 LowBytes){
    std::cout<<"!pushU8ToOpStack\n";
    
    CurrentFrame->OperandStack->push(HighBytes);
    CurrentFrame->OperandStack->push(LowBytes);
}


/**
 * @brief Pega valor cat. 2 no índice 'index' das variáveis locais.
 * 
 * Concatena bytes mais significativos em váriaveis locais em `index` com os menos significativos em `index + 1`
 * Método chamado em opcodes que pegam valores cat. 2 do vetor de variáveis locais: lload, dload, lload_<i>
 * 
 * @param startingIndex índice dos bytes mais significativos (em big endian)
 * @return valor de 32 bits no índice 'index' das variáveis locais
 */
u8 Jvm::getU8FromLocalVars(u4 startingIndex){
    u4 LowBytes = (*CurrentFrame->localVariables)[startingIndex];
    u4 HighBytes = (*CurrentFrame->localVariables)[startingIndex + 1];
    return (static_cast<u8>(HighBytes) << 32) | LowBytes;
}

/**
 * @brief Realiza o parse do descritor de um método e retorna quantidade de elementos
 *
 * Itera pela string Descriptor para determinar quantos u4 devem ser transferidos da pilha de operandos
 * para o vetor de variáveis do método a ser chamado
 *
 * @param Descriptor String que descreve parametros de um método
 * @return quantidade de entradas que devem ser "popadas" da pilha
 */
int Jvm::numberOfEntriesFromString(const std::string & Descriptor) {
    if(Descriptor.empty())
        return 0;


    int number_of_attr = 0;
    const char* iter = Descriptor.c_str();
    do {
        char c = *iter++ ;
        switch (c) {
            case ')':

                break;

            case 'L':
                number_of_attr++;
                while(c!=';') c = *iter++;

                break;

            case 'B':
            case 'S':
            case 'I':
            case 'F':
            case 'C':
            case 'Z':
                number_of_attr++;
                break;

            case 'J':
            case 'D':
                number_of_attr += 2;
                break;

            case '[':
            case '(':
                break;

            default:
                std::cerr << "WARNING: "
                          << "Caracter " << c << " not recognized during attr parsing"
                          << std::endl;

                break;
        }
    } while(*iter);
    return number_of_attr;
}


inline u1 Jvm::NextCodeByte(){
return (*CurrentCode->code)[pc++];
}

/**
 *
 * @brief  Retorna entrada da constant pool no index em questão
 * @param  Index indice da constant pool
 * @return Entrada da constant pool no indice index
 */
inline cp_info* Jvm::GetConstantPoolEntryAt(u2 index){
    return (*CurrentClass->constant_pool)[index];
}

/**
 * Itera pelo atributo code e usa vetor de de funções para chamar
 * função responsável pelo bytecode em questão
 *
 */
void Jvm::ExecBytecode(){    
    std::cout<<"!ExecBytecode\n";
    while(pc < CurrentCode->code_length){
        u1 bytecode =  NextCodeByte();
        (this->*bytecodeFuncs[bytecode])();
    }
    PopFrameStack();
}

//salva valores atuais pro retorno ao frame
/**
 * Salva estado do frame atual
 */
void Jvm::SaveFrameState(){    
    std::cout<<"!SaveFrameState\n";
    CurrentFrame->nextPC      = pc;
    CurrentFrame->frameClass  = CurrentClass;
    CurrentFrame->frameMethod = CurrentMethod;
}

/**
 * Cria um novo Frame, o torna o frame atual e o empilha no FrameStack
 */
void Jvm::NewFrame(){    
    std::cout<<"!NewFrame\n";

    //instancia novo frame
    auto NewFrame = new Frame;
    NewFrame->localVariables = new std::vector<u4>;
    NewFrame->OperandStack   = new JVM::stack<u4>;

    u2 MaxLocals = CurrentCode->max_locals;
    NewFrame->localVariables->resize(MaxLocals);

    //substitui frame atual por novo frame
    FrameStack.push(NewFrame);
    CurrentFrame = NewFrame;
    pc = 0;

}
/**
 * @brief Desempilha frame e atualiza estado
 *
 * Desempilha o Frame no fim de sua execução, e atualiza o estado da
 * jvm para onde estava antes de chamar tal método, ou seja,
 * classe, método e pc
 */
void Jvm::PopFrameStack(){    
    std::cout<<"!PopFrameStack\n";

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


/**
 *  @brief Atualiza o método atual
 *
 *  Busca o método com o nome fornecido na classe atual, como o método pode não existir,
 *  no caso do método <clinit> retorna uma flag de confirmação caso tenha sido encontrado o método
 *
 * @param MethodName Nome do método que deve ser executado
 * @return caso o método existe, retorna 1, caso contrario 0
 */


int Jvm::GetMethod(const std::string& MethodName){

    for(auto Method: *CurrentClass->methods)
        if((*CurrentClass->constant_pool)[Method->name_index]->AsString() == MethodName){
            CurrentMethod = Method;
            return 1;
        }
    return 0;
}

/**
 * Atualiza o CurrentCode para o Code do CurrentMethod após mudança de frame
 */
void Jvm::GetCurrentMethodCode(){    
    std::cout<<"!GetCurrentMethodCode\n";

    for(auto Attribute : *CurrentMethod->attributes){
        u2 NameIndex = Attribute->attribute_name_index;
        cp_info* AttributeName = (*CurrentClass->constant_pool)[NameIndex];
        std::cout<<"!"<<AttributeName<<std::endl;
        if(AttributeName->AsString() == "Code"){
            CurrentCode = Attribute;
            return;
        }
    }
}

/**
 * @brief Garante que o class_file requerido será retornado.
 *
 * Verifica se a classe em questão já foi carregada. Se sim, retorna seu class_file. Caso contrário,
 * invoca o loader com a classe em questão e garante a inicialização estática por meio da função CheckStaticInit.
 *
 * @param class_name Nome da classe que será carregada ou retornada se já estiver carregada.
 * @return Ponteiro para o class_file referente à classe requerida.
 */
class_file* Jvm::GetClass(std::string class_name){

    if(MethodArea->find(class_name) == MethodArea->end()) {
        Loader->LoadClass(class_name);
        CheckStaticInit(class_name);
    }

    return (*MethodArea)[class_name];
}
/*
 TODO: verificar se reboco magico do -3 funciona pra outros casos de init como chamar funcao estatica
*/

/**
 * @brief Garante execuçaõ do método <clinit> das classes carregadas
 *
 * Método chamado após uma classe ser carregada durante execução, realiza método
 * <clinit> e retorna ao fluxo de execução
 *
 * @param class_name Nome da classe inicializada
 */
 void Jvm::CheckStaticInit(std::string class_name) {
    // apos o static, o new vai ser executado novamente
    // ent voltamos o pc pra antes dele (opcode + u2 index)
    pc-=3;

    SaveFrameState();
    NewFrame();
    CurrentClass = GetClass(class_name);
    CurrentClass->StaticFields = new std::unordered_map<std::string, FieldEntry*>;
    CurrentFrame->frameClass = CurrentClass;
    std::string MethodName = "<clinit>";
    if (GetMethod(MethodName)) {
        GetCurrentMethodCode();
        SaveFrameState();

    }
    else{
        PopFrameStack(); // retorna ao frame antigo
        pc+=3;
    }
}




// create n dimensional array
// dimension_counts contem tamanho de cada array
// de forma recusiva

/**
 * @brief retorna caso base do array
 *
 * Usa o tipo do array pra determinar a quantidade de bytes por elemento, então aloca
 * memória para o mesmo de acordo com a quantidade de elementos
 *
 * @param Type tipo final do array
 * @param count quantidade de elementos
 * @return ponteiro para memoria alocada para novo vetor
 */
void* NewBaseArray(ArrayTypeCode Type, int count){
    int byteSize = 0;
    switch (Type) {
        case ArrayTypeCode::T_BOOLEAN:
        case ArrayTypeCode::T_BYTE:{
            byteSize = 1;
            break;
        }
        case ArrayTypeCode::T_CHAR:
        case ArrayTypeCode::T_SHORT:{
            byteSize = 2;
            break;
        }
        case ArrayTypeCode::T_INT:
        case ArrayTypeCode::T_FLOAT:{
            byteSize = 4;
            break;
        }
        case ArrayTypeCode::T_DOUBLE:
        case ArrayTypeCode::T_LONG:
        case ArrayTypeCode::T_REF:
        case ArrayTypeCode::T_ARRAY:{
            byteSize = 8;
            break;
        }
    }
    return new char[count * byteSize]() ;
}

//TODO: quando uma dimensao qualquer for 0, retorna o array ate la(ta na spec)
/**
 *
 * @brief Cria instância de Arrayref encapsulado em uma Reference
 *
 * @param Type    código do tipo final a ser guardado no array
 * @param counts  pilha quantidade de elementos em cada dimensão
 * @param dims    quantidade de dimensões do array
 * @return        instância de Arrayref encapsulado em uma Reference
 */

Reference* NewArray(ArrayTypeCode Type, JVM::stack<int> counts, int dims){
    auto Array = new ArrayInstance;
    int count  = counts.Pop();
    Array->size = count;
    if(dims == 1){
        Array->ComponentType = Type;
        Array->DataVec       = NewBaseArray(Type, count);
    }
    else{
        Array->ComponentType = ArrayTypeCode::T_ARRAY;
        Array->ArrayVec      = new void*[count];

        for (int i = 0; i < count; ++i)
            Array->ArrayVec[i] = NewArray(Type, counts, dims - 1);
    }
    auto ArrayRef = new Reference;

    ArrayRef->Type     = ReferenceType::ArrayType;
    ArrayRef->ArrayRef = Array;

    return ArrayRef;
}

// todo comment
u4 Jvm::PopOpStack(){
return CurrentFrame->OperandStack->Pop();
}


// funcoes auxiliares pros bytecode

/**
 * le 2 bytes do code atual e forma um index u2
 * @return index u2 formado a partir do code
 */
u2 Jvm::GetIndex2() {
    // std::cout<<"GetIndex2\n";
    u1 indexbyte1 = (*CurrentCode->code)[pc++];
    u1 indexbyte2 = (*CurrentCode->code)[pc++];
    return (indexbyte1 << 8) | indexbyte2;
}
/**
 * Função genérica para return de cat1
 */
void Jvm::return_u4(){    
    std::cout<<"!return_u4\n";
    u4 ReturnValue = CurrentFrame->OperandStack->Pop();
    PopFrameStack();
    CurrentFrame->OperandStack->push(ReturnValue);
}
/**
 * Função genérica para return de cat2
 */
void Jvm::return_u8(){    
    std::cout<<"!return_u8\n";

    u4 ReturnValue1 = CurrentFrame->OperandStack->Pop();
    u4 ReturnValue2 = CurrentFrame->OperandStack->Pop();
    PopFrameStack();
    CurrentFrame->OperandStack->push(ReturnValue2);
    CurrentFrame->OperandStack->push(ReturnValue1);

}
/**
 * @brief Função genérica para instruções de invoke
 *
 * Guarda um ponteiro pra pilha de operandos do frame atual, então cria um novo frame
 * pro método a ser chamado (GetClass lida com classe não carregada), e carrega os argumentos (caso existam)
 * no vetor de variáveis locais do novo frame
 *
 * @param ClassName nome da classe "dona" do método invocado
 * @param MethodName nome do
 * @param Descriptor descritor dos argumentos recebidos pela função
 */
void Jvm::invoke(std::string ClassName, std::string MethodName, std::string Descriptor){
    std::cout<<"!invoke\n";
    auto CallerOperandStack = CurrentFrame->OperandStack;
    SaveFrameState();
    NewFrame();
    CurrentClass = GetClass(ClassName);
    CurrentFrame->frameClass = CurrentClass;
    GetMethod(MethodName);
    GetCurrentMethodCode();
    SaveFrameState();

    Descriptor = Descriptor.substr(1, Descriptor.find(')')-1);
    LoadLocalVariables(Descriptor, CallerOperandStack);
}


/**
 * @brief Implementação do Println do Java
 * @param MethodDescriptor usado para determinar o tipo de print a ser usado,
 * string, int, etc
 */
void Jvm::JavaPrint(std::string& MethodDescriptor) {
    std::cout<<"!JavaPrint\n";
    
    auto PrintType = MethodDescriptor.substr(1, MethodDescriptor.size() - 3);
    std::unordered_set<std::string> PrintAsInt = {"B", "S", "I"};
    if(PrintType.substr(0, 16) == "Ljava/lang/String;"){
        auto Output = reinterpret_cast<cp_info*>(CurrentFrame->OperandStack->Pop())->AsString(); // segfault:  reinterpret_cast<cp_info*>(CurrentFrame->OperandStack->Pop()) n é acessível
        std::cout<<Output;
    }
    else if(PrintAsInt.find(PrintType) != PrintAsInt.end()){
        int Output = CurrentFrame->OperandStack->Pop();
        std::cout<<Output;
    }
    else{

        auto Output = reinterpret_cast<cp_info*>(CurrentFrame->OperandStack->Pop())->AsString(); // segfault:  reinterpret_cast<cp_info*>(CurrentFrame->OperandStack->Pop()) n é acessível
        std::cout<<Output;
    }


}



// todo doxygen?
void Jvm::LoadLocalVariables(std::string& Descriptor, JVM::stack<u4> *CallerOperandStack) {
    std::cout<<"!LoadLocalVariables\n";
    int count = 0;
    if(!Descriptor.empty())
        count = numberOfEntriesFromString(Descriptor);
    do{
        u4 value = CallerOperandStack->Pop();
        (*CurrentFrame->localVariables)[count] = value;
    }while(count--);
}



// so bytecode




// Do nothing
void Jvm::nop() {
    std::cout<<"!nop\n";
    return;
}
// todo implement
// An aconst_null instruction is type safe if one can validly push the type null onto the incoming operand stack yielding the outgoing type state.
void Jvm::aconst_null(){    
    std::cout<<"!aconst_null\n";

}

// Push the int constant <i> (-1, 0, 1, 2, 3, 4 or 5) onto the operand
// stack.
// Notes Each of this family of instructions is equivalent to bipush <i> for
// the respective value of <i>, except that the operand <i> is implicit.
void Jvm::iconst_m1(){    
    std::cout<<"!iconst_m1\n";
    CurrentFrame->OperandStack->push(static_cast<const u4>(-1));
}

void Jvm::iconst_0(){    
    std::cout<<"!iconst_0\n";
    CurrentFrame->OperandStack->push(static_cast<const u4>(0));
}

void Jvm::iconst_1(){    
    std::cout<<"!iconst_1\n";
    CurrentFrame->OperandStack->push(static_cast<const u4>(1));
}

void Jvm::iconst_2(){    
    std::cout<<"!iconst_2\n";
    CurrentFrame->OperandStack->push(static_cast<const u4>(2));
}

void Jvm::iconst_3(){    
    std::cout<<"!iconst_3\n";
    CurrentFrame->OperandStack->push(static_cast<const u4>(3));
}

void Jvm::iconst_4(){    
    std::cout<<"!iconst_4\n";
    CurrentFrame->OperandStack->push(static_cast<const u4>(4));
}

void Jvm::iconst_5(){    
    std::cout<<"!iconst_5\n";
    CurrentFrame->OperandStack->push(static_cast<const u4>(5));
}

//!k
void Jvm::lconst_0(){    
    std::cout<<"!lconst_0\n";
    Cat2Value Value{};
    Value.AsLong = 0;
    CurrentFrame->OperandStack->push(Value.HighBytes);
    CurrentFrame->OperandStack->push(Value.HighBytes);

    pushU8ToOpStack(Value.HighBytes, Value.LowBytes);
}

//!k
void Jvm::lconst_1(){    
    std::cout<<"!lconst_1\n";
    Cat2Value Value{};
    Value.AsLong = 1.0;
    pushU8ToOpStack(Value.HighBytes, Value.LowBytes);
}


//!k
void Jvm::fconst_0(){    
    std::cout<<"!fconst_0\n";
    const float value = 0.0;

    CurrentFrame->OperandStack->push(static_cast<const u4>(value));
}

//!k
void Jvm::fconst_1(){    
    std::cout<<"!fconst_1\n";
    const float value = 1.0;

    CurrentFrame->OperandStack->push(static_cast<const u4>(value));
}

//!k
void Jvm::fconst_2(){    
    std::cout<<"!fconst_2\n";
    const float value = 2.0;

    CurrentFrame->OperandStack->push(static_cast<const u4>(value));

}


/**
 * @brief Função genérica para instruções de dconst
 *
 * Empurra constante double na pilha de operandos
 *
 * @param value constante a ser empilhada
 */
void Jvm::dconst(double value) {
    std::cout<<"!dconst\n";
    
    Cat2Value Value{};
    Value.AsDouble = value;
    pushU8ToOpStack(Value.HighBytes, Value.LowBytes);
}

void Jvm::dconst_0(){    
    std::cout<<"!dconst_0\n";
    dconst(0.0);
}

void Jvm::dconst_1(){    
    std::cout<<"!dconst_1\n";
    dconst(1.0);
}

// todo fix check if wrong
// The immediate byte is sign-extended to an int value. That value
// is pushed onto the operand stac
void Jvm::bipush(){    
    std::cout<<"!bipush\n";
    u1 value = (*CurrentCode->code)[pc++];

    CurrentFrame->OperandStack->push(static_cast<u4>(value));
}
// todo fix check if wrong
void Jvm::sipush(){    
    std::cout<<"!sipush\n";
    u4 value = GetIndex2();
    CurrentFrame->OperandStack->push(static_cast<u4>(value));
}
//!k
// tested, works
void Jvm::ldc(){    
    std::cout<<"!ldc\n";
    u1 index = (*CurrentCode->code)[pc++];
    cp_info* RunTimeConstant = (*CurrentClass->constant_pool)[index];
    switch (RunTimeConstant->tag) {
        case ConstantPoolTag::CONSTANT_Integer:
        case ConstantPoolTag::CONSTANT_Float:{
            u4 value = RunTimeConstant->bytes;
        }
        case ConstantPoolTag::CONSTANT_String:{
            void* StringRef = (*CurrentClass->constant_pool)[RunTimeConstant->string_index];
            CurrentFrame->OperandStack->push(reinterpret_cast<u4p>(StringRef));
        }
        default:{
            //deu ruim
        }
    }

}


// todo fix implement erradasso
// todo test
void Jvm::ldc_w(){    
    std::cout<<"!ldc_w\n";
    u4 value;
    u2 index = GetIndex2();
    cp_info* cpEntry = (*CurrentClass->constant_pool)[index];

    switch (cpEntry->tag) {
        case ConstantPoolTag::CONSTANT_Integer:
        case ConstantPoolTag::CONSTANT_Float:
            value = (*CurrentClass->constant_pool)[index]->bytes;
            CurrentFrame->OperandStack->push(value);
        case ConstantPoolTag::CONSTANT_String:{
            value = cpEntry->string_index;// todo test, casting u4 to u2 and theres asstring too
            CurrentFrame->OperandStack->push(value);
        }
        case ConstantPoolTag::CONSTANT_Class:{
            value = cpEntry->name_index;// todo test
            CurrentFrame->OperandStack->push(value);
        }
        case ConstantPoolTag::CONSTANT_MethodType:{
            value = cpEntry->name_index;// todo test
            CurrentFrame->OperandStack->push(value);
        }
        case ConstantPoolTag::CONSTANT_MethodHandle:{
            value = cpEntry->descriptor_index;// !todo fix test p sure this is wrong
            CurrentFrame->OperandStack->push(value);
        }
        default:{
            //deu ruim
        }
    }


}

//!k
// tested, works
void Jvm::ldc2_w(){    
    std::cout<<"!ldc2_w\n";
    // Cat2Value value{};
    u2 index = GetIndex2();

    cp_info* cpEntry = (*CurrentClass->constant_pool)[index];

    switch (cpEntry->tag) {
        case ConstantPoolTag::CONSTANT_Long:
        case ConstantPoolTag::CONSTANT_Double:{
            pushU8ToOpStack(cpEntry->high_bytes, cpEntry->low_bytes);
        }
        default:{
            //deu ruim
        }
    }


}
//!k
void Jvm::iload(){    
    std::cout<<"!iload\n";
    u1 index = (*CurrentCode->code)[pc++];
    u4 value = (*CurrentFrame->localVariables)[index];
    // no sign extension as it has to be pushed as an unsigned
    CurrentFrame->OperandStack->push(value);

}

//!k
void Jvm::lload(){    
    std::cout<<"!lload\n";
    u1 index = (*CurrentCode->code)[pc++]; 
    Cat2Value value{};

    value.AsLong = getU8FromLocalVars(index);

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}

// todo test
void Jvm::fload(){    
    std::cout<<"!fload\n";
    u1 index = (*CurrentCode->code)[pc++];
    float value = (*CurrentFrame->localVariables)[index];

    CurrentFrame->OperandStack->push(value);

}

void Jvm::dload(){    
    std::cout<<"!dload\n";
    u1 index = (*CurrentCode->code)[pc++];
    Cat2Value value{};

    value.AsDouble = getU8FromLocalVars(index);
    pushU8ToOpStack(value.HighBytes, value.LowBytes);
}

// todo test
void Jvm::aload(){    
    std::cout<<"!aload\n";
    u1 index = (*CurrentCode->code)[pc++];
    u4 objectref = (*CurrentFrame->localVariables)[index];
    CurrentFrame->OperandStack->push(objectref);
}

void Jvm::iload_0(){    
    std::cout<<"!iload_0\n";
    u4 value = (*CurrentFrame->localVariables)[0];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::iload_1(){    
    std::cout<<"!iload_1\n";
    u4 value = (*CurrentFrame->localVariables)[1];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::iload_2(){    
    std::cout<<"!iload_2\n";
    u4 value = (*CurrentFrame->localVariables)[2];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::iload_3(){    
    std::cout<<"!iload_3\n";
    u4 value = (*CurrentFrame->localVariables)[3];
    CurrentFrame->OperandStack->push(value);
}


void Jvm::lload_0(){    
    std::cout<<"!lload_0\n";
    Cat2Value value{};

    value.AsLong = getU8FromLocalVars(0);
    
    pushU8ToOpStack(value.HighBytes, value.LowBytes);
}



// Both <n> and <n>+1 must be indices into the local variable array
void Jvm::lload_1(){    
    std::cout<<"!lload_1\n";
    Cat2Value value{};

    value.AsLong = getU8FromLocalVars(1);

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::lload_2(){    
    std::cout<<"!lload_2\n";
    Cat2Value value{};

    value.AsLong = getU8FromLocalVars(2);

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::lload_3(){    
    std::cout<<"!lload_3\n";
    Cat2Value value{};

    value.AsLong = getU8FromLocalVars(3);

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::fload_0(){    
    std::cout<<"!fload_0\n";
    U4ToType value{};
    value.AsFloat = (*CurrentFrame->localVariables)[0];
    CurrentFrame->OperandStack->push(value.Bytes);

}




void Jvm::fload_1(){    
    std::cout<<"!fload_1\n";
    U4ToType value{};
    value.AsFloat = (*CurrentFrame->localVariables)[1];
    CurrentFrame->OperandStack->push(value.Bytes);

}




void Jvm::fload_2(){    
    std::cout<<"!fload_2\n";
    U4ToType value{};
    value.AsFloat = (*CurrentFrame->localVariables)[2];
    CurrentFrame->OperandStack->push(value.Bytes);

}




void Jvm::fload_3(){    
    std::cout<<"!fload_3\n";
    U4ToType value{};
    value.AsFloat = (*CurrentFrame->localVariables)[3];
    CurrentFrame->OperandStack->push(value.Bytes);

}

// pra dload_<n>
void Jvm::dload(uint index){   
    std::cout<<"!dload\n";
    Cat2Value value{};

    value.AsDouble = getU8FromLocalVars(index);
    pushU8ToOpStack(value.HighBytes, value.LowBytes);
}

// todo test
void Jvm::dload_0(){    
    std::cout<<"!dload_0\n";
    dload(0);
}




void Jvm::dload_1(){    
    std::cout<<"!dload_1\n";
    dload(1);
}




void Jvm::dload_2(){    
    std::cout<<"!dload_2\n";
    dload(2);
}




void Jvm::dload_3(){    
    std::cout<<"!dload_3\n";
    dload(3);
}



// tested, works
void Jvm::aload_0(){    
    std::cout<<"!aload_0\n";
    u4 value;
    value = (*CurrentFrame->localVariables)[0];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::aload_1(){    
    std::cout<<"!aload_1\n";
    u4 value;
    value = (*CurrentFrame->localVariables)[1];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::aload_2(){    
    std::cout<<"!aload_2\n";
    u4 value;
    value = (*CurrentFrame->localVariables)[2];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::aload_3(){    
    std::cout<<"!aload_3\n";
    u4 value;
    value = (*CurrentFrame->localVariables)[3];
    CurrentFrame->OperandStack->push(value);
}



void Jvm::iaload(){    
    std::cout<<"!iaload\n";
    u4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"iaload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"iaload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_INT) // todo runtime exception
        std::cerr<<"iaload: Array nao e de int\n";

    u4 value = reinterpret_cast<u4*>(Array->DataVec)[index];
    CurrentFrame->OperandStack->push(value);
}



// todo test
void Jvm::laload(){    
    std::cout<<"!laload\n";
    Cat2Value value;
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"laload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"laload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_LONG)
        std::cerr<<"laload: Array nao e de long\n";
    if( index > Array->size - 1) // todo ArrayIndexOutOfBoundsException
        std::cerr<<"laload ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    value.AsLong = reinterpret_cast<long*>(Array->DataVec)[index]; //todo load to datavec not arrayvec

    pushU8ToOpStack(value.HighBytes, value.LowBytes);


}



// todo test
void Jvm::faload(){    
    std::cout<<"!faload\n";
    u4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"faload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"faload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_FLOAT) // todo runtime exception
        std::cerr<<"faload: Array nao e de float\n";

    u4 value = reinterpret_cast<u4*>(Array->DataVec)[index];
    CurrentFrame->OperandStack->push(value);


}



// todo test
void Jvm::daload(){   
    std::cout<<"!daload\n";
    Cat2Value value;
    u4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"laload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"laload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_DOUBLE)
        std::cerr<<"laload: Array nao e de double\n";
    if( index > Array->size - 1) // todo ArrayIndexOutOfBoundsException
        std::cerr<<"laload ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    value.AsDouble = reinterpret_cast<double*>(Array->DataVec)[index];

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}


// todo implement fix erradasso Array->DataVec?
// todo test
void Jvm::aaload(){    
    std::cout<<"!aaload\n";
    u4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"aaload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"aaload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_REF and Array->ComponentType != ArrayTypeCode::T_ARRAY)
        std::cerr<<"aaload: Array nao e de ref\n";

    u4 value = reinterpret_cast<u4*>(Array->DataVec)[index];
    CurrentFrame->OperandStack->push(value);


}


// todo test
void Jvm::baload(){    
    std::cout<<"!baload\n";
    u4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"baload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"baload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_BYTE) // todo runtime exception
        std::cerr<<"baload: Array nao e de byte\n";

    u4 value = reinterpret_cast<u4*>(Array->DataVec)[index]; 
    CurrentFrame->OperandStack->push(value);
}



// todo test
void Jvm::caload(){    
    std::cout<<"!caload\n";
    u4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"caload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"caload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_CHAR) // todo runtime exception
        std::cerr<<"caload: Array nao e de char\n";

    u4 value = reinterpret_cast<u4*>(Array->DataVec)[index]; 
    CurrentFrame->OperandStack->push(value);

}



// todo test
void Jvm::saload(){    
    std::cout<<"!saload\n";
    u4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"saload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"saload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_SHORT) // todo runtime exception
        std::cerr<<"saload: Array nao e de short\n";

    u4 value = reinterpret_cast<u4*>(Array->DataVec)[index]; 
    CurrentFrame->OperandStack->push(value);

}



//!k
// todo: can b used w  wide 
//The istore opcode can be used in conjunction with the wide
// instruction (§wide) to access a local variable using a two-byte
// unsigned index
//!k
void Jvm::istore(){    
    std::cout<<"!istore\n";
    u1 index = (*CurrentCode->code)[pc++];
    u4 value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[index] = value;

}



//!k
void Jvm::lstore(){    
    std::cout<<"!lstore\n";
    u1 index = (*CurrentCode->code)[pc++];
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();
    // todo: required value set conversion in fstore dstore astore?

    (*CurrentFrame->localVariables)[index] = highBytes;
    (*CurrentFrame->localVariables)[index + 1] = lowBytes;

}




void Jvm::fstore(){    
    std::cout<<"!fstore\n";
    u1 index = (*CurrentCode->code)[pc++];
    u4 value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[index] = value;

}



//todo test, not casting to double here
void Jvm::dstore(){    
    std::cout<<"!dstore\n";
    u1 index = (*CurrentCode->code)[pc++];
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();

    (*CurrentFrame->localVariables)[index] = highBytes;
    (*CurrentFrame->localVariables)[index + 1] = lowBytes;

}



// todo ctest
void Jvm::astore(){    
    std::cout<<"!astore\n";
    u1 index = (*CurrentCode->code)[pc++];
    u4 objectref = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[index] = objectref;

}




//!k
void Jvm::istore_0(){    
    std::cout<<"!istore_0\n";
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[0] = value;
}




//!k
void Jvm::istore_1(){    
    std::cout<<"!istore_1\n";
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[1] = value;
}




//!k
void Jvm::istore_2(){    
    std::cout<<"!istore_2\n";
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[2] = value;
}




//!k
void Jvm::istore_3(){    
    std::cout<<"!istore_3\n";
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[3] = value;
}



//!k
void Jvm::lstore_0(){    
    std::cout<<"!lstore_0\n";
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();

    (*CurrentFrame->localVariables)[0] = highBytes;
    (*CurrentFrame->localVariables)[0 + 1] = lowBytes;


}



//!k
void Jvm::lstore_1(){    
    std::cout<<"!lstore_1\n";
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();

    (*CurrentFrame->localVariables)[1] = highBytes;
    (*CurrentFrame->localVariables)[1 + 1] = lowBytes;


}



//!k
void Jvm::lstore_2(){    
    std::cout<<"!lstore_2\n";
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();

    (*CurrentFrame->localVariables)[2] = highBytes;
    (*CurrentFrame->localVariables)[2 + 1] = lowBytes;

}



//!k
void Jvm::lstore_3(){    
    std::cout<<"!lstore_3\n";
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();

    (*CurrentFrame->localVariables)[3] = highBytes;
    (*CurrentFrame->localVariables)[3 + 1] = lowBytes;

}




void Jvm::fstore_0(){    
    std::cout<<"!fstore_0\n";
    u4 value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[0] = value;

}




void Jvm::fstore_1(){    
    std::cout<<"!fstore_1\n";
    u4 value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[1] = value;

}




void Jvm::fstore_2(){    
    std::cout<<"!fstore_2\n";
    u4 value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[2] = value;

}




void Jvm::fstore_3(){    
    std::cout<<"!fstore_3\n";
    u4 value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[3] = value;

}




void Jvm::dstore_0(){    
    std::cout<<"!dstore_0\n";
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();

    (*CurrentFrame->localVariables)[0] = highBytes;
    (*CurrentFrame->localVariables)[0 + 1] = lowBytes;

}



// tested, works
void Jvm::dstore_1(){    
    std::cout<<"!dstore_1\n";
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();

    (*CurrentFrame->localVariables)[1] = highBytes;
    (*CurrentFrame->localVariables)[1 + 1] = lowBytes;

    (*CurrentFrame->localVariables)[1] = highBytes;
    (*CurrentFrame->localVariables)[1 + 1] = lowBytes;
    
}




void Jvm::dstore_2(){    
    std::cout<<"!dstore_2\n";
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();

    (*CurrentFrame->localVariables)[2] = highBytes;
    (*CurrentFrame->localVariables)[2 + 1] = lowBytes;

}




void Jvm::dstore_3(){    
    std::cout<<"!dstore_3\n";
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();

    (*CurrentFrame->localVariables)[3] = highBytes;
    (*CurrentFrame->localVariables)[3 + 1] = lowBytes;

}




void Jvm::astore_0(){    
    std::cout<<"!astore_0\n";
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[0] = value;
}




void Jvm::astore_1(){    
    std::cout<<"!astore_1\n";
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[1] = value;
}




void Jvm::astore_2(){    
    std::cout<<"!astore_2\n";
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[2] = value;
}




void Jvm::astore_3(){    
    std::cout<<"!astore_3\n";
    u4 value;
    value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[3] = value;
}



// todo implement fix erradasso?
void Jvm::iastore(){    
    std::cout<<"!iastore\n";
    u4 value = CurrentFrame->OperandStack->Pop();
    u4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"iastore: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"iastore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_INT)
        std::cerr<<"iastore: Array nao e de int\n"; 
    if( index > Array->size - 1) // todo ArrayIndexOutOfBoundsException
        std::cerr<<"laload ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    reinterpret_cast<int*>(Array->DataVec)[index] = static_cast<int>(value);
}


// todo test
void Jvm::lastore(){    
    std::cout<<"!lastore\n";
    long long value = popU8FromOpStack();
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"lastore: referencia nao e para um array\n";
// todo exceptions
    ArrayInstance* Array = ArrayRef->ArrayRef; 
    if( index > Array->size - 1) // todo ArrayIndexOutOfBoundsException
        std::cerr<<"lastore ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    if(Array->ComponentType != ArrayTypeCode::T_LONG)
        std::cerr<<"lastore: Array nao e de long\n";

    reinterpret_cast<long long*>(Array->DataVec)[index] = (value);

}



// todo test
void Jvm::fastore(){    
    std::cout<<"!fastore\n";
    u4 value = CurrentFrame->OperandStack->Pop();
    u4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"fastore: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"fastore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_FLOAT)
        std::cerr<<"fastore: Array nao e de float\n"; 
    if( index > Array->size - 1) // todo ArrayIndexOutOfBoundsException
        std::cerr<<"fastore ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    reinterpret_cast<float*>(Array->DataVec)[index] = static_cast<float>(value);

}



// todo test
void Jvm::dastore(){    
    std::cout<<"!dastore\n";
    double value = popU8FromOpStack();
    u4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"dastore: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"dastore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef; 
    if( index > Array->size - 1) // todo ArrayIndexOutOfBoundsException
        std::cerr<<"dastore ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    if(Array->ComponentType != ArrayTypeCode::T_DOUBLE)
        std::cerr<<"dastore: Array nao e de double\n";

    reinterpret_cast<double*>(Array->DataVec)[index] = (value);

}



// todo test
void Jvm::aastore(){    
    std::cout<<"!aastore\n";
    u4 value = CurrentFrame->OperandStack->Pop();
    u4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"aastore: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"aastore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_ARRAY or Array->ComponentType != ArrayTypeCode::T_REF)
        std::cerr<<"aastore: Array nao e de ref\n";

    reinterpret_cast<u4*>(Array->DataVec)[index] = static_cast<int>(value);
}



// todo test
void Jvm::bastore(){    
    std::cout<<"!bastore\n";
    u4 value = CurrentFrame->OperandStack->Pop();
    u4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"bastore: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"bastore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_BYTE)
        std::cerr<<"bastore: Array nao e de byte\n"; 
    if( index > Array->size - 1) // todo ArrayIndexOutOfBoundsException
        std::cerr<<"bastore ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    reinterpret_cast<u1*>(Array->DataVec)[index] = static_cast<u1>(value);

}



// todo test
void Jvm::castore(){    
    std::cout<<"!castore\n";
    u4 value = CurrentFrame->OperandStack->Pop();
    u4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"castore: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"castore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_CHAR)
        std::cerr<<"castore: Array nao e de char\n"; 
    if( index > Array->size - 1) // todo ArrayIndexOutOfBoundsException
        std::cerr<<"castore ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    reinterpret_cast<char*>(Array->DataVec)[index] = static_cast<char>(value); //todo use u1 or s1

}



// todo test
void Jvm::sastore(){    
    std::cout<<"!sastore\n";
    u4 value = CurrentFrame->OperandStack->Pop();
    u4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) // todo runtime exception
        std::cerr<<"sastore: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"sastore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_SHORT)
        std::cerr<<"sastore: Array nao e de short\n"; 
    if( index > Array->size - 1) // todo ArrayIndexOutOfBoundsException
        std::cerr<<"sastore ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    reinterpret_cast<short*>(Array->DataVec)[index] = static_cast<short>(value); //todo use u1 or s1

}



// todo check if these pop require doxygen
void Jvm::pop(){    
    std::cout<<"!pop\n";
    CurrentFrame->OperandStack->pop();
}




void Jvm::pop2(){    
    std::cout<<"!pop2\n";
    CurrentFrame->OperandStack->pop();
    CurrentFrame->OperandStack->pop();
}



// tested, works
void Jvm::dup(){    
    std::cout<<"!dup\n";

    u4 value = CurrentFrame->OperandStack->Pop();

    CurrentFrame->OperandStack->push(value); // segfault
    CurrentFrame->OperandStack->push(value); // segfault

}




void Jvm::dup_x1(){    
    std::cout<<"!dup_x1\n";
    u4 value1 = CurrentFrame->OperandStack->Pop();
    u4 value2 = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value1);
    CurrentFrame->OperandStack->push(value2);
    CurrentFrame->OperandStack->push(value1);

}




// Form1 only
void Jvm::dup_x2(){    
    std::cout<<"!dup_x2\n";
    u4 value1 = CurrentFrame->OperandStack->Pop();
    u4 value2 = CurrentFrame->OperandStack->Pop();
    u4 value3 = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value1);
    CurrentFrame->OperandStack->push(value2);
    CurrentFrame->OperandStack->push(value3);
    CurrentFrame->OperandStack->push(value1);

}

// vals ok
// Form1 escolhida: both value1 and value2 are values of a category 1 computational type
void Jvm::dup2(){    
    std::cout<<"!dup2\n";
    u4 value1 = CurrentFrame->OperandStack->Pop();
    u4 value2 = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value2);
    CurrentFrame->OperandStack->push(value1);
    CurrentFrame->OperandStack->push(value2);
    CurrentFrame->OperandStack->push(value1);

}



//Form1 escolhida: all values are category 1 computational type
void Jvm::dup2_x1(){    
    std::cout<<"!dup2_x1\n";
    u4 value1 = CurrentFrame->OperandStack->Pop();
    u4 value2 = CurrentFrame->OperandStack->Pop();
    u4 value3 = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value2);
    CurrentFrame->OperandStack->push(value1);
    CurrentFrame->OperandStack->push(value3);
    CurrentFrame->OperandStack->push(value2);
    CurrentFrame->OperandStack->push(value1);

}




//Form1 escolhida: all values are category 1 computational type
void Jvm::dup2_x2(){    
    std::cout<<"!dup2_x2\n";
    u4 value1 = CurrentFrame->OperandStack->Pop();
    u4 value2 = CurrentFrame->OperandStack->Pop();
    u4 value3 = CurrentFrame->OperandStack->Pop();
    u4 value4 = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value2);
    CurrentFrame->OperandStack->push(value1);
    CurrentFrame->OperandStack->push(value4);
    CurrentFrame->OperandStack->push(value3);
    CurrentFrame->OperandStack->push(value2);
    CurrentFrame->OperandStack->push(value1);

}



// todo test
void Jvm::swap(){    
    std::cout<<"!swap\n";
    u4 value1 = CurrentFrame->OperandStack->Pop();
    u4 value2 = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value1);
    CurrentFrame->OperandStack->push(value2);
    //both cat 1, no need for u8 handling

}




void Jvm::iadd(){    
    std::cout<<"!iadd\n";
    s4 value1;
    s4 value2;
    value2 = static_cast<int>(CurrentFrame->OperandStack->Pop());
    value1 = static_cast<int>(CurrentFrame->OperandStack->Pop());
    s4 result = value1 + value2;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
}



//!k
void Jvm::ladd(){    
    std::cout<<"!ladd\n";
    Cat2Value value{};
    long value2 = popU8FromOpStack();
    long value1 = popU8FromOpStack();

    value.AsLong = value1 + value2;


    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}



// Both value1 and value2 must be of type float. The values are popped from the operand stack and undergo value set conversion (§2.8.3), resulting in value1' and value2'. The float result is value1' + value2'. The result is pushed onto the operand stack. The result of an fadd instruction is governed by the rules of IEEE arithmetic: • If either value1' or value2' is NaN, the result is NaN. • The sum of two infinities of opposite sign is NaN. • The sum of two infinities of the same sign is the infinity of that sign. • The sum of an infinity and any finite value is equal to the infinity. • The sum of two zeroes of opposite sign is positive zero. • The sum of two zeroes of the same sign is the zero of that sign. • The sum of a zero and a nonzero finite value is equal to the nonzero value. • The sum of two nonzero finite values of the same magnitude and opposite sign is positive zero. • In the remaining cases, where neither operand is an infinity, a zero, or NaN and the values have the same sign or have different magnitudes, the sum is computed and rounded to the nearest representable value using IEEE 754 round to nearest mode. If THE JAVA VIRTUAL MACHINE INSTRUCTION SET Instructions 6.5 421 the magnitude is too large to represent as a float, we say the operation overflows; the result is then an infinity of appropriate sign. If the magnitude is too small to represent as a float, we say the operation underflows; the result is then a zero of appropriate sign. The Java Virtual Machine requires support of gradual underflow as defined by IEEE 754. Despite the fact that overflow, underflow, or loss of precision may occur, execution of an fadd instruction never throws a run-time ex
void Jvm::fadd(){    
    std::cout<<"!fadd\n";

    float value1;
    float value2;
    value2 = static_cast<float>(CurrentFrame->OperandStack->Pop());
    value1 = static_cast<float>(CurrentFrame->OperandStack->Pop());

    float result = value1 + value2;

    CurrentFrame->OperandStack->push(static_cast<u4>(result));

}



// !todo: fix, erradasso
void Jvm::dadd(){    
    std::cout<<"!dadd\n";
    Cat2Value value{};
    double value2 = static_cast<double>(popU8FromOpStack());
    double value1 = static_cast<double>(popU8FromOpStack());

    value.AsDouble = value1 + value2;



    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}



//!k
void Jvm::isub(){    
    std::cout<<"!isub\n";
    s4 value1;
    s4 value2;
    value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 result = value1 - value2;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
//Despite the fact that overflow may occur, execution of an isub instruction never throws a run-time exception.
}




void Jvm::lsub(){    
    std::cout<<"!lsub\n";
    Cat2Value value1, value2, aux;
    long result;

    value2.AsLong = popU8FromOpStack();
    value1.AsLong = popU8FromOpStack();

    result = value1.AsLong - value2.AsLong;
    aux.HighBytes = result;
    aux.LowBytes = result;
    pushU8ToOpStack(aux.HighBytes, aux.LowBytes);

}




void Jvm::fsub(){    
    std::cout<<"!fsub\n";
    U4ToType value1, value2;
    float result;

    value2.AsFloat = CurrentFrame->OperandStack->Pop();
    value1.AsFloat = CurrentFrame->OperandStack->Pop();

    result = value1.AsFloat - value2.AsFloat;
    CurrentFrame->OperandStack->push(result);

}




void Jvm::dsub(){    
    std::cout<<"!dsub\n";
    Cat2Value value1, value2, aux;
    double result;

    value2.AsDouble = static_cast<double>(popU8FromOpStack());
    value1.AsDouble = static_cast<double>(popU8FromOpStack());

    result = value1.AsDouble - value2.AsDouble;
    aux.HighBytes = result;
    aux.LowBytes = result;
    pushU8ToOpStack(aux.HighBytes, aux.LowBytes);


}



//!k
void Jvm::imul(){    
    std::cout<<"!imul\n";
    s4 value1;
    s4 value2;

    value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    s4 result = value1 * value2;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
}



//!k
void Jvm::lmul(){    
    std::cout<<"!lmul\n";

    Cat2Value value1, value2, result;

    value2.AsLong = (popU8FromOpStack());
    value1.AsLong = (popU8FromOpStack());
// entendendo que cpp já segue  rules of IEEE arithmetic:
    result.AsLong = value1.AsLong * value2.AsLong;

    pushU8ToOpStack(result.HighBytes, result.LowBytes);
}




void Jvm::fmul(){    
    std::cout<<"!fmul\n";

    float value1;
    float value2;
    value2 = static_cast<float>(CurrentFrame->OperandStack->Pop());
    value1 = static_cast<float>(CurrentFrame->OperandStack->Pop());

    float result = value1 * value2;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));

}




void Jvm::dmul(){    
    std::cout<<"!dmul\n";

    Cat2Value value1, value2, aux;
    double result;

    value2.AsDouble = static_cast<double>(popU8FromOpStack());
    value1.AsDouble = static_cast<double>(popU8FromOpStack());
// entendendo que cpp já segue  rules of IEEE arithmetic:
    result = value1.AsDouble * value2.AsDouble;

    aux.HighBytes = result;
    aux.LowBytes = result;
    pushU8ToOpStack(aux.HighBytes, aux.LowBytes);

}




void Jvm::idiv(){    
    std::cout<<"!idiv\n";
    int value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    int value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    if (value2 == 0)
        throw ArithmeticException();
    s4 result = value1 / value2;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
}




void Jvm::ldiv(){    
    std::cout<<"!ldiv\n";

    Cat2Value value1, value2, result;

    value2.AsLong = popU8FromOpStack();
    value1.AsLong = popU8FromOpStack();

    if(value2.AsLong == 0)
        throw ArithmeticException();

// entendendo que cpp já segue  rules of IEEE arithmetic:
    result.AsLong = value1.AsLong / value2.AsLong;
    pushU8ToOpStack(result.HighBytes, result.LowBytes);

}




void Jvm::fdiv(){    
    std::cout<<"!fdiv\n";
    float value1, value2, result;

    value2 = CurrentFrame->OperandStack->Pop();
    value1 = CurrentFrame->OperandStack->Pop();
// entendendo que cpp já segue  rules of IEEE arithmetic:
// otherwise todo implement all those rules
    result = value1 / value2;
    CurrentFrame->OperandStack->push(result);
}




void Jvm::ddiv(){    
    std::cout<<"!ddiv\n";
    Cat2Value value1, value2, aux;
    double result;

    value2.AsLong = popU8FromOpStack();
    value1.AsLong = popU8FromOpStack();
// entendendo que cpp já segue  rules of IEEE arithmetic:
    value1.AsDouble = value1.AsDouble / value2.AsDouble;

    pushU8ToOpStack(value1.HighBytes, value1.LowBytes);

}



//!k
void Jvm::irem(){    
    std::cout<<"!irem\n";
    s4 value2 = CurrentFrame->OperandStack->Pop();
    s4 value1 = CurrentFrame->OperandStack->Pop();
    s4 result;

    result = value1 - (value1 / value2) * value2;
    // todo throw Run-time Exception
    // If the value of the divisor for an int remainder operator is 0, irem
// throws an ArithmeticException.

    CurrentFrame->OperandStack->push(static_cast<u4>(result));

}



//!k
void Jvm::lrem(){    
    std::cout<<"!lrem\n";
    Cat2Value value{};
    long value2 = popU8FromOpStack();
    long value1 = popU8FromOpStack();

    long result = value1 - (value1 / value2) * value2;

    value.AsLong = result;
    pushU8ToOpStack(value.HighBytes, value.LowBytes);

    // todo throw Run-time Exception


}




void Jvm::frem(){    
    std::cout<<"!frem\n";
    float value2 = CurrentFrame->OperandStack->Pop();
    float value1 = CurrentFrame->OperandStack->Pop();
    float result;

    value1 - (value1 / value2) * value2;
    // todo throw Run-time Exception

    CurrentFrame->OperandStack->push(result);


}




void Jvm::drem(){    
    std::cout<<"!drem\n";
    Cat2Value value{};
    double value2 = popU8FromOpStack();
    double value1 = popU8FromOpStack();

    double result = value1 - (value1 / value2) * value2;

    value.AsDouble = result;

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}



//!k
void Jvm::ineg(){    
    std::cout<<"!ineg\n";
    s4 value;
    value = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 result = - value;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
}



//!k
void Jvm::lneg(){    
    std::cout<<"!lneg\n";
    Cat2Value value{};
    value.AsLong = -1* popU8FromOpStack();

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}



//!k
void Jvm::fneg(){    
    std::cout<<"!fneg\n";
    float value = -1 * CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value);

}




void Jvm::dneg(){    
    std::cout<<"!dneg\n";
    Cat2Value value{};
    double result = -1* popU8FromOpStack();
    value.AsDouble = result;


    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}



//!k
void Jvm::ishl(){    
    std::cout<<"!ishl\n";
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    s4 result = value1 << (0x1F & value2);
    CurrentFrame->OperandStack->push(static_cast<u4>(result));

}



//!k
void Jvm::lshl(){    
    std::cout<<"!lshl\n";
    Cat2Value value{};
    long value2 = popU8FromOpStack();
    long value1 = popU8FromOpStack();

    value.AsLong = value1 << (0x3F & value2 );


    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}



//!k
void Jvm::ishr(){    
    std::cout<<"!ishr\n";
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    s4 result = value1 >> (0x1F & value2);
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
}


//!k
void Jvm::lshr(){    
    std::cout<<"!lshr\n";
    Cat2Value value{};
    long value2 = popU8FromOpStack();
    long value1 = popU8FromOpStack();

    value.AsLong = value1 >> (0x3F & value2 );


    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}

//!k
void Jvm::iushr(){    
    std::cout<<"!iushr\n";
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    s4 shift = value2 & 0x1F;
    u4 result = (value1) >> shift;

    CurrentFrame->OperandStack->push(result);

}




void Jvm::lushr(){    
    std::cout<<"!lushr\n";
    Cat2Value value{};
    int value2 = CurrentFrame->OperandStack->Pop();
    long value1 = popU8FromOpStack();

    value.AsLong = value1 >> (value2 & 0x3F);

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::iand(){    
    std::cout<<"!iand\n";

    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 result =  value1 & value2;

    CurrentFrame->OperandStack->push(static_cast<u4>(result));

}



//!k
void Jvm::land(){    
    std::cout<<"!land\n";
    Cat2Value value{};
    long value2 = popU8FromOpStack();
    long value1 = popU8FromOpStack();

    value.AsLong = value1 & value2;


    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::ior(){    
    std::cout<<"!ior\n";
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    s4 result =  value1 | value2;

    CurrentFrame->OperandStack->push(static_cast<u4>(result));


}



//!k
void Jvm::lor(){    
    std::cout<<"!lor\n";
    Cat2Value value{};
    long value2 = popU8FromOpStack();
    long value1 = popU8FromOpStack();

    value.AsLong = value1 | value2;


    pushU8ToOpStack(value.HighBytes, value.LowBytes);


}



//!k
void Jvm::ixor(){    
    std::cout<<"!ixor\n";
    s4 value2 = CurrentFrame->OperandStack->Pop();
    s4 value1 = CurrentFrame->OperandStack->Pop();

    s4 result =  value1 ^ value2;

    CurrentFrame->OperandStack->push(static_cast<u4>(result));

}




void Jvm::lxor(){    
    std::cout<<"!lxor\n";
    Cat2Value value{};
    long value2 = popU8FromOpStack();
    long value1 = popU8FromOpStack();

    value.AsLong = value1 ^ value2;


    pushU8ToOpStack(value.HighBytes, value.LowBytes);


}



// todo check
void Jvm::iinc(){ //todo pode ser alterado por wide, ver --
std::cout<<"!iinc\n";

    u1 index = (*CurrentCode->code)[pc++];
    s1 const_ = static_cast<s1>((*CurrentCode->code)[pc++]);
    (*CurrentFrame->localVariables)[index] += static_cast<int>(const_);

}



// idk todo test
void Jvm::i2l(){    
    std::cout<<"!i2l\n";
    Cat2Value value{};
    value.AsLong = (CurrentFrame->OperandStack->Pop());

    // push pro operand stack em big endian
    pushU8ToOpStack(value.HighBytes, value.LowBytes);
}




void Jvm::i2f(){    
    std::cout<<"!i2f\n";
    int value = static_cast<int>(CurrentFrame->OperandStack->Pop());
    float result = static_cast<float>(value);
    CurrentFrame->OperandStack->push(result);
}


//Converte int pra double
void Jvm::i2d(){    
    std::cout<<"!i2d\n";
    Cat2Value value{};

    value.AsDouble = CurrentFrame->OperandStack->Pop();


    // push em big endian
    CurrentFrame->OperandStack->push(value.HighBytes);
    CurrentFrame->OperandStack->push(value.HighBytes);
    CurrentFrame->OperandStack->push(value.LowBytes );
}



//!k
void Jvm::l2i(){    
    std::cout<<"!l2i\n";
    Cat2Value value{};

    value.AsLong = popU8FromOpStack();
    // push pro stack de operandos em big endian
    CurrentFrame->OperandStack->push(value.LowBytes);

}


//!k
// todo test
void Jvm::l2f(){    
    std::cout<<"!l2f\n";
    float value = popU8FromOpStack();

    CurrentFrame->OperandStack->push(static_cast<u4>(value));
}




void Jvm::l2d(){    
    std::cout<<"!l2d\n";
    Cat2Value value{};

    value.AsLong = popU8FromOpStack();
    value.AsDouble = value.AsLong;



    // push pro operand stack em big endian
    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::f2i(){    
    std::cout<<"!f2i\n";
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
void Jvm::f2l(){    
    std::cout<<"!f2l\n";
    //converte u4 pra float, float pra long
    Cat2Value value{};
    float valueFloat = (CurrentFrame->OperandStack->Pop());

    value.AsLong = valueFloat;

    if(std::isnan(value.AsLong)){
        value.HighBytes = 0;
        value.LowBytes = 0;

    } else if(  value.AsLong >= INT64_MAX ||  value.AsLong == std::numeric_limits<float>::infinity()){ // todo test these infinities
        // !todo test if this rlly makes int64 in high & low bytes
        value.LowBytes = static_cast<u4>((INT64_MAX & 0xffffffff00000000) >> 32);
        value.HighBytes = static_cast<u4>(INT64_MAX & 0x00000000ffffffff);

    } else if(  value.AsLong <= INT64_MIN ||  value.AsLong == -1 * std::numeric_limits<float>::infinity()) {
        value.LowBytes = static_cast<u4>((INT64_MIN & 0xffffffff00000000) >> 32);
        value.HighBytes = static_cast<u4>(INT64_MIN & 0x00000000ffffffff);

    } else {
    
    }

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}



// todo test
void Jvm::f2d(){    
    std::cout<<"!f2d\n";
    Cat2Value value{};

    value.AsDouble = (CurrentFrame->OperandStack->Pop());
    pushU8ToOpStack(value.HighBytes, value.LowBytes);
}


// todo fix ez
void Jvm::d2i(){    
    std::cout<<"!d2i\n";
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



// todo tesg
void Jvm::d2l(){    
    std::cout<<"!d2l\n";
    Cat2Value value{};

    value.AsDouble = popU8FromOpStack();
    value.AsLong =  value.AsDouble;

    // converte Long pra double
    if(std::isnan(value.AsLong)){
        value.HighBytes = 0;
        value.LowBytes = 0;
    } else if( value.AsLong >= __LONG_LONG_MAX__ || value.AsLong == std::numeric_limits<long long>::infinity()){ // todo test these infinities
        value.LowBytes = static_cast<u4>((__LONG_LONG_MAX__ & 0xffffffff00000000) >> 32);
        value.HighBytes = static_cast<u4>(__LONG_LONG_MAX__ & 0x00000000ffffffff);

    } else if( value.AsLong <=  std::numeric_limits<long long>::min() || value.AsLong == -1 * std::numeric_limits<long long>::infinity()) { // n achei min pra long long em cpp (que funcionasse) mas como vai virar unsigned p min tá 0. long specs: In Java SE 8 and later, you can use the long data type to represent an unsigned 64-bit long, which has a minimum value of 0 and a maximum value of 264-1
        value.LowBytes = static_cast<u4>((std::numeric_limits<long long>::min() & 0xffffffff00000000) >> 32);
        value.HighBytes = static_cast<u4>(std::numeric_limits<long long>::min() & 0x00000000ffffffff);

    } else { // IEEE 754 round towards zero mode: check if rounded to zero todo delete this comment
    
    }


}



// todo test
void Jvm::d2f(){    
    std::cout<<"!d2f\n";
    Cat2Value valDouble{};
    float valueFloat;

    valDouble.AsDouble = popU8FromOpStack();

    // converte Long pra double
    if(std::isnan(valDouble.AsDouble)){
        // float nan
        valueFloat = std::nanf("");
    } else if( valDouble.AsDouble >= __FLT_MAX__ || valDouble.AsDouble == std::numeric_limits<float>::infinity()){ // todo test these infinities
        valueFloat = std::numeric_limits<float>::max();
    } else if( valDouble.AsDouble <=  __FLT_MIN__ || valDouble.AsDouble == -1 * std::numeric_limits<float>::infinity()) { // n achei min pra long long em cpp (que funcionasse) mas como vai virar unsigned p min tá 0. long specs: In Java SE 8 and later, you can use the long data type to represent an unsigned 64-bit long, which has a minimum valDouble of 0 and a maximum valDouble of 264-1
        valueFloat = std::numeric_limits<float>::min();
    } else {
        valueFloat = valDouble.AsDouble;
    }

    CurrentFrame->OperandStack->push(valueFloat);

}

// todo check if this is fine w teacher
void Jvm::i2b(){    
    std::cout<<"!i2b\n";
   // transforma int pra byte mas pilha de operandos já está em byte
   return;
}




void Jvm::i2c(){    
    std::cout<<"!i2c\n";
    U4ToType value{};
    value.AsInt = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value.AsChar);

}



// short is signed int16_t (s2)
void Jvm::i2s(){    
    std::cout<<"!i2s\n";

    U4ToType value{};
    // trunca e faz sign extension
    value.AsShort = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value.AsShort);
}

//!k
// todo test all 3 cases
void Jvm::lcmp(){    
    std::cout<<"!lcmp\n";
    long long value2 = popU8FromOpStack();
    long long value1 = popU8FromOpStack();
    s4 intValue = 9;
    if(value1 > value2)
        intValue = 1;
    else if(value1 == value2)
        intValue = 0;
    else // (value1 < value2)
        intValue = -1;

    CurrentFrame->OperandStack->push(static_cast<u4>(intValue));

}




void Jvm::fcmpl(){    
    std::cout<<"!fcmpl\n";
    float value2 = CurrentFrame->OperandStack->Pop();
    float value1 = CurrentFrame->OperandStack->Pop();
    int intValue = 9;

    if(value1 > value2)
        intValue = 1;
    else if(value1 == value2)
        intValue = 0;
    else // (value1 < value2)
        intValue = -1;

    CurrentFrame->OperandStack->push(static_cast<u4>(intValue));


}




void Jvm::fcmpg(){    
    std::cout<<"!fcmpg\n";
    float value2 = CurrentFrame->OperandStack->Pop();
    float value1 = CurrentFrame->OperandStack->Pop();
    int intValue = 9;

    if(value1 > value2)
        intValue = 1;
    else if(value1 == value2)
        intValue = 0;
    else // (value1 < value2)
        intValue = -1;

    CurrentFrame->OperandStack->push(static_cast<u4>(intValue));

}



// todo mb delete these static casts
void Jvm::dcmpl(){    
    std::cout<<"!dcmpl\n";
    double value2 = static_cast<double>(popU8FromOpStack());
    double value1 = static_cast<double>(popU8FromOpStack());
    int intValue = 9;
    if(value1 > value2)
        intValue = 1;
    else if(value1 == value2)
        intValue = 0;
    else // (value1 < value2)
        intValue = -1;

    CurrentFrame->OperandStack->push(static_cast<u4>(intValue));

}




void Jvm::dcmpg(){    
    std::cout<<"!dcmpg\n";
    double value2 = static_cast<double>(popU8FromOpStack());
    double value1 = static_cast<double>(popU8FromOpStack());
    int intValue = 9;
    if(value1 > value2)
        intValue = 1;
    else if(value1 == value2)
        intValue = 0;
    else // (value1 < value2)
        intValue = -1;

    CurrentFrame->OperandStack->push(static_cast<u4>(intValue));

}




void Jvm::ifeq(){    
    std::cout<<"!ifeq\n";
    s4 value = static_cast<int>(CurrentFrame->OperandStack->Pop());

    if(value == 0){
        pc += GetIndex2();
    }

}




void Jvm::ifne(){    
    std::cout<<"!ifne\n";
    s4 value = static_cast<int>(CurrentFrame->OperandStack->Pop());

    if(value != 0){
        pc += GetIndex2();
    }


}




void Jvm::iflt(){    
    std::cout<<"!iflt\n";
    s4 value = static_cast<int>(CurrentFrame->OperandStack->Pop());

    if(value < 0){
        pc += GetIndex2();
    }


}




void Jvm::ifge(){    
    std::cout<<"!ifge\n";
    s4 value = static_cast<int>(CurrentFrame->OperandStack->Pop());

    if(value >= 0){
        pc += GetIndex2();
    }


}




void Jvm::ifgt(){    
    std::cout<<"!ifgt\n";
    s4 value = static_cast<int>(CurrentFrame->OperandStack->Pop());

    if(value > 0){
        pc += GetIndex2();
    }


}




void Jvm::ifle(){    
    std::cout<<"!ifle\n";
    s4 value = static_cast<int>(CurrentFrame->OperandStack->Pop());

    if(value <= 0){
        pc += GetIndex2();
    }


}




void Jvm::if_icmpeq(){    
    std::cout<<"!if_icmpeq\n";
    u2 offset = GetIndex2();
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    if(value1 == value2){
        pc += offset;
    }

}




void Jvm::if_icmpne(){    
    std::cout<<"!if_icmpne\n";
    u2 offset = GetIndex2();
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    if(value1 != value2){
        pc += offset;
    }

}




void Jvm::if_icmplt(){    
    std::cout<<"!if_icmplt\n";
    u2 offset = GetIndex2();
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    if(value1 < value2){
        pc += offset;
    }

}




void Jvm::if_icmpge(){    
    std::cout<<"!if_icmpge\n";
    u2 offset = GetIndex2();
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    if(value1 >= value2){
        pc += offset;
    }

}




void Jvm::if_icmpgt(){    
    std::cout<<"!if_icmpgt\n";
    u2 offset = GetIndex2();
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    if(value1 > value2){
        pc += offset;
    }

}




void Jvm::if_icmple(){    
    std::cout<<"!if_icmple\n";
    u2 offset = GetIndex2();
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    if(value1 <= value2){
        pc += offset;
    }

}




void Jvm::if_acmpeq(){    
    std::cout<<"!if_acmpeq\n";
    u2 offset = GetIndex2();
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    if(value1 == value2){
        pc += offset;
    }

}




void Jvm::if_acmpne(){    
    std::cout<<"!if_acmpne\n";
    u2 offset = GetIndex2();
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    if(value1 != value2){
        pc += offset;
    }

}




void Jvm::goto_(){    
    std::cout<<"!goto_\n";
    s2 branchoffset = GetIndex2();
    pc += branchoffset ;
}


// todo fix implement
//     The address of the opcode of the instruction immediately
// following this jsr instruction is pushed onto the operand stack as
// a value of type returnAddress.
void Jvm::jsr(){    
    std::cout<<"!jsr\n";
    u2 offset = GetIndex2();
    CurrentFrame->OperandStack->push(pc + offset);
    //nota: devia ter dado push como type returnAddress? n aconteceu


}




void Jvm::ret(){    
    std::cout<<"!ret\n";
    u1 index = (*CurrentCode->code)[pc++];
    u4 NewPc = (*CurrentFrame->localVariables)[index];
    pc = NewPc;
}

// todo implement
void Jvm::tableswitch() {
    std::cout<<"!tableswitch\n";
    
}

// todo implement
void Jvm::lookupswitch(){    
    std::cout<<"!lookupswitch\n";
}




void Jvm::ireturn(){    
    std::cout<<"!ireturn\n";
//check return == int
    return_u4();
}



//!k
void Jvm::lreturn(){    
    std::cout<<"!lreturn\n";
//check return == long
    return_u8();
}





void Jvm::freturn(){    
    std::cout<<"!freturn\n";

//check return == float
    return_u4();
}




void Jvm::dreturn(){    
    std::cout<<"!dreturn\n";
//check return == double
    return_u8();
}



 

void Jvm::areturn(){    
    std::cout<<"!areturn\n";
//check return == ref
    return_u4();
}

// todo doxygen
void Jvm::return_(){    
    std::cout<<"!return_\n";
//check return == void
if(FrameStack.size() != 1) // caso seja 1, ta no final da main ou clinit da main
	PopFrameStack();
}
// tested, works

void Jvm::getstatic(){    
    std::cout<<"!getstatic\n";
    u2 index = GetIndex2();
    cp_info* Fieldref     = GetConstantPoolEntryAt(index);
    cp_info* NameAndType  = GetConstantPoolEntryAt(Fieldref->name_and_type_index);
    auto Name      = GetConstantPoolEntryAt(NameAndType->name_index)->AsString();
    auto Descriptor= GetConstantPoolEntryAt(NameAndType->descriptor_index)->AsString();

    if(Descriptor == "Ljava/io/PrintStream;")
        return;

    class_file* Class = CurrentFrame->frameClass;
    auto StaticFields = *(Class->StaticFields);
    auto Entry  = StaticFields[Name];


    CurrentFrame->OperandStack->push(Entry->AsInt);

}
//TODO:  so pra int para testar

void Jvm::putstatic(){    
    std::cout<<"!putstatic\n";
    u2 index = GetIndex2();
    u4 value = PopOpStack();
    cp_info* Fieldref    = GetConstantPoolEntryAt(index);
    cp_info* NameAndType = (*CurrentClass->constant_pool)[Fieldref->name_and_type_index];
    auto Name     = GetConstantPoolEntryAt(NameAndType->name_index)->AsString();
    auto Descriptor= GetConstantPoolEntryAt(NameAndType->descriptor_index)->AsString();

    class_file* Class = CurrentFrame->frameClass;
    auto& StaticFields = *(Class->StaticFields);


    auto Entry =  new FieldEntry;
    Entry->AsInt = value;
    StaticFields[Name] = Entry;

}

void Jvm::getfield(){
    std::cout<<"!getfield\n";
    u2 index = GetIndex2();

    auto Fieldref    = GetConstantPoolEntryAt(index);
    auto NameAndType = GetConstantPoolEntryAt(Fieldref->name_and_type_index);

    auto Name            = (*CurrentClass->constant_pool)[NameAndType->name_index]->AsString();
    auto FieldDescriptor = (*CurrentClass->constant_pool)[NameAndType->descriptor_index]->AsString();

    auto ObjectRef = reinterpret_cast<Reference*>(PopOpStack());

    FieldEntry Value = (*ObjectRef->ClassRef->ObjectData)[Name];;
    if(FieldDescriptor == "D" or FieldDescriptor == "L") {
        Cat2Value CValue{};
        CValue.AsLong = Value.AsLong;
        CurrentFrame->OperandStack->push(CValue.HighBytes);
        CurrentFrame->OperandStack->push(CValue.LowBytes);
    }
    else
        CurrentFrame->OperandStack->push(Value.AsInt);







}


// todo implement, fix erradasso
void Jvm::new_(){    
    std::cout<<"!new_\n";
    //todo exceptions linking e run time
    u2 index = GetIndex2();
    cp_info* ref = GetConstantPoolEntryAt(index);

    // resolvendo se classe ou interface
    if(ref->tag == ConstantPoolTag::CONSTANT_Class){
        auto ClassName = GetConstantPoolEntryAt(ref->name_index)->AsString();
        //allocate memory to class by name

        auto ClassFile = GetClass(ClassName);
        auto ClassHandle = new Handle;
        ClassHandle->ClassObject = ClassFile;
        ClassHandle->MethodTable = ClassFile->methods;

        auto Object = new ClassInstance;
        Object->ClassHandle = ClassHandle;
        Object->ObjectData = new std::unordered_map<std::string, FieldEntry>;

        auto ObjectRef = new Reference;
        ObjectRef->Type = ReferenceType::ClassType;
        ObjectRef->ClassRef = Object;

        CurrentFrame->OperandStack->push(reinterpret_cast<u4>(ObjectRef));
    }
    else if(ref->tag == ConstantPoolTag::CONSTANT_InterfaceMethodref){
        cp_info* InterfaceNameAndType = (*CurrentClass->constant_pool)[ref->name_and_type_index];

        std::string Name            = (*CurrentClass->constant_pool)[InterfaceNameAndType->name_index]->AsString();
        std::string FieldDescriptor = (*CurrentClass->constant_pool)[InterfaceNameAndType->descriptor_index]->AsString();
        // !todo complete
    }
    else{
        std::cerr<<"new: CP_INFO nao e nem classe nem interface\n";
    }


}

void Jvm::putfield(){
    std::cout<<"!putfield\n";
    u2 index       = GetIndex2();

    auto Fieldref    = GetConstantPoolEntryAt(index);
    auto NameAndType = GetConstantPoolEntryAt(Fieldref->name_and_type_index);

    auto Name            = (*CurrentClass->constant_pool)[NameAndType->name_index]->AsString();
    auto FieldDescriptor = (*CurrentClass->constant_pool)[NameAndType->descriptor_index]->AsString();

    FieldEntry Value{};
    if(FieldDescriptor == "D" or FieldDescriptor == "L")
        Value.AsLong = popU8FromOpStack();
    else
    	Value.AsInt = PopOpStack();

    auto ObjectRef = reinterpret_cast<Reference*>(PopOpStack());
    (*ObjectRef->ClassRef->ObjectData)[Name] = Value;



} // tested, works
//TODO: to fazendo so pro hello world por enquanto

void Jvm::invokevirtual(){    
    std::cout<<"!invokevirtual\n";
    u2 index = GetIndex2();

    cp_info* MethodRef = (*CurrentClass->constant_pool)[index];
    auto ClassEntry = (GetConstantPoolEntryAt(MethodRef->class_index));
    auto ClassName = GetConstantPoolEntryAt(ClassEntry->name_index)->AsString();
    cp_info* NameAndType = (*CurrentClass->constant_pool)[MethodRef->name_and_type_index];

    // formato <valor>
    auto MethodName = (*CurrentClass->constant_pool)[NameAndType->name_index]->AsString();
    auto MethodDescriptor = (*CurrentClass->constant_pool)[NameAndType->descriptor_index]->AsString();
    if(MethodName == "println"){
        JavaPrint(MethodDescriptor);
        return;
    }

    invoke(ClassName, MethodName, MethodDescriptor);





}

// TODO: INSTANCIAS DE OUTRAS CLASSES (ta carregando o init da main)

void Jvm::invokespecial(){    
    std::cout<<"!invokespecial\n";
    u2 index = GetIndex2();
    auto MethodOrInterfaceMethod = GetConstantPoolEntryAt(index);

    auto Class = GetConstantPoolEntryAt(MethodOrInterfaceMethod->class_index);

    auto NameAndType = GetConstantPoolEntryAt(MethodOrInterfaceMethod->name_and_type_index);

    auto ClassName = GetConstantPoolEntryAt(Class->name_index)->AsString();
    if(ClassName == "java/lang/Object")
        return;

    auto Descriptor = GetConstantPoolEntryAt(NameAndType->descriptor_index)->AsString();
    //faz uma copia da pilha de operandos do frame atual pra carregar as variaveis locais na pilha nova


    invoke(ClassName, {"<init>"}, Descriptor);

}




void Jvm::invokestatic(){    
    std::cout<<"!invokestatic\n";
    auto a= reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());

}
// todo implement
void Jvm::invokeinterface(){    
    std::cout<<"!invokeinterface\n";
    u2 index = GetIndex2();
    auto InterfaceMethod = GetConstantPoolEntryAt(index);

    auto Class = GetConstantPoolEntryAt(InterfaceMethod->class_index);

    auto NameAndType = GetConstantPoolEntryAt(InterfaceMethod->name_and_type_index);

    auto ClassName = GetConstantPoolEntryAt(Class->name_index)->AsString();
    if(ClassName == "java/lang/Object"){
        std::cout<<"!early return in invokeinterface: lang/object\n";
        return;
        }
        

    auto Descriptor = GetConstantPoolEntryAt(NameAndType->descriptor_index)->AsString();
    //faz uma copia da pilha de operandos do frame atual pra carregar as variaveis locais na pilha nova


    invoke(ClassName, {"<init>"}, Descriptor);

}

// todo implement
void Jvm::invokedynamic(){    
    std::cout<<"!invokedynamic\n";

}



// }

void Jvm::newarray(){    
    std::cout<<"!newarray\n";

    auto Count = static_cast<int>(CurrentFrame->OperandStack->Pop());

    if(Count < 0)
        throw NegativeArraySizeException();

    u1 ArrayTypeCodeValue = (*CurrentCode->code)[pc++];

    u1 ArrayEntrySize;
    ArrayTypeCode Type = static_cast<ArrayTypeCode>(ArrayTypeCodeValue);
    switch (Type) {
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
        case ArrayTypeCode::T_INT:
        case ArrayTypeCode::T_REF:{
            ArrayEntrySize = 4;
            break;
        }

        case ArrayTypeCode::T_DOUBLE:
        case ArrayTypeCode::T_LONG:{
            ArrayEntrySize = 8;
            break;
        }
        case ArrayTypeCode::T_ARRAY:{
            std::cerr<<"Tipo final do array nao pode ser array\n";
            break;
        }

    }
    JVM::stack<int> Counts;
    Counts.push(Count);

    CurrentFrame->OperandStack->push(reinterpret_cast<u4p>(NewArray(Type, Counts, 1)));

}
// todo implement
void Jvm::anewarray(){    
    std::cout<<"!anewarray\n";

}
// todo implement
void Jvm::arraylength(){    
    std::cout<<"!arraylength\n";

}
// todo implement
// provalvelmente n precisa implementar
void Jvm::athrow(){    
    std::cout<<"!athrow\n";


}
// todo  implement
void Jvm::checkcast(){    
    std::cout<<"!checkcast\n";

}
// todo  implement
void Jvm::instanceof() {
    std::cout<<"!instanceof\n";
    

}

void Jvm::monitorenter(){    
    std::cout<<"!monitorenter\n";
    // região crítica, thread n é implementado
    return;

}

void Jvm::monitorexit(){    
    std::cout<<"!monitorexit\n";
    // região crítica, thread n é implementado
    return;

}
// todo implement
void Jvm::wide(){    
    std::cout<<"!wide\n";

}



 //TODO: vou usar 1 byte pra testar por enquanto, dps fazer pros outros; -> switch case do tipo

void Jvm::multianewarray(){    
    std::cout<<"!multianewarray\n";
    u2          index           = GetIndex2();
    u1          dimensions      = NextCodeByte();
    cp_info*    ArrayInfo       = GetConstantPoolEntryAt(index);
    std::string ArrayDiscriptor = GetConstantPoolEntryAt(ArrayInfo->name_index)->AsString();
    std::string ArrayType       = ArrayDiscriptor.substr(dimensions, ArrayDiscriptor.size());

    ArrayTypeCode Temp = ArrayTypeCode::T_INT;


    
    JVM::stack<int> sizes;

    // pega o tamanho de cada dimensao
    for (int i = 0; i < dimensions; ++i) {
        int count = static_cast<int>(CurrentFrame->OperandStack->Pop());
        sizes.push(count);
    }

    CurrentFrame->OperandStack->push(reinterpret_cast<u4p>(NewArray(Temp, sizes, dimensions)));
  }

// todo implement fix
void Jvm::ifnull(){    
    std::cout<<"!ifnull\n";
    auto value = reinterpret_cast<Reference*>(PopOpStack());
    if ( value == NULL){
        u2 offset = GetIndex2();
        // Execution then proceeds at that offset from the address of the opcode of this ifnonnull instruction
        // todo fix check if wrong 
        pc += offset;
    }

}
// todo implement fix
void Jvm::ifnonnull(){    
    std::cout<<"!ifnonnull\n";
    auto value = reinterpret_cast<Reference*>(PopOpStack());
    if ( value != NULL){
        u2 offset = GetIndex2();
        // Execution then proceeds at that offset from the address of the opcode of this ifnonnull instruction
        // todo fix check if wrong 
        pc += offset;
    }

}
//!c
void Jvm::goto_w(){    
    std::cout<<"!goto_w\n";
    u1 branchbyte1 = (*CurrentCode->code)[pc++];
    u1 branchbyte2 = (*CurrentCode->code)[pc++];
    u1 branchbyte3 = (*CurrentCode->code)[pc++];
    u1 branchbyte4 = (*CurrentCode->code)[pc++];
    s4 branchoffset = (branchbyte1 << 24) | (branchbyte2 << 16)| (branchbyte3 << 8) | branchbyte4;

    pc += branchoffset;
}

// todo implement fix
void Jvm::jsr_w(){    
    std::cout<<"!jsr_w\n";

    u2 offset = GetIndex2();
    CurrentFrame->OperandStack->push(pc + offset);
    //todo: push como type returnAddress? n aconteceu

}



