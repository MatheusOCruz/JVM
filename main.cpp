#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <memory>


template<typename T>
inline void swapBytes( T *var) {
    swapBytes<T>(var);
}

template<>
inline void swapBytes<uint32_t>(uint32_t *var) {
    *var = __builtin_bswap32(*var);
}

template<>
inline void swapBytes<uint16_t>(uint16_t *var) {
    *var = __builtin_bswap16(*var);
}
template<>
inline void swapBytes<uint8_t>(uint8_t *var) { } // n precisa de swap

#define COPY_BYTES_AND_ADVANCE_ITERATOR(iter, X) \
std::copy((iter), (iter) + sizeof(X), reinterpret_cast<uint8_t*>(&X)); \
iter += sizeof(X); \
swapBytes(&X);




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

struct cp_info{
    uint8_t tag;
    union {
        struct { //CONSTANT_Class_info, CONSTANT_NameAndType_info
            uint16_t name_index;
            uint16_t descriptor_index;  // CONSTANT_MethodType_info , CONSTANT_NameAndType_info

        };
        struct { // CONSTANT_Fieldref_info, CONSTANT_Methodref_info, CONSTANT_InterfaceMethodref_info, CONSTANT_InvokeDynamic_info
            union{
                uint16_t class_index; // CONSTANT_Fieldref_info, CONSTANT_Methodref_info, CONSTANT_InterfaceMethodref_info
                uint16_t bootstrap_method_attr_index; // CONSTANT_InvokeDynamic_info
            };
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
        struct{ // CONSTANT_UTF8_info
            uint16_t  length;
            std::vector<uint8_t> *bytes_vec; //o vetor ta dando bo, vamo ter q ir de malloc msm
        };
        struct{ // CONSTANT_MethodHandle_info
            uint8_t reference_kind;
            uint16_t reference_index;
        };
    };
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

std::unique_ptr<cp_info> buildConstantPoolEntry(buffer_iterator *iter, int tag) {
    std::unique_ptr<cp_info> Entry(new cp_info{});

    Entry->tag = tag;
    //ler os bytes de acordo com o tag
    switch (static_cast<ConstantPoolTag>(tag)) {
        case ConstantPoolTag::CONSTANT_Utf8: {
            std::cout << "Constante Utf8" << std::endl;

            uint16_t length;
            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, length);

            auto bytes = new std::vector<uint8_t>();
            bytes->reserve(length);

            std::copy((*iter), (*iter) + length, std::back_inserter(*bytes));
            *iter += length;

            Entry->length = length;
            Entry->bytes_vec = bytes;

            break;
        }
        case ConstantPoolTag::CONSTANT_Integer:
        case ConstantPoolTag::CONSTANT_Float: {
            std::cout << "Constante Integer ou Float" << std::endl;

            uint32_t bytes;
            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, bytes);
            Entry->bytes = bytes;

            break;
        }
        case ConstantPoolTag::CONSTANT_Long:
        case ConstantPoolTag::CONSTANT_Double: {
            std::cout << "Constante Long ou Double" << std::endl;

            uint32_t high_bytes;
            uint32_t low_bytes;

            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, high_bytes);
            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, low_bytes);

            Entry->high_bytes = high_bytes;
            Entry->low_bytes = low_bytes;

            break;
        }
        case ConstantPoolTag::CONSTANT_Class: {
            std::cout << "Constante Class" << std::endl;

            uint16_t name_index;
            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, name_index);
            Entry->name_index = name_index;

            break;
        }
        case ConstantPoolTag::CONSTANT_String: {
            std::cout << "Constante String" << std::endl;

            uint16_t string_index;
            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, string_index);
            Entry->string_index = string_index;

            break;
        }
        case ConstantPoolTag::CONSTANT_Fieldref:
        case ConstantPoolTag::CONSTANT_Methodref:
        case ConstantPoolTag::CONSTANT_InterfaceMethodref: {
            std::cout << "Constante Fieldref ou Methodref ou InterfaceMethodref" << tag << std::endl;

            uint16_t class_index;
            uint16_t name_and_type_index;

            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, class_index);
            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, name_and_type_index);

            Entry->class_index = class_index;
            Entry->name_and_type_index = name_and_type_index;

            break;
        }
        case ConstantPoolTag::CONSTANT_NameAndType: {
            std::cout << "Constante NameAndType" << std::endl;

            uint16_t name_index;
            uint16_t descriptor_index;

            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, name_index);
            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, descriptor_index);

            Entry->name_index = name_index;
            Entry->descriptor_index = descriptor_index;

            break;
        }
        case ConstantPoolTag::CONSTANT_MethodHandle:
            std::cout << "Constante MethodHandle" << std::endl;
            uint8_t  reference_kind;
            uint16_t reference_index;

            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, reference_kind);
            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, reference_index);

            Entry->reference_kind  = reference_kind;
            Entry->reference_index = reference_index;

            break;
        case ConstantPoolTag::CONSTANT_MethodType:
            std::cout << "Constante MethodType" << std::endl;

            uint16_t descriptor_index;
            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, descriptor_index);
            Entry->descriptor_index = descriptor_index;

            break;
        case ConstantPoolTag::CONSTANT_InvokeDynamic:
            std::cout << "Constante InvokeDynamic" << std::endl;

            uint16_t bootstrap_method_attr_index;
            uint16_t name_and_type_index;

            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, bootstrap_method_attr_index);
            COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, name_and_type_index);

            Entry->bootstrap_method_attr_index = bootstrap_method_attr_index;
            Entry->name_and_type_index = name_and_type_index;

            break;
        default:
            throw std::runtime_error("cp_info nao existe amigao");
    }
    return Entry;
}


void buildConstantPoolTable(buffer_iterator *iter, int constant_pool_count,std::vector<std::unique_ptr<cp_info>> &constant_pool){
    uint8_t tag;
    std::unique_ptr<cp_info> uPtr;
    constant_pool.reserve(constant_pool_count);
    for(int i = 0; i<constant_pool_count-1; i++){

        COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, tag);
        constant_pool.emplace_back(std::move(buildConstantPoolEntry(iter,tag)));

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
    using unique_cp_info = std::unique_ptr<cp_info>; // so pras var ficar na mesma coluna msm
    uint32_t                    magic;
    uint16_t                    minor_version;
    uint16_t                    major_version;
    uint16_t                    constant_pool_count;
    std::vector<std::unique_ptr<cp_info>> constant_pool;
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

    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, minor_version);

    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, major_version);

    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, constant_pool_count);

    printf("magic: %X\n",magic);
    printf("version: %d.%d\n",major_version,minor_version);
    printf("constant pool count: %d",constant_pool_count);

    buildConstantPoolTable(&iter, constant_pool_count, constant_pool);



    return 0;
}
