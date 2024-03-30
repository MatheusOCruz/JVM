#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <memory>
#define COPY_BYTES_AND_ADVANCE_ITERATOR(iter, X) std::copy(iter, iter + sizeof(X), reinterpret_cast<uint8_t*>(&X)); iter += sizeof(X);

typedef std::vector<uint8_t>::iterator buffer_iterator;

enum class ConstantPoolTag : uint8_t {
    CONSTANT_Utf8 = 1,
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_Class = 7,
    CONSTANT_String = 8,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_NameAndType = 12,
    CONSTANT_MethodHandle = 15,
    CONSTANT_MethodType = 16,
    CONSTANT_InvokeDynamic = 18
};

struct Constant_Pool_Entry{
    uint8_t tag;
    union {
        struct { //CONSTANT_Class_info, CONSTANT_NameAndType_info
            uint16_t name_index;
            uint16_t descriptor_index;  //CONSTANT_Class_info nao tem esse, mas n rola de ter 2 struct com name_index, paciencia

        };
        struct { // CONSTANT_Fieldref_info, CONSTANT_Methodref_info, CONSTANT_InterfaceMethodref_info
            uint16_t class_index;
            uint16_t name_and_type_index;
        };
        struct { //CONSTANT_String_info
            uint16_t string_index;
        };
        struct{ //CONSTANT_Integer_info, CONSTANT_Float_info
            uint32_t bytes;
        };
        struct{ //CONSTANT_Long_info, CONSTANT_Double_info
            uint32_t high_bytes;
            uint32_t low_bytes;
        };
        struct{
            uint16_t  lenght;
            uint8_t bytes_vec[]; //o vetor ta dando bo, vamo ter q ir de malloc msm
        };
    };
    // ta faltando tudo, so uma ideia de como implementar

};



struct cp_info{
    uint8_t tag;
    Constant_Pool_Entry info;
};

struct attribute_info{
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    std::vector<uint8_t> info; // em tese nesse caso aqui e byte, num sei
};

struct field_info{
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attibutes_count;
    std::vector<attribute_info> attributes;
};
struct method_info{
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    std::vector<attribute_info> attributes;
};

Constant_Pool_Entry buildConstantPoolEntry(int tag, uint8_t info[]){
        //ler os bytes de acordo com o tag
    switch (static_cast<ConstantPoolTag>(tag)) {
        case ConstantPoolTag::CONSTANT_Utf8:
            std::cout << "Constante Utf8" << std::endl;
            break;
        case ConstantPoolTag::CONSTANT_Integer:
            std::cout << "Constante Integer" << std::endl;
            break;
        case ConstantPoolTag::CONSTANT_Float:
            std::cout << "Constante Float" << std::endl;
            break;
        case ConstantPoolTag::CONSTANT_Long:
            std::cout << "Constante Long" << std::endl;
            break;
        case ConstantPoolTag::CONSTANT_Double:
            std::cout << "Constante Double" << std::endl;
            break;
        case ConstantPoolTag::CONSTANT_Class:
            std::cout << "Constante Class" << std::endl;
            break;
        case ConstantPoolTag::CONSTANT_String:
            std::cout << "Constante String" << std::endl;
            break;
        case ConstantPoolTag::CONSTANT_Fieldref:
            std::cout << "Constante Fieldref" << std::endl;
            break;
        case ConstantPoolTag::CONSTANT_Methodref:
            std::cout << "Constante Methodref" << std::endl;
            break;
        case ConstantPoolTag::CONSTANT_InterfaceMethodref:
            std::cout << "Constante InterfaceMethodref" << std::endl;
            break;
        case ConstantPoolTag::CONSTANT_NameAndType:
            std::cout << "Constante NameAndType" << std::endl;
            break;
        case ConstantPoolTag::CONSTANT_MethodHandle:
            std::cout << "Constante MethodHandle" << std::endl;
            break;
        case ConstantPoolTag::CONSTANT_MethodType:
            std::cout << "Constante MethodType" << std::endl;
            break;
        case ConstantPoolTag::CONSTANT_InvokeDynamic:
            std::cout << "Constante InvokeDynamic" << std::endl;
            break;
        default:
            throw std::runtime_error("ConstantPoolEntry nao existe amigao");
    }
}

void buildConstantPoolTable(buffer_iterator iter, int constant_pool_count,std::vector<cp_info> constant_pool){
    uint8_t tag;
    for(int i = 0; i<constant_pool_count-1; i++){

    }
}



int main(int argc, char* argv[]) {
    /*
    if (argc != 2){
        std::cerr<<"tem que passar o arquivo meu mestre\n";
        return 1;
    }
    */



    std::string class_file_path = "/home/matheus/CLionProjects/JVM/Main.class";

    std::ifstream class_file(class_file_path, std::ios::binary);

    if(!class_file) std::cerr << "deu ruim pra abrir o arquivo:"<<class_file_path<<std::endl;

    std::vector<uint8_t> buffer(std::istream_iterator<char>(class_file),{});

    auto iter = buffer.begin();

    uint32_t                    magic;
    uint16_t                    minor_version;
    uint16_t                    major_version;
    uint16_t                    constant_pool_count;
    std::vector<cp_info>        constant_pool;
    uint16_t                    access_flags;
    uint16_t                    this_class;
    uint16_t                    super_class;
    uint16_t                    interfaces_count;
    std::vector<uint16_t>       interfaces;
    uint16_t                    fields_count;
    std::vector<field_info>     fields;
    uint16_t                    methods_count;
    std::vector<method_info>    methods;
    uint16_t                    attributes_count;
    std::vector<attribute_info> attributes;


    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, magic);
    magic = __builtin_bswap32(magic);

    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, minor_version);
    minor_version = __builtin_bswap16(minor_version);

    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, major_version);
    major_version = __builtin_bswap16(major_version);

    printf("%X\n",magic);
    printf("%d.%d",major_version,minor_version);









    return 0;
}
