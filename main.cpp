#include <bits/stdc++.h>
#include <fstream>


#define COPY_BYTES_AND_ADVANCE_ITERATOR(iter, X) std::copy(iter, iter + sizeof(X), reinterpret_cast<uint8_t*>(&X)); iter += sizeof(X);



union Constant_Pool_Entry{
    struct Constant_Class_info{
        uint8_t tag;
        uint16_t name_index;
    };
    struct Constant_Fieldref_info{
        uint8_t tag;
        uint16_t class_index;
        uint16_t name_and_type_index;
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
    std::vector<uint8_t> info;
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
