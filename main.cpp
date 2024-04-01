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

typedef uint8_t  u1;
typedef uint16_t u2;
typedef uint32_t u4;

typedef std::vector<uint8_t>::iterator buffer_iterator;

u1 read_u1(buffer_iterator *iter){
    u1 temp;
    std::copy((*iter), (*iter) + sizeof(temp), reinterpret_cast<uint8_t*>(&temp));
    *iter+= sizeof(u1);
    return temp;
}

u2 read_u2(buffer_iterator *iter){
    u2 temp;
    std::copy((*iter), (*iter) + sizeof(temp), reinterpret_cast<uint8_t*>(&temp));
    *iter+=sizeof(u2);
    return temp;
}

u4 read_u4(buffer_iterator *iter){
    u4 temp;
    std::copy((*iter), (*iter) + sizeof(temp), reinterpret_cast<uint8_t*>(&temp));
    *iter+=sizeof(u2);
    return temp;
}



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
    ConstantPoolTag tag;
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
    std::vector<uint8_t>* info; // em tese nesse caso aqui e byte, num sei
};

struct field_info{
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    std::vector<attribute_info*>* attributes;
};
struct method_info{
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    std::vector<attribute_info*>* attributes;
};

cp_info* buildConstantPoolEntry(buffer_iterator *iter, int tag) {
    auto Entry = new cp_info{};

    Entry->tag = static_cast<ConstantPoolTag>(tag);
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


void buildConstantPoolTable(buffer_iterator *iter, int constant_pool_count,std::vector<cp_info*> &constant_pool){
    uint8_t tag;

    constant_pool.reserve(constant_pool_count);

    for(int i = 0; i<constant_pool_count-1; i++){

        COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, tag);
        constant_pool.push_back(buildConstantPoolEntry(iter,tag));

    }
}
attribute_info* buildAttributeInfo(buffer_iterator *iter){
    auto Entry = new attribute_info{};

    uint16_t attribute_name_index;
    uint32_t attribute_lenght;

    auto info = new std::vector<uint8_t>;
    info->reserve(attribute_lenght);

    COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, attribute_name_index);
    COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, attribute_lenght);

    std::copy((*iter), (*iter) + attribute_lenght, std::back_inserter(*info));
    *iter += attribute_lenght;

    Entry->attribute_name_index = attribute_name_index;
    Entry->attribute_length = attribute_lenght;
    Entry->info = info;

    return Entry;
}

void buildAttributes(buffer_iterator *iter,int attributes_count, std::vector<attribute_info*> &attributes){
    attributes.reserve(attributes_count);
    for (int i = 0; i < attributes_count; ++i) {
        attributes.push_back(buildAttributeInfo(iter));
    }
}


field_info* buildFieldInfo(buffer_iterator *iter){
    auto Entry = new field_info{};
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t attributes_count;

    COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, access_flags);
    COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, name_index);
    COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, attributes_count);

    Entry->access_flags = access_flags;
    Entry->name_index = name_index;
    Entry->attributes_count = attributes_count;

    buildAttributes(iter, attributes_count, *Entry->attributes);



}

void buildFields(buffer_iterator *iter, int fields_count,std::vector<field_info*> &fields){
    for (int i = 0; i < fields_count; ++i) {
        fields.push_back(buildFieldInfo(iter));
    }
}

method_info* buildMethodInfo(buffer_iterator *iter){
    auto Entry = new method_info{};

    COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, Entry->access_flags);
    COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, Entry-> name_index);
    COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, Entry->descriptor_index);
    COPY_BYTES_AND_ADVANCE_ITERATOR(*iter, Entry->attributes_count)

    buildAttributes(iter, Entry->attributes_count, *Entry->attributes);

    return Entry;
}

void buildMethods(buffer_iterator *iter, int methods_count, std::vector<method_info*> &methods){
    methods.reserve(methods_count);
    for (int i = 0; i < methods_count; ++i) {
        methods.push_back(buildMethodInfo(iter));
    }
}



std::vector<uint8_t> FileToBuffer(const char* nomeArquivo) {
    std::ifstream arquivo(nomeArquivo, std::ios::binary);

    if (!arquivo) {
        std::cerr << "Não foi possível abrir o arquivo: " << nomeArquivo << std::endl;
        return {};
    }

    // Obter o tamanho do arquivo
    arquivo.seekg(0, std::ios::end);
    std::streampos tamanhoArquivo = arquivo.tellg();
    arquivo.seekg(0, std::ios::beg);

    // Ler o arquivo byte a byte e armazená-lo no vetor buffer
    std::vector<uint8_t> buffer(tamanhoArquivo);
    arquivo.read(reinterpret_cast<char*>(buffer.data()), tamanhoArquivo);

    return buffer;
}

int main(int argc, char* argv[]) {
    /*
    if (argc != 2){
        std::cerr<<"tem que passar o arquivo meu mestre\n";
        return 1;
    }
    */



    char* class_file_path = "/home/matheus/CLionProjects/JVM/Main.class";

    std::vector<uint8_t> buffer = FileToBuffer(class_file_path);



    int i = 0;
    for( auto byte: buffer){
        i++;
        printf("%02X ", byte);
        if (i == 10){
            printf("\n");
            i = 0;
        }
    }

    auto iter = buffer.begin();

    uint32_t                    magic;
    uint16_t                    minor_version;
    uint16_t                    major_version;
    uint16_t                    constant_pool_count;
    std::vector<cp_info*>       constant_pool;
    uint16_t                    access_flags;
    uint16_t                    this_class;
    uint16_t                    super_class;
    uint16_t                    interfaces_count;
    std::vector<uint16_t>       interfaces;
    uint16_t                    fields_count;
    std::vector<field_info*>     fields;
    uint16_t                    methods_count;
    std::vector<method_info*>    methods;
    uint16_t                    attributes_count;
    std::vector<attribute_info*> attributes;


    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, magic);

    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, minor_version);

    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, major_version);

    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, constant_pool_count);

    printf("\nmagic: %X\n",magic);
    printf("version: %d.%d\n",major_version,minor_version);
    printf("constant pool count: %d\n",constant_pool_count);

    buildConstantPoolTable(&iter, constant_pool_count, constant_pool);

    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, access_flags);

    printf("access flags: %04X", access_flags);

    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, this_class);
    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, super_class);

    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, interfaces_count);

    std::copy(iter, iter + interfaces_count * 2, std::back_inserter(interfaces));
    *iter += interfaces_count * 2;

    COPY_BYTES_AND_ADVANCE_ITERATOR(iter, fields_count);



    return 0;
}
