//jvm.c


// Created by matheus on 4/11/24.
//
// !todo: excluir prints , ctr f

#include "../../include/Jvm/Jvm.h"
#include <cmath>
#include <limits>
#include <unordered_set>



void Jvm::Run(){
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
    u4 LowBytes = (*CurrentFrame->localVariables)[startingIndex + 1];
    u4 HighBytes = (*CurrentFrame->localVariables)[startingIndex];
    u8 cat2 = ((u8)HighBytes << 32) | LowBytes;
    return static_cast<u8>(cat2);
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
    CurrentFrame->nextPC      = pc;
    CurrentFrame->frameClass  = CurrentClass;
    CurrentFrame->frameMethod = CurrentMethod;
}

/**
 * Cria um novo Frame, o torna o frame atual e o empilha no FrameStack
 */
void Jvm::NewFrame(){

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

//TODO: procurar nas interfaces da superclasse
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
    if(MethodName == "<clinit>")
        return 0;
    // procura nas superclasses
    auto* Class = CurrentClass;
    while(Class->super_class) {
        auto SuperEntry = (*Class->constant_pool)[Class->super_class];
        auto Name = (*Class->constant_pool)[SuperEntry->name_index]->AsString();
        auto* Class = GetClass(Name);
        for(auto Method: *Class->methods)
            if((*Class->constant_pool)[Method->name_index]->AsString() == MethodName){
                CurrentClass  = Class;
                CurrentMethod = Method;
                return 1;
            }
    }
    Class = CurrentClass;
    //bora procurar nas interfaces


    return 0;
}

/**
 * Atualiza o CurrentCode para o Code do CurrentMethod após mudança de frame
 */
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

void Jvm::LoadInterfaces(std::string class_name) {
    auto Class = GetClass(class_name);
    auto Interfaces = Class->interfaces;
    for(auto Interface: *Interfaces){
        auto ClassEntry = (*Class->constant_pool)[Interface];
        auto Name = (*Class->constant_pool)[ClassEntry->name_index]->AsString();
        GetClass(Name);
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
        LoadInterfaces(class_name);
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

    }
}


inline int Jvm::IsLoaded(std::string class_name) {
    return MethodArea->find(class_name) != MethodArea->end();
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
        case ArrayTypeCode::T_FLOAT:
        case ArrayTypeCode::T_REF:
        case ArrayTypeCode::T_ARRAY:
        {
            byteSize = 4;
            break;
        }
        case ArrayTypeCode::T_DOUBLE:
        case ArrayTypeCode::T_LONG:{
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


u4 Jvm::PopOpStack(){
    return CurrentFrame->OperandStack->Pop();
}


// funcoes auxiliares pros bytecode

/**
 * le 2 bytes do code atual e forma um index u2
 * @return index u2 formado a partir do code
 */
u2 Jvm::GetIndex2() {
    u1 indexbyte1 = (*CurrentCode->code)[pc++];
    u1 indexbyte2 = (*CurrentCode->code)[pc++];
    return (((u2) indexbyte1) << 8) | indexbyte2;
}
/**
 * Função genérica para return de cat1
 */
void Jvm::return_u4(){
    u4 ReturnValue = CurrentFrame->OperandStack->Pop();
    PopFrameStack();
    CurrentFrame->OperandStack->push(ReturnValue);
}
/**
 * Função genérica para return de cat2
 */
void Jvm::return_u8(){

    u4 ReturnValue1 = CurrentFrame->OperandStack->Pop();
    u4 ReturnValue2 = CurrentFrame->OperandStack->Pop();
    PopFrameStack();
    CurrentFrame->OperandStack->push(ReturnValue2);
    CurrentFrame->OperandStack->push(ReturnValue1);

}

u4 Jvm::read_u4() {
    u4 value = ((*CurrentCode->code)[pc] << 24) |
                     ((*CurrentCode->code)[pc + 1] << 16) |
                     ((*CurrentCode->code)[pc + 2] << 8) |
            (*CurrentCode->code)[pc + 3];
    pc += 4;
    return value;
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
void Jvm::invoke(std::string ClassName, std::string MethodName, std::string Descriptor, bool isStatic){
    auto CallerOperandStack = CurrentFrame->OperandStack;
    SaveFrameState();
    NewFrame();
    CurrentClass = GetClass(ClassName);
    CurrentFrame->frameClass = CurrentClass;
    GetMethod(MethodName);
    GetCurrentMethodCode();
    SaveFrameState();

    Descriptor = Descriptor.substr(1, Descriptor.find(')')-1);
    LoadLocalVariables(Descriptor, CallerOperandStack,isStatic);
}

void Jvm::LoadLocalVariables(std::string& Descriptor, JVM::stack<u4> *CallerOperandStack, bool isStatic) {
    int count = 0;
    if(!Descriptor.empty())
        count = numberOfEntriesFromString(Descriptor);
    if(!isStatic)
        count++; // objectref
    while(count--){
        u4 value = CallerOperandStack->Pop();
        (*CurrentFrame->localVariables)[count] = value;
    }
}


/**
 * @brief Implementação do Print do Java
 * @param MethodDescriptor usado para determinar o tipo de print a ser usado,
 * string, int, etc
 */
void Jvm::JavaPrint(std::string& MethodDescriptor) {

    auto PrintType = MethodDescriptor.substr(1, MethodDescriptor.size() - 3);
    if(PrintType == "Ljava/lang/String;"){
        auto Output = reinterpret_cast<cp_info*>(CurrentFrame->OperandStack->Pop())->AsString(); // segfault:  reinterpret_cast<cp_info*>(CurrentFrame->OperandStack->Pop()) n é acessível
        std::cout<<Output;
    }
    else if(PrintType == "I"){
		U4ToType output;
        output.UBytes = CurrentFrame->OperandStack->Pop();
        std::cout<<output.AsInt;
    }
    else if(PrintType == "S"){
		U4ToType output;
        output.UBytes = CurrentFrame->OperandStack->Pop();
        std::cout << output.AsShort;
    }
    else if(PrintType == "C"){
		U4ToType output;
        output.UBytes = CurrentFrame->OperandStack->Pop();
		wchar_t f = output.AsChar;
        std::wcout << f;
    }
    else if(PrintType == "B"){
		U4ToType output;
        output.UBytes = CurrentFrame->OperandStack->Pop();
        std::cout << output.AsByte;
    }
    else if(PrintType == "F"){
		U4ToType output;
        output.UBytes = CurrentFrame->OperandStack->Pop();
        std::cout << output.AsFloat;
    }
    else if(PrintType == "D"){
        Cat2Value Output;
        Output.Bytes = popU8FromOpStack();
        std::cout << Output.AsDouble;
    }
    else if(PrintType == "J"){
        Cat2Value Output;
        Output.Bytes = popU8FromOpStack();
        std::cout << Output.AsLong;
    }
}






// so bytecode




// Do nothing
void Jvm::nop() {
    return;
}

void Jvm::aconst_null(){

}


void Jvm::iconst_m1(){
    CurrentFrame->OperandStack->push(static_cast<const u4>(-1));
}

void Jvm::iconst_0(){
    s4 const value  = 0;
    CurrentFrame->OperandStack->push(static_cast<const u4>(value));
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
    Cat2Value Value{};
    Value.AsLong = 0;
    CurrentFrame->OperandStack->push(Value.HighBytes);
    CurrentFrame->OperandStack->push(Value.HighBytes);

    pushU8ToOpStack(Value.HighBytes, Value.LowBytes);
}


void Jvm::lconst_1(){    
    Cat2Value Value{};
    Value.AsLong = 1;
    pushU8ToOpStack(Value.HighBytes, Value.LowBytes);
}



void Jvm::fconst_0(){    
	U4ToType value{};
	value.AsFloat = 0.0;
    CurrentFrame->OperandStack->push(static_cast<const u4>(value.UBytes));
}


void Jvm::fconst_1(){    
	U4ToType value{};
	value.AsFloat = 1.0;
    CurrentFrame->OperandStack->push(static_cast<const u4>(value.UBytes));
}


void Jvm::fconst_2(){    
	U4ToType value{};
	value.AsFloat = 2.0;
    CurrentFrame->OperandStack->push(static_cast<const u4>(value.UBytes));
}


/**
 * @brief Função genérica para instruções de dconst
 *
 * Empurra constante double na pilha de operandos
 *
 * @param value constante a ser empilhada
 */
void Jvm::dconst(double value) {

    Cat2Value Value{};
    Value.AsDouble = value;
    pushU8ToOpStack(Value.HighBytes, Value.LowBytes);
}

void Jvm::dconst_0(){
    dconst(0.0);
}

void Jvm::dconst_1(){
    dconst(1.0);
}


void Jvm::bipush(){
    s1 immediate = (*CurrentCode->code)[pc++];
    s4 value = immediate;


    CurrentFrame->OperandStack->push(static_cast<s4>(value));
}

void Jvm::sipush(){    
    s2 intermediate = GetIndex2();
    s4 value = intermediate;
    CurrentFrame->OperandStack->push(static_cast<u4>(value));

}



void Jvm::ldc(){
    u1 index = (*CurrentCode->code)[pc++];
    cp_info* RunTimeConstant = (*CurrentClass->constant_pool)[index];

    switch (RunTimeConstant->tag) {
        case ConstantPoolTag::CONSTANT_Integer:
        case ConstantPoolTag::CONSTANT_Float:{
            u4 value = RunTimeConstant->bytes;
            CurrentFrame->OperandStack->push(value);
			break;
        }
        case ConstantPoolTag::CONSTANT_String:{
            void* StringRef = (*CurrentClass->constant_pool)[RunTimeConstant->string_index];
            CurrentFrame->OperandStack->push(reinterpret_cast<u4p>(StringRef));
			break;
        }
        default:{
            //deu ruim
        }
    }

}




void Jvm::ldc_w(){
    u4 value;
    u2 index = GetIndex2();
    cp_info* cpEntry = (*CurrentClass->constant_pool)[index];

    switch (cpEntry->tag) {
        case ConstantPoolTag::CONSTANT_Integer:
        case ConstantPoolTag::CONSTANT_Float:
            value = (*CurrentClass->constant_pool)[index]->bytes;
            CurrentFrame->OperandStack->push(value);
        case ConstantPoolTag::CONSTANT_String:{
            value = cpEntry->string_index;
            CurrentFrame->OperandStack->push(value);
        }
        case ConstantPoolTag::CONSTANT_Class:{
            value = cpEntry->name_index;
            CurrentFrame->OperandStack->push(value);
        }
        case ConstantPoolTag::CONSTANT_MethodType:{
            value = cpEntry->name_index;
            CurrentFrame->OperandStack->push(value);
        }
        case ConstantPoolTag::CONSTANT_MethodHandle:{
            value = cpEntry->descriptor_index;
            CurrentFrame->OperandStack->push(value);
        }
        default:{
            //deu ruim
        }
    }


}



void Jvm::ldc2_w(){
    u2 index = GetIndex2();

    cp_info* cpEntry = (*CurrentClass->constant_pool)[index];

    switch (cpEntry->tag) {
        case ConstantPoolTag::CONSTANT_Long:
        case ConstantPoolTag::CONSTANT_Double:{
            pushU8ToOpStack(cpEntry->high_bytes, cpEntry->low_bytes);
			break;
        }
        default:{
            //deu ruim
        }
    }


}

void Jvm::iload(){    
    u1 index = (*CurrentCode->code)[pc++];
    u4 value = (*CurrentFrame->localVariables)[index];
    CurrentFrame->OperandStack->push(value);

}


void Jvm::lload(){    
    u1 index = (*CurrentCode->code)[pc++]; 
    Cat2Value value{};

    value.AsLong = getU8FromLocalVars(index);

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}


void Jvm::fload(){
    u1 index = (*CurrentCode->code)[pc++];
    u4 value = (*CurrentFrame->localVariables)[index];
    CurrentFrame->OperandStack->push(value);

}


void Jvm::dload(){
    u1 index = (*CurrentCode->code)[pc++];
    Cat2Value value{};
    value.Bytes = getU8FromLocalVars(index);
    pushU8ToOpStack(value.HighBytes, value.LowBytes);
}


void Jvm::aload(){    
    u1 index = (*CurrentCode->code)[pc++];
    u4 objectref = (*CurrentFrame->localVariables)[index];
    CurrentFrame->OperandStack->push(objectref);
}



void Jvm::iload_0(){    
    s4 value = (*CurrentFrame->localVariables)[0];
    CurrentFrame->OperandStack->push(static_cast<u4>(value));
}




void Jvm::iload_1(){    
    s4 value = (*CurrentFrame->localVariables)[1];
    CurrentFrame->OperandStack->push(static_cast<u4>(value));
}




void Jvm::iload_2(){    
    s4 value = (*CurrentFrame->localVariables)[2];
    CurrentFrame->OperandStack->push(static_cast<u4>(value));
}




void Jvm::iload_3(){    
    s4 value = (*CurrentFrame->localVariables)[3];
    CurrentFrame->OperandStack->push(static_cast<u4>(value));
}


void Jvm::lload_0(){
    Cat2Value value{};

    value.AsLong = getU8FromLocalVars(0);

    pushU8ToOpStack(value.HighBytes, value.LowBytes);
}




void Jvm::lload_1(){
    Cat2Value value{};

    value.AsLong = getU8FromLocalVars(1);

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::lload_2(){
    Cat2Value value{};

    value.AsLong = getU8FromLocalVars(2);

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::lload_3(){
    Cat2Value value{};

    value.AsLong = getU8FromLocalVars(3);

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}

void Jvm::fload_0(){
    u4 value = (*CurrentFrame->localVariables)[0];
    CurrentFrame->OperandStack->push(value);
}

void Jvm::fload_1(){
    u4 value = (*CurrentFrame->localVariables)[1];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::fload_2(){
    u4 value = (*CurrentFrame->localVariables)[2];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::fload_3(){
    u4 value = (*CurrentFrame->localVariables)[3];
    CurrentFrame->OperandStack->push(value);
}

/**
 * @brief Pra implementar dload_<n>
 * 
 * implementa dload_<n> para o n (0,1,2,3) recebido
 * @param index índice pra colocar o double nas varáveis locais
 * @return void
 */
void Jvm::dload(uint index){
    Cat2Value value{};
    value.Bytes = getU8FromLocalVars(index);
    pushU8ToOpStack(value.HighBytes, value.LowBytes);
}


void Jvm::dload_0(){
    dload(0);
}




void Jvm::dload_1(){
    dload(1);
}




void Jvm::dload_2(){
    dload(2);
}




void Jvm::dload_3(){
    dload(3);
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
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) 
        std::cerr<<"iaload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"iaload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_INT) 
        std::cerr<<"iaload: Array nao e de int\n";

    u4 value = reinterpret_cast<u4*>(Array->DataVec)[index];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::laload(){
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) 
        std::cerr<<"laload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"laload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_LONG)
        std::cerr<<"laload: Array nao e de long\n";
    if( index > Array->size - 1) 
        std::cerr<<"laload ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    Cat2Value value{};
    value.AsLong = reinterpret_cast<s8*>(Array->DataVec)[index]; 

    pushU8ToOpStack(value.HighBytes, value.LowBytes);


}



void Jvm::faload(){
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) 
        std::cerr<<"faload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"faload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_FLOAT) 
        std::cerr<<"faload: Array nao e de float\n";

    u4 value = reinterpret_cast<u4*>(Array->DataVec)[index];
    CurrentFrame->OperandStack->push(value);


}




void Jvm::daload(){
    Cat2Value value{};
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) 
        std::cerr<<"laload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"laload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_DOUBLE)
        std::cerr<<"laload: Array nao e de double\n";
    if( index > Array->size - 1) 
        std::cerr<<"laload ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    value.AsDouble = reinterpret_cast<double*>(Array->DataVec)[index];

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::aaload(){
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());

    if(ArrayRef == NULL ) 
        std::cerr<<"aaload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"aaload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_REF and Array->ComponentType != ArrayTypeCode::T_ARRAY)
        std::cerr<<"aaload: Array nao e de ref\n";

    u4 value = reinterpret_cast<u4*>(Array->DataVec)[index];
    CurrentFrame->OperandStack->push(value);


}



void Jvm::baload(){
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) 
        std::cerr<<"baload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"baload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_BYTE) 
        std::cerr<<"baload: Array nao e de byte\n";

    s1 value = reinterpret_cast<s1*>(Array->DataVec)[index];
    CurrentFrame->OperandStack->push(value);
}




void Jvm::caload(){
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) 
        std::cerr<<"caload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"caload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_CHAR) 
        std::cerr<<"caload: Array nao e de char\n";

    u2 value = reinterpret_cast<u2*>(Array->DataVec)[index];
    CurrentFrame->OperandStack->push(value);

}




void Jvm::saload(){
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) 
        std::cerr<<"saload: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"saload: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_SHORT) 
        std::cerr<<"saload: Array nao e de short\n";

    s2 value = reinterpret_cast<s2*>(Array->DataVec)[index];
    CurrentFrame->OperandStack->push(value);

}



void Jvm::istore(){    
    u1 index = (*CurrentCode->code)[pc++];
    s4 value = CurrentFrame->OperandStack->Pop(); 
    (*CurrentFrame->localVariables)[index] = value;

}



void Jvm::lstore(){    
    u1 index = (*CurrentCode->code)[pc++];
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();
    

    (*CurrentFrame->localVariables)[index] = highBytes;
    (*CurrentFrame->localVariables)[index + 1] = lowBytes;

}




void Jvm::fstore(){
    u1 index = (*CurrentCode->code)[pc++];
    u4 value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[index] = value;
}



void Jvm::dstore(){
    u1 index = (*CurrentCode->code)[pc++];

    Cat2Value value{};
    value.Bytes = popU8FromOpStack();

    (*CurrentFrame->localVariables)[index] = value.HighBytes;
    (*CurrentFrame->localVariables)[index + 1] = value.LowBytes;

}




void Jvm::astore(){
    u1 index = (*CurrentCode->code)[pc++];
    u4 objectref = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[index] = objectref;

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




void Jvm::lstore_0(){    
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();

    (*CurrentFrame->localVariables)[0] = highBytes;
    (*CurrentFrame->localVariables)[0 + 1] = lowBytes;


}




void Jvm::lstore_1(){    
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();

    (*CurrentFrame->localVariables)[1] = highBytes;
    (*CurrentFrame->localVariables)[1 + 1] = lowBytes;


}




void Jvm::lstore_2(){    
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();

    (*CurrentFrame->localVariables)[2] = highBytes;
    (*CurrentFrame->localVariables)[2 + 1] = lowBytes;

}




void Jvm::lstore_3(){    
    u4 lowBytes = CurrentFrame->OperandStack->Pop();
    u4 highBytes = CurrentFrame->OperandStack->Pop();

    (*CurrentFrame->localVariables)[3] = highBytes;
    (*CurrentFrame->localVariables)[3 + 1] = lowBytes;

}




void Jvm::fstore_0(){
    U4ToType value;

    value.AsFloat = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[0] = value.UBytes;

}




void Jvm::fstore_1(){
    u4 value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[1] = value;
}




void Jvm::fstore_2(){
    u4 value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[2] = value;
}




void Jvm::fstore_3(){
    u4 value = CurrentFrame->OperandStack->Pop();
    (*CurrentFrame->localVariables)[3] = value;
}




void Jvm::dstore_0(){
    Cat2Value value{};
    value.Bytes = popU8FromOpStack();

    (*CurrentFrame->localVariables)[0] = value.HighBytes;
    (*CurrentFrame->localVariables)[0 + 1] = value.LowBytes;

}



void Jvm::dstore_1(){
    Cat2Value value{};
    value.Bytes = popU8FromOpStack();

    (*CurrentFrame->localVariables)[1] = value.HighBytes;
    (*CurrentFrame->localVariables)[1 + 1] = value.LowBytes;

}




void Jvm::dstore_2(){
    Cat2Value value{};
    value.Bytes = popU8FromOpStack();

    (*CurrentFrame->localVariables)[2] = value.HighBytes;
    (*CurrentFrame->localVariables)[2 + 1] = value.LowBytes;

}




void Jvm::dstore_3() {
    Cat2Value value{};
    value.Bytes = popU8FromOpStack();
    (*CurrentFrame->localVariables)[3] = value.HighBytes;
    (*CurrentFrame->localVariables)[3 + 1] = value.LowBytes;
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
    s4 value = CurrentFrame->OperandStack->Pop();
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) 
        std::cerr<<"iastore: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"iastore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_INT)
        std::cerr<<"iastore: Array nao e de int\n";
    if( index > Array->size - 1) 
        std::cerr<<"laload ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    reinterpret_cast<s4*>(Array->DataVec)[index] = static_cast<s4>(value);
}



void Jvm::lastore(){
    s8 value = popU8FromOpStack();
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"lastore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef; 
    if( index > Array->size - 1) 
        std::cerr<<"lastore ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    if(Array->ComponentType != ArrayTypeCode::T_LONG)
        std::cerr<<"lastore: Array nao e de long\n";

    reinterpret_cast<s8*>(Array->DataVec)[index] = (value);

}




void Jvm::fastore(){
	U4ToType value;
    value.UBytes = CurrentFrame->OperandStack->Pop();
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) 
        std::cerr<<"fastore: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"fastore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_FLOAT)
        std::cerr<<"fastore: Array nao e de float\n";
    if( index > Array->size - 1) 
        std::cerr<<"fastore ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";
    reinterpret_cast<float*>(Array->DataVec)[index] = value.AsFloat;

}




void Jvm::dastore(){
	Cat2Value value;
    value.Bytes = popU8FromOpStack();
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) 
        std::cerr<<"dastore: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"dastore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;
    if( index > Array->size - 1) 
        std::cerr<<"dastore ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    if(Array->ComponentType != ArrayTypeCode::T_DOUBLE)
        std::cerr<<"dastore: Array nao e de double\n";

    reinterpret_cast<double*>(Array->DataVec)[index] = (value.AsDouble);
}

void Jvm::aastore(){
    u4 value = CurrentFrame->OperandStack->Pop();
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) 
        std::cerr<<"aastore: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"aastore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_ARRAY and Array->ComponentType != ArrayTypeCode::T_REF)
        std::cerr<<"aastore: Array nao e de ref\n";

    reinterpret_cast<u4*>(Array->DataVec)[index] = static_cast<int>(value);
}

void Jvm::bastore(){
    s4 value = CurrentFrame->OperandStack->Pop();
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) 
        std::cerr<<"bastore: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"bastore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_BYTE)
        std::cerr<<"bastore: Array nao e de byte\n";
    if( index > Array->size - 1) 
        std::cerr<<"bastore ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    reinterpret_cast<s1*>(Array->DataVec)[index] = static_cast<s1>(value);

}




void Jvm::castore(){
    s4 value = CurrentFrame->OperandStack->Pop();
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) 
        std::cerr<<"castore: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"castore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_CHAR)
        std::cerr<<"castore: Array nao e de char\n";
    if( index > Array->size - 1) 
        std::cerr<<"castore ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    reinterpret_cast<u2*>(Array->DataVec)[index] = static_cast<u2>(value); 
}




void Jvm::sastore(){
    s4 value = CurrentFrame->OperandStack->Pop();
    s4 index = CurrentFrame->OperandStack->Pop();

    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    if(ArrayRef == NULL ) 
        std::cerr<<"sastore: referencia nao é acessível\n";
    if(ArrayRef->Type != ReferenceType::ArrayType)
        std::cerr<<"sastore: referencia nao e para um array\n";

    ArrayInstance* Array = ArrayRef->ArrayRef;

    if(Array->ComponentType != ArrayTypeCode::T_SHORT)
        std::cerr<<"sastore: Array nao e de short\n";
    if( index > Array->size - 1) 
        std::cerr<<"sastore ArrayIndexOutOfBoundsException: Index do value além do escopo do array\n";

    reinterpret_cast<s2*>(Array->DataVec)[index] = static_cast<s2>(value); //todo use u1 or s1
}




void Jvm::pop(){    
    CurrentFrame->OperandStack->pop();
}




void Jvm::pop2(){
    CurrentFrame->OperandStack->pop();
    CurrentFrame->OperandStack->pop();
}



void Jvm::dup(){

    u4 value = CurrentFrame->OperandStack->Pop();

    CurrentFrame->OperandStack->push(value); 
    CurrentFrame->OperandStack->push(value); 

}




void Jvm::dup_x1(){
    u4 value1 = CurrentFrame->OperandStack->Pop();
    u4 value2 = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value1);
    CurrentFrame->OperandStack->push(value2);
    CurrentFrame->OperandStack->push(value1);

}





void Jvm::dup_x2(){
    u4 value1 = CurrentFrame->OperandStack->Pop();
    u4 value2 = CurrentFrame->OperandStack->Pop();
    u4 value3 = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value1);
    CurrentFrame->OperandStack->push(value2);
    CurrentFrame->OperandStack->push(value3);
    CurrentFrame->OperandStack->push(value1);

}


void Jvm::dup2(){
    u4 value1 = CurrentFrame->OperandStack->Pop();
    u4 value2 = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value2);
    CurrentFrame->OperandStack->push(value1);
    CurrentFrame->OperandStack->push(value2);
    CurrentFrame->OperandStack->push(value1);

}




void Jvm::dup2_x1(){
    u4 value1 = CurrentFrame->OperandStack->Pop();
    u4 value2 = CurrentFrame->OperandStack->Pop();
    u4 value3 = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value2);
    CurrentFrame->OperandStack->push(value1);
    CurrentFrame->OperandStack->push(value3);
    CurrentFrame->OperandStack->push(value2);
    CurrentFrame->OperandStack->push(value1);

}





void Jvm::dup2_x2(){
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




void Jvm::swap(){    
    u4 value1 = CurrentFrame->OperandStack->Pop();
    u4 value2 = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value1);
    CurrentFrame->OperandStack->push(value2);

}




void Jvm::iadd(){    
    s4 value1;
    s4 value2;
    value2 = static_cast<int>(CurrentFrame->OperandStack->Pop());
    value1 = static_cast<int>(CurrentFrame->OperandStack->Pop());
    s4 result = value1 + value2;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
}



// Both value1 and value2 must be of type long. The values are popped from the operand stack. The long result is value1 + value2. The result is pushed onto the operand stack. The result is the 64 low-order bits of the true mathematical result in a sufficiently wide two's-complement format, represented as a value of type long. If overflow occurs, the sign of the result may not be the same as the sign of the mathematical sum of the two values. Despite the fact that overflow may occur, execution of an ladd instruction never throws a run-time exception.
void Jvm::ladd(){
	Cat2Value value1, value2, result;
	value1.Bytes = popU8FromOpStack();
    value2.Bytes = popU8FromOpStack();

	result.AsLong = value1.AsLong + value2.AsLong;
    pushU8ToOpStack(result.HighBytes, result.LowBytes);
}



// Both value1 and value2 must be of type float. The values are popped from the operand stack and undergo value set conversion (§2.8.3), resulting in value1' and value2'. The float result is value1' + value2'. The result is pushed onto the operand stack. The result of an fadd instruction is governed by the rules of IEEE arithmetic: • If either value1' or value2' is NaN, the result is NaN. • The sum of two infinities of opposite sign is NaN. • The sum of two infinities of the same sign is the infinity of that sign. • The sum of an infinity and any finite value is equal to the infinity. • The sum of two zeroes of opposite sign is positive zero. • The sum of two zeroes of the same sign is the zero of that sign. • The sum of a zero and a nonzero finite value is equal to the nonzero value. • The sum of two nonzero finite values of the same magnitude and opposite sign is positive zero. • In the remaining cases, where neither operand is an infinity, a zero, or NaN and the values have the same sign or have different magnitudes, the sum is computed and rounded to the nearest representable value using IEEE 754 round to nearest mode. If THE JAVA VIRTUAL MACHINE INSTRUCTION SET Instructions 6.5 421 the magnitude is too large to represent as a float, we say the operation overflows; the result is then an infinity of appropriate sign. If the magnitude is too small to represent as a float, we say the operation underflows; the result is then a zero of appropriate sign. The Java Virtual Machine requires support of gradual underflow as defined by IEEE 754. Despite the fact that overflow, underflow, or loss of precision may occur, execution of an fadd instruction never throws a run-time ex
void Jvm::fadd(){
	U4ToType value1, value2, result;
	value2.UBytes = CurrentFrame->OperandStack->Pop();
	value1.UBytes = CurrentFrame->OperandStack->Pop();

	result.AsFloat = value1.AsFloat + value2.AsFloat;
    CurrentFrame->OperandStack->push(static_cast<u4>(result.UBytes));
}


void Jvm::dadd() {
	Cat2Value value1, value2, result;
	value1.Bytes = popU8FromOpStack();
    value2.Bytes = popU8FromOpStack();

	result.AsDouble = value1.AsDouble + value2.AsDouble;
    pushU8ToOpStack(result.HighBytes, result.LowBytes);
}

void Jvm::isub(){    
    s4 value1;
    s4 value2;
    value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 result = value1 - value2;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
//Despite the fact that overflow may occur, execution of an isub instruction never throws a run-time exception.
}

void Jvm::lsub(){
	Cat2Value value1, value2, result;
    value2.Bytes = popU8FromOpStack();
	value1.Bytes = popU8FromOpStack();

	result.AsLong = value1.AsLong - value2.AsLong;
    pushU8ToOpStack(result.HighBytes, result.LowBytes);
}




void Jvm::fsub(){
	U4ToType value1, value2, result;
	value2.UBytes = CurrentFrame->OperandStack->Pop();
	value1.UBytes = CurrentFrame->OperandStack->Pop();

	result.AsFloat = value1.AsFloat - value2.AsFloat;
    CurrentFrame->OperandStack->push(static_cast<u4>(result.UBytes));
}




void Jvm::dsub(){
	Cat2Value value1, value2, result;
    value2.Bytes = popU8FromOpStack();
	value1.Bytes = popU8FromOpStack();

	result.AsDouble = value1.AsDouble - value2.AsDouble;
    pushU8ToOpStack(result.HighBytes, result.LowBytes);
}


void Jvm::imul(){    
    s4 value1;
    s4 value2;

    value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    s4 result = value1 * value2;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
}




void Jvm::lmul(){
	Cat2Value value1, value2, result;
    value2.Bytes = popU8FromOpStack();
	value1.Bytes = popU8FromOpStack();

	result.AsLong = value1.AsLong * value2.AsLong;
    pushU8ToOpStack(result.HighBytes, result.LowBytes);
}




void Jvm::fmul(){
	U4ToType value1, value2, result;
	value2.UBytes = CurrentFrame->OperandStack->Pop();
	value1.UBytes = CurrentFrame->OperandStack->Pop();

	result.AsFloat = value1.AsFloat * value2.AsFloat;
    CurrentFrame->OperandStack->push(static_cast<u4>(result.UBytes));
}


void Jvm::dmul(){
	Cat2Value value1, value2, result;
	value1.Bytes = popU8FromOpStack();
    value2.Bytes = popU8FromOpStack();

	result.AsDouble = value1.AsDouble * value2.AsDouble;
    pushU8ToOpStack(result.HighBytes, result.LowBytes);
}




void Jvm::idiv(){    
    int value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    int value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    if (value2 == 0)
        throw ArithmeticException();

    s4 result = value1 / value2;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
}




void Jvm::ldiv(){
	Cat2Value value1, value2, result;
    value2.Bytes = popU8FromOpStack();
	value1.Bytes = popU8FromOpStack();

	result.AsLong = value1.AsLong / value2.AsLong;
    pushU8ToOpStack(result.HighBytes, result.LowBytes);
}




void Jvm::fdiv(){
	U4ToType value1, value2, result;
	value2.UBytes = CurrentFrame->OperandStack->Pop();
	value1.UBytes = CurrentFrame->OperandStack->Pop();

	result.AsFloat = value1.AsFloat / value2.AsFloat;
    CurrentFrame->OperandStack->push(static_cast<u4>(result.UBytes));
}




void Jvm::ddiv(){
	Cat2Value value1, value2, result;
    value2.Bytes = popU8FromOpStack();
	value1.Bytes = popU8FromOpStack();

	result.AsDouble = value1.AsDouble / value2.AsDouble;
    pushU8ToOpStack(result.HighBytes, result.LowBytes);
}




void Jvm::irem(){
    int value2 = CurrentFrame->OperandStack->Pop();
    int value1 = CurrentFrame->OperandStack->Pop();
    int result;

    value1 - (value1 / value2) * value2;
    // todo throw Run-time Exception

    CurrentFrame->OperandStack->push(result);
}




void Jvm::lrem(){    
    Cat2Value value{};
    long value2 = popU8FromOpStack();
    long value1 = popU8FromOpStack();

    long result = value1 - (value1 / value2) * value2;

    value.AsLong = result;
    pushU8ToOpStack(value.HighBytes, value.LowBytes);


    if (value2 == 0)
        throw ArithmeticException();


}

void Jvm::frem(){
	U4ToType value1, value2, result;
	value2.UBytes = CurrentFrame->OperandStack->Pop();
	value1.UBytes = CurrentFrame->OperandStack->Pop();

	long q = floor(value1.AsFloat / value2.AsFloat);

	result.AsFloat = value1.AsFloat - q * value2.AsFloat;
    CurrentFrame->OperandStack->push(static_cast<u4>(result.UBytes));
}

void Jvm::drem(){
	Cat2Value value1, value2, result;
    value2.Bytes = popU8FromOpStack();
	value1.Bytes = popU8FromOpStack();

	long q = floor(value1.AsDouble / value2.AsDouble);

	result.AsDouble = value1.AsDouble - q * value2.AsDouble;
    pushU8ToOpStack(result.HighBytes, result.LowBytes);
}


void Jvm::ineg(){    
    s4 value;
    value = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 result = - value;
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
}




void Jvm::lneg(){    
    Cat2Value value{};
    value.AsLong = -1* popU8FromOpStack();

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::fneg(){    
    float value = -1 * CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value);

}




void Jvm::dneg(){
    Cat2Value value{};
    double result = -1* popU8FromOpStack();
    value.AsDouble = result;


    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::ishl(){    
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    s4 result = value1 << (0x1F & value2);
    CurrentFrame->OperandStack->push(static_cast<u4>(result));

}




void Jvm::lshl(){    
    Cat2Value value{};
    long value2 = popU8FromOpStack();
    long value1 = popU8FromOpStack();

    value.AsLong = value1 << (0x3F & value2 );


    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::ishr(){    
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    s4 result = value1 >> (0x1F & value2);
    CurrentFrame->OperandStack->push(static_cast<u4>(result));
}



void Jvm::lshr(){    
    Cat2Value value{};
    long value2 = popU8FromOpStack();
    long value1 = popU8FromOpStack();

    value.AsLong = value1 >> (0x3F & value2 );


    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}


void Jvm::iushr(){    
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    s4 shift = value2 & 0x1F;
    u4 result = (value1) >> shift;

    CurrentFrame->OperandStack->push(result);

}




void Jvm::lushr(){
    Cat2Value value{};
    int value2 = CurrentFrame->OperandStack->Pop();
    long value1 = popU8FromOpStack();

    value.AsLong = value1 >> (value2 & 0x3F);

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::iand(){

    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 result =  value1 & value2;

    CurrentFrame->OperandStack->push(static_cast<u4>(result));

}




void Jvm::land(){    
    Cat2Value value{};
    long value2 = popU8FromOpStack();
    long value1 = popU8FromOpStack();

    value.AsLong = value1 & value2;


    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::ior(){    
    s4 value2 = static_cast<s4>(CurrentFrame->OperandStack->Pop());
    s4 value1 = static_cast<s4>(CurrentFrame->OperandStack->Pop());

    s4 result =  value1 | value2;

    CurrentFrame->OperandStack->push(static_cast<u4>(result));


}




void Jvm::lor(){    
    Cat2Value value{};
    long value2 = popU8FromOpStack();
    long value1 = popU8FromOpStack();

    value.AsLong = value1 | value2;


    pushU8ToOpStack(value.HighBytes, value.LowBytes);


}




void Jvm::ixor(){    
    s4 value2 = CurrentFrame->OperandStack->Pop();
    s4 value1 = CurrentFrame->OperandStack->Pop();

    s4 result =  value1 ^ value2;

    CurrentFrame->OperandStack->push(static_cast<u4>(result));

}




void Jvm::lxor(){
    Cat2Value value{};
    long value2 = popU8FromOpStack();
    long value1 = popU8FromOpStack();

    value.AsLong = value1 ^ value2;


    pushU8ToOpStack(value.HighBytes, value.LowBytes);


}




void Jvm::iinc(){
    u1 index = (*CurrentCode->code)[pc++];
    auto const_ = static_cast<int8_t>((*CurrentCode->code)[pc++]);
    (*CurrentFrame->localVariables)[index] += const_;

}




void Jvm::i2l(){
    Cat2Value value{};
    value.AsLong = (CurrentFrame->OperandStack->Pop());

    // push pro operand stack em big endian
    pushU8ToOpStack(value.HighBytes, value.LowBytes);
}




void Jvm::i2f(){

    int Value = static_cast<int>(CurrentFrame->OperandStack->Pop());
    float AsFloat = Value;
    FieldEntry Converter{};
    Converter.AsFloat = AsFloat;
    CurrentFrame->OperandStack->push(Converter.AsInt);
 
  
}


//Converte int pra double
void Jvm::i2d(){
    Cat2Value value{};

    s4 AsInt = CurrentFrame->OperandStack->Pop();
    value.AsDouble = AsInt;

    // push em big endian
    pushU8ToOpStack(value.HighBytes, value.LowBytes);
}




void Jvm::l2i(){    
    Cat2Value value{};

    value.AsLong = popU8FromOpStack();
    // push pro low-order 32 pro stack, em big endian
    CurrentFrame->OperandStack->push(value.LowBytes);

}



void Jvm::l2f(){
    float value = popU8FromOpStack();

    CurrentFrame->OperandStack->push(static_cast<u4>(value));
}




void Jvm::l2d(){
    Cat2Value value{};
    long long AsLong;

    AsLong = popU8FromOpStack();
    value.AsDouble = AsLong;

    // push pro operand stack em big endian
    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::f2i(){
    U4ToType converter {};

    converter.AsFloat = CurrentFrame->OperandStack->Pop();
    u4 result = converter.AsInt;

    CurrentFrame->OperandStack->push(result);
}


// todo fix, might be wrong
void Jvm::f2l(){
    //converte u4 pra float, float pra long
    Cat2Value value{};
    U4ToType valueFloat {};
    valueFloat.AsFloat = (CurrentFrame->OperandStack->Pop());

    value.AsLong = valueFloat.AsFloat;

    if(std::isnan(value.AsLong)){
        value.HighBytes = 0;
        value.LowBytes = 0;

    } else if(  value.AsLong >= INT64_MAX ||  value.AsLong == std::numeric_limits<float>::infinity()){
        value.LowBytes = static_cast<u4>((INT64_MAX & 0xffffffff00000000) >> 32);
        value.HighBytes = static_cast<u4>(INT64_MAX & 0x00000000ffffffff);

    } else if(  value.AsLong <= INT64_MIN ||  value.AsLong == -1 * std::numeric_limits<float>::infinity()) {
        value.LowBytes = static_cast<u4>((INT64_MIN & 0xffffffff00000000) >> 32);
        value.HighBytes = static_cast<u4>(INT64_MIN & 0x00000000ffffffff);

    } else {

    }

    pushU8ToOpStack(value.HighBytes, value.LowBytes);

}




void Jvm::f2d(){
    Cat2Value value{};
    U4ToType AsFloat{};

    AsFloat.AsFloat = CurrentFrame->OperandStack->Pop();
    value.AsDouble = AsFloat.AsFloat;
    pushU8ToOpStack(value.HighBytes, value.LowBytes);
}



void Jvm::d2i(){
    Cat2Value value{};
    U4ToType converter {};
    value.AsDouble = popU8FromOpStack();
    converter.AsInt = value.AsDouble;
    CurrentFrame->OperandStack->push(converter.UBytes);

}




void Jvm::d2l(){
    Cat2Value value{};
    Cat2Value AsLong {};

    value.AsDouble = popU8FromOpStack();
    AsLong.AsLong = value.AsDouble;

    pushU8ToOpStack(AsLong.HighBytes, AsLong.LowBytes);

}




void Jvm::d2f(){
    Cat2Value valDouble{};
    U4ToType converter {};
    
    valDouble.AsDouble = popU8FromOpStack();
    converter.AsFloat = valDouble.AsDouble;
    
    CurrentFrame->OperandStack->push(converter.UBytes);

}


void Jvm::i2b(){    
    U4ToType value;
    value.AsInt = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(static_cast<u4>(value.AsByte));
   return;
}




void Jvm::i2c(){    
    U4ToType value{};
    value.AsInt = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value.AsChar);

}




void Jvm::i2s(){
    U4ToType value{};
    // trunca e faz sign extension
    value.AsShort = CurrentFrame->OperandStack->Pop();
    CurrentFrame->OperandStack->push(value.UBytes);
}



void Jvm::lcmp(){
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




void Jvm::dcmpl(){
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
    int32_t value = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());

	s2 offset = GetIndex2();
    if(value == 0) pc += offset - 3; // Por que o PC ja andou 1 + 2 bytes branchs
}




void Jvm::ifne(){
    int32_t value = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());

	s2 offset = GetIndex2();
    if(value != 0) pc += offset - 3; // Por que o PC ja andou 1 + 2 bytes branchs
}




void Jvm::iflt(){
    int32_t value = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());

	s2 offset = GetIndex2();
    if(value < 0) pc += offset - 3; // Por que o PC ja andou 1 + 2 bytes branchs
}




void Jvm::ifge(){

    auto branchoffset = static_cast<int16_t>(GetIndex2()) -3; // em relacao ao $
    auto value = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());

    if(value >= 0){
        pc += branchoffset;
    }

}





void Jvm::ifgt(){

    auto branchoffset = static_cast<int16_t>(GetIndex2()) -3; // em relacao ao $
    int32_t value = static_cast<int>(CurrentFrame->OperandStack->Pop());

    if(value > 0)
        pc += branchoffset;

}





void Jvm::ifle(){
    auto branchoffset = static_cast<int16_t>(GetIndex2()) -3; // em relacao ao $
    int32_t value = static_cast<int>(CurrentFrame->OperandStack->Pop());

    if(value <= 0){
        pc += branchoffset;

    }

}




void Jvm::if_icmpeq(){

    auto branchoffset = static_cast<int16_t>(GetIndex2()) -3; // em relacao ao $
    int32_t value2 = static_cast<uint32_t>(CurrentFrame->OperandStack->Pop());
    int32_t value1 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());

    if(value1 == value2){
        pc += branchoffset;

    }
}




void Jvm::if_icmpne(){

    auto branchoffset = static_cast<int16_t>(GetIndex2()) -3; // em relacao ao $
    auto value2 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());
    auto value1 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());

    if(value1 != value2){
        pc += branchoffset;
    }
}




void Jvm::if_icmplt(){

    auto branchoffset = static_cast<int16_t>(GetIndex2()) -3; // em relacao ao $
    auto value2 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());
    auto value1 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());

    if(value1 < value2){
        pc += branchoffset ;
    }
}




void Jvm::if_icmpge(){

    auto branchoffset = static_cast<int16_t>(GetIndex2()) -3; // em relacao ao $

    auto value2 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());
    auto value1 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());

    if(value1 >= value2){

        pc += branchoffset;

    }
}

void Jvm::if_icmpgt(){

    auto branchoffset = static_cast<int16_t>(GetIndex2()) -3; // em relacao ao $

    auto value2 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());
    auto value1 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());

    if(value1 > value2){

        pc += branchoffset ;

    }
}

void Jvm::if_icmple(){

    auto branchoffset = static_cast<int16_t>(GetIndex2()) -3; // em relacao ao $

    auto value2 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());
    auto value1 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());

    if(value1 <= value2){

        pc += branchoffset ;

    }

}

void Jvm::if_acmpeq(){

    auto branchoffset = static_cast<int16_t>(GetIndex2()) -3; // em relacao ao $
    auto value2 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());
    auto value1 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());

    if(value1 == value2){
        pc += branchoffset ;

    }
}

void Jvm::if_acmpne(){

    auto branchoffset = static_cast<int16_t>(GetIndex2()) -3; // em relacao ao $
    int32_t value2 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());
    int32_t value1 = static_cast<int32_t>(CurrentFrame->OperandStack->Pop());

    if(value1 != value2){
        pc += branchoffset ;

    }
}

void Jvm::goto_(){
    auto branchoffset = static_cast<int16_t>(GetIndex2()); 
    pc += branchoffset -3; // Por que o PC ja andou 1 + 2 bytes branchs
}

// todo testa, potencial de ta errado
void Jvm::jsr(){
	u4 inst_pc = pc - 1;
    u2 offset = GetIndex2();
    CurrentFrame->OperandStack->push(pc);
	pc = inst_pc + offset;
}




void Jvm::ret(){
    u1 index = (*CurrentCode->code)[pc++];
    u4 NewPc = (*CurrentFrame->localVariables)[index];
    pc = NewPc;
}




void Jvm::tableswitch() {
	int inst_pc = pc - 1;
    while (pc % 4 != 0) pc++;

    // Lê os valores do bytecode
    int32_t default_offset = read_u4();
    int32_t low = read_u4();
    int32_t high = read_u4();

    // Calcula o número de jump offsets
    int32_t num_offsets = high - low + 1;

    // Lê os jump offsets
    std::vector<int32_t> jump_offsets(num_offsets);
    for (int32_t i = 0; i < num_offsets; ++i) {
        jump_offsets[i] = read_u4();
    }

    // Obtém a key do topo da pilha de operandos
    int32_t index = PopOpStack();

    // Verifica se o índice está dentro do intervalo
    if (index < low || index > high) {
		pc = inst_pc + default_offset;
    } else {
        int32_t offset = jump_offsets[index - low];
        pc = inst_pc + offset;  // Ajusta pc para o offset correspondente ao índice
    }
}



void Jvm::lookupswitch(){
	int inst_pc = pc - 1;
    while (pc % 4 != 0) pc++;

    int32_t default_offset = read_u4();
    int32_t npairs = read_u4();

    std::map<int32_t, int32_t> matchOffsetPairs;
    for (int32_t i = 0; i < npairs; ++i) {
        int32_t match = read_u4();
        int32_t offset = read_u4();
        matchOffsetPairs[match] = offset;
    }

    int32_t key = PopOpStack();

    auto it = matchOffsetPairs.find(key);
    if (it != matchOffsetPairs.end()) {
        pc = inst_pc + it->second;
    } else {
		pc = inst_pc + default_offset;
    }

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
    if(FrameStack.size() != 1) // caso seja 1, ta no final da main ou clinit da main
        PopFrameStack();
}


void Jvm::getstatic(){
    u2 index = GetIndex2();
    cp_info* Fieldref     = GetConstantPoolEntryAt(index);
    auto ClassInfo = GetConstantPoolEntryAt(Fieldref->class_index);
    auto ClassName           =GetConstantPoolEntryAt(ClassInfo->name_index)->AsString();
    cp_info* NameAndType  = GetConstantPoolEntryAt(Fieldref->name_and_type_index);
    auto Name      = GetConstantPoolEntryAt(NameAndType->name_index)->AsString();
    auto Descriptor= GetConstantPoolEntryAt(NameAndType->descriptor_index)->AsString();

    if(Descriptor == "Ljava/io/PrintStream;")
        return;

    class_file* Class = GetClass(ClassName);
    auto& StaticFields = *(Class->StaticFields);
    auto Entry  = StaticFields[Name];

    CurrentFrame->OperandStack->push(Entry->AsInt);

}
//TODO:  so pra int para testar

void Jvm::putstatic(){
    u2 index = GetIndex2();
    u4 value = PopOpStack();

    cp_info* Fieldref        = GetConstantPoolEntryAt(index);
    auto ClassInfo = GetConstantPoolEntryAt(Fieldref->class_index);
    auto ClassName = GetConstantPoolEntryAt(ClassInfo->name_index)->AsString();
    cp_info* NameAndType  = GetConstantPoolEntryAt(Fieldref->name_and_type_index);
    auto Name      = GetConstantPoolEntryAt(NameAndType->name_index)->AsString();
    auto Descriptor= GetConstantPoolEntryAt(NameAndType->descriptor_index)->AsString();

    class_file* Class = GetClass(ClassName);
    auto& StaticFields = *(Class->StaticFields);


    auto Entry =  new FieldEntry;
    Entry->AsInt = value;
    StaticFields[Name] = Entry;

}

void Jvm::getfield(){
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



void Jvm::new_(){
    
    u2 index = GetIndex2();
    cp_info* ref = GetConstantPoolEntryAt(index);

    // resolvendo se classe ou interface
    if(ref->tag == ConstantPoolTag::CONSTANT_Class){
        auto ClassName = GetConstantPoolEntryAt(ref->name_index)->AsString();
        //allocate memory to class by name
        class_file* ClassFile;
        if(IsLoaded(ClassName))
            ClassFile = GetClass(ClassName);
        else{
            pc-=3;
            GetClass(ClassName);
            return;
        }
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
        
    }
    else{
        std::cerr<<"new: CP_INFO nao e nem classe nem interface\n";
    }


}

void Jvm::putfield(){
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



} 
//TODO: to fazendo so pro hello world por enquanto

void Jvm::invokevirtual(){
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
		std::cout << std::endl;
        return;
    }
    else if(MethodName == "print"){
        JavaPrint(MethodDescriptor);
        return;
    }

    invoke(ClassName, MethodName, MethodDescriptor);

}



void Jvm::invokespecial(){
    u2 index = GetIndex2();
    auto MethodOrInterfaceMethod = GetConstantPoolEntryAt(index);

    auto Class = GetConstantPoolEntryAt(MethodOrInterfaceMethod->class_index);

    auto NameAndType = GetConstantPoolEntryAt(MethodOrInterfaceMethod->name_and_type_index);

    auto ClassName = GetConstantPoolEntryAt(Class->name_index)->AsString();
    if(ClassName == "java/lang/Object")
        return;

    auto Descriptor = GetConstantPoolEntryAt(NameAndType->descriptor_index)->AsString();
    //faz uma copia da pilha de operandos do frame atual pra carregar as variaveis locais na pilha nova
    if(!IsLoaded(ClassName)){
        pc-=3;
        GetClass(ClassName);
        return;
    }

    invoke(ClassName, {"<init>"}, Descriptor);

}




void Jvm::invokestatic(){

    u2 index = GetIndex2();

    cp_info* MethodRef = (*CurrentClass->constant_pool)[index];
    auto ClassEntry = (GetConstantPoolEntryAt(MethodRef->class_index));
    auto ClassName = GetConstantPoolEntryAt(ClassEntry->name_index)->AsString();
    cp_info* NameAndType = (*CurrentClass->constant_pool)[MethodRef->name_and_type_index];

    // formato <valor>
    auto MethodName = (*CurrentClass->constant_pool)[NameAndType->name_index]->AsString();
    auto MethodDescriptor = (*CurrentClass->constant_pool)[NameAndType->descriptor_index]->AsString();

    auto isStatic = true;
    if(!IsLoaded(ClassName)){
        pc-=3;
        GetClass(ClassName);
        return;
    }
    invoke(ClassName, MethodName, MethodDescriptor, isStatic);

}

void Jvm::invokeinterface(){
    u2 index = GetIndex2();
    u1 Count = NextCodeByte();
    NextCodeByte();
    cp_info* MethodRef = (*CurrentClass->constant_pool)[index];
    auto ClassEntry = (GetConstantPoolEntryAt(MethodRef->class_index));
    auto ClassName = GetConstantPoolEntryAt(ClassEntry->name_index)->AsString();
    cp_info* NameAndType = (*CurrentClass->constant_pool)[MethodRef->name_and_type_index];

    // formato <valor>
    auto MethodName = (*CurrentClass->constant_pool)[NameAndType->name_index]->AsString();
    auto MethodDescriptor = (*CurrentClass->constant_pool)[NameAndType->descriptor_index]->AsString();

    invoke(ClassName, MethodName, MethodDescriptor);

}


void Jvm::invokedynamic(){
    throw std::runtime_error("nao temos invokeDynamic\n");
}




void Jvm::newarray(){

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

void Jvm::anewarray(){
    throw std::runtime_error("anewarray n implementado\n");

}

void Jvm::arraylength(){
    auto* ArrayRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    auto* Array = ArrayRef->ArrayRef;
    CurrentFrame->OperandStack->push(Array->size);
}


void Jvm::athrow(){    


}

void Jvm::checkcast(){

}

void Jvm::instanceof() {
    auto ObjectRef = reinterpret_cast<Reference*>(CurrentFrame->OperandStack->Pop());
    auto index = GetIndex2();
    int result = 0;
    if(ObjectRef->Nullref != nullptr) {
        auto entry = GetConstantPoolEntryAt(index);
        auto Name = GetConstantPoolEntryAt(entry->name_index)->AsString();
        auto Class = GetClass(Name);
        auto InstanceClass = ObjectRef->ClassRef->ClassHandle->ClassObject;
        if (InstanceClass == Class)
            result = 1;
    }
    CurrentFrame->OperandStack->push(result);


}

void Jvm::monitorenter(){    
    // região crítica, thread n é implementado
    return;

}

void Jvm::monitorexit(){    
    // região crítica, thread n é implementado
    return;

}



void Jvm::wide(){
	u1 instruction = (*CurrentCode->code)[pc++];
	u2 index = GetIndex2();

	switch((WideOp) instruction) {
	case(WideOp::WIDE_fload): 
	case(WideOp::WIDE_aload):
	case(WideOp::WIDE_iload): {
		u4 value = (*CurrentFrame->localVariables)[index];
		CurrentFrame->OperandStack->push(value);
		break;
	}
	case(WideOp::WIDE_dload):
	case(WideOp::WIDE_lload): {
		Cat2Value value;
		value.Bytes = getU8FromLocalVars(index);
		pushU8ToOpStack(value.HighBytes, value.LowBytes);
		break;
	}

	case(WideOp::WIDE_astore):
	case(WideOp::WIDE_fstore):
	case(WideOp::WIDE_istore): {
		u4 value = CurrentFrame->OperandStack->Pop();
		(*CurrentFrame->localVariables)[index] = value;
		break;
	}
	case(WideOp::WIDE_dstore):
	case(WideOp::WIDE_lstore): {
		u4 lowBytes = CurrentFrame->OperandStack->Pop();
		u4 highBytes = CurrentFrame->OperandStack->Pop();
		(*CurrentFrame->localVariables)[index] = highBytes;
		(*CurrentFrame->localVariables)[index + 1] = lowBytes;
		break;
	}
	case(WideOp::WIDE_ret): {
		uint32_t NewPc = (*CurrentFrame->localVariables)[index];
		pc = NewPc;
		break;
	}
	case(WideOp::WIDE_iinc): {
		int32_t const_ = static_cast<int32_t>(GetIndex2());
		(*CurrentFrame->localVariables)[index] += const_;
		break;
	}
	}
}



//TODO: vou usar 1 byte pra testar por enquanto, dps fazer pros outros; -> switch case do tipo

void Jvm::multianewarray(){
    u2          index           = GetIndex2();
    u1          dimensions      = NextCodeByte();
    cp_info*    ArrayInfo       = GetConstantPoolEntryAt(index);
    std::string ArrayDiscriptor = GetConstantPoolEntryAt(ArrayInfo->name_index)->AsString();

    auto c  = ArrayDiscriptor.c_str();
    ArrayTypeCode Type{};
    c+=dimensions;
    switch (*c) {
        case 'L':
            Type = ArrayTypeCode::T_REF;
            break;
        case 'B':
            Type = ArrayTypeCode::T_BYTE;
            break;
        case 'C':
            Type = ArrayTypeCode::T_CHAR;
            break;
        case 'D':
            Type = ArrayTypeCode::T_DOUBLE;
            break;
        case 'F':
            Type = ArrayTypeCode::T_FLOAT;
            break;
        case 'I':
            Type = ArrayTypeCode::T_INT;
            break;
        case 'J':
            Type = ArrayTypeCode::T_LONG;
            break;
        case 'S':
            Type = ArrayTypeCode::T_SHORT;
            break;
        case 'Z':
            Type = ArrayTypeCode::T_BOOLEAN;
            break;
        default:
            Type = ArrayTypeCode::T_INT;
            break;

    }



    JVM::stack<int> sizes;

    // pega o tamanho de cada dimensao
    for (int i = 0; i < dimensions; ++i) {
        int count = static_cast<int>(CurrentFrame->OperandStack->Pop());
        sizes.push(count);
    }

    CurrentFrame->OperandStack->push(reinterpret_cast<u4p>(NewArray(Type, sizes, dimensions)));
}


void Jvm::ifnull(){    
    auto value = reinterpret_cast<Reference*>(PopOpStack());
    if ( value == NULL){
        u2 offset = GetIndex2() - 3; 
        pc += offset;
    }

}

void Jvm::ifnonnull(){    
    auto value = reinterpret_cast<Reference*>(PopOpStack());
    if ( value != NULL){
// Execution then proceeds at that offset from the address of the opcode of this ifnonnull instruction
        u2 offset = GetIndex2() - 3;
        pc += offset;
    }

}


void Jvm::goto_w(){    
    u1 branchbyte1 = (*CurrentCode->code)[pc++];
    u1 branchbyte2 = (*CurrentCode->code)[pc++];
    u1 branchbyte3 = (*CurrentCode->code)[pc++];
    u1 branchbyte4 = (*CurrentCode->code)[pc++];
    s4 branchoffset = (branchbyte1 << 24) | (branchbyte2 << 16)| (branchbyte3 << 8) | branchbyte4;

    pc += branchoffset;
}


void Jvm::jsr_w(){    
	u4 inst_pc = pc - 1;
    u4 offset = read_u4();
    CurrentFrame->OperandStack->push(pc);
	pc = inst_pc + offset;
}

