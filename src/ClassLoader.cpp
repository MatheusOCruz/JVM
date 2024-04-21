//
// Created by matheus on 4/2/24.
//

#include "../include/ClassLoader.h"

void ClassLoader::LoadMain(char *nomeArquivo) {

    class_files[nomeArquivo] = LoadClass(nomeArquivo);
    PrintConstantPoolTable(class_files[nomeArquivo]);
}


class_file* ClassLoader::LoadClass(const char *nomeArquivo) {
    current_file = new class_file;

    LoadFile(nomeArquivo);
    CheckMagic();
    CheckVersion();
    BuildConstantPoolTable();
    current_file->access_flags        = read_u2();
    current_file->this_class          = read_u2();
    current_file->super_class         = read_u2();
    BuildInterfaces();
    BuildFields();
    BuildMethods();
    BuildAttributes();

    FormatCheck();



    delete file_buffer;

    return current_file;

}

void ClassLoader::LoadFile(const char *nomeArquivo) {
    std::ifstream arquivo(nomeArquivo, std::ios::binary);

    if (!arquivo) {
        std::cerr << "Não foi possível abrir o arquivo: " << nomeArquivo << std::endl;
        return;
    }

    // Obter o tamanho do arquivo
    arquivo.seekg(0, std::ios::end);
    std::streampos tamanhoArquivo = arquivo.tellg();
    arquivo.seekg(0, std::ios::beg);

    // Ler o arquivo byte a byte e armazená-lo no vetor buffer
    file_buffer = new std::vector<uint8_t>(tamanhoArquivo);
    arquivo.read(reinterpret_cast<char*>(file_buffer->data()), tamanhoArquivo);

    iter = file_buffer->begin();
    arquivo.close();
}

u1 ClassLoader::read_u1() {
    u1 temp;
    std::copy(iter, iter + sizeof(temp), reinterpret_cast<uint8_t*>(&temp));
    iter+= sizeof(temp);
    return temp;
}

u2 ClassLoader::read_u2() {
    u2 temp;
    std::copy(iter, iter + sizeof(temp), reinterpret_cast<uint8_t*>(&temp));
    iter+= sizeof(temp);
    temp = __builtin_bswap16(temp);
    return temp;
}

u4 ClassLoader::read_u4() {
    u4 temp;
    std::copy(iter, iter + sizeof(temp), reinterpret_cast<uint8_t*>(&temp));
    iter+= sizeof(temp);
    temp = __builtin_bswap32(temp);
    return temp;
}

template<typename T>
std::vector<T> *ClassLoader::read_vec(int length) {
    auto temp = new std::vector<T>();
    temp->reserve(length);

    std::copy(iter, iter + (length* sizeof(T)), std::back_inserter(*temp));
    iter += (length* sizeof(T));

    return temp;
}

void ClassLoader::BuildConstantPoolInfo() {
    auto Entry = new cp_info{};

    switch (Entry->tag = static_cast<ConstantPoolTag>(read_u1())) {
        case ConstantPoolTag::CONSTANT_Utf8: {

            Entry->length    = read_u2();
            Entry->bytes_vec = read_vec<u1>(Entry->length);
            break;
        }
        case ConstantPoolTag::CONSTANT_Integer:
        case ConstantPoolTag::CONSTANT_Float: {

            Entry->bytes = read_u4();

            break;
        }
        case ConstantPoolTag::CONSTANT_Long:
        case ConstantPoolTag::CONSTANT_Double: {

            Entry->high_bytes = read_u4();
            Entry->low_bytes  = read_u4();

            break;
        }
        case ConstantPoolTag::CONSTANT_Class: {

            Entry->name_index = read_u2();

            break;
        }
        case ConstantPoolTag::CONSTANT_String: {

            Entry->string_index = read_u2();

            break;
        }
        case ConstantPoolTag::CONSTANT_Fieldref:
        case ConstantPoolTag::CONSTANT_Methodref:
        case ConstantPoolTag::CONSTANT_InterfaceMethodref: {

            Entry->class_index         = read_u2();
            Entry->name_and_type_index = read_u2();

            break;
        }
        case ConstantPoolTag::CONSTANT_NameAndType: {

            Entry->name_index       = read_u2();
            Entry->descriptor_index = read_u2();

            break;
        }
        case ConstantPoolTag::CONSTANT_MethodHandle:

            Entry->reference_kind  = read_u1();
            Entry->reference_index = read_u2();

            break;
        case ConstantPoolTag::CONSTANT_MethodType:

            Entry->descriptor_index = read_u2();

            break;
        case ConstantPoolTag::CONSTANT_InvokeDynamic:

            Entry->bootstrap_method_attr_index = read_u2();
            Entry->name_and_type_index = read_u2();

            break;
        default:
            throw std::runtime_error("entrada da constant_pool nao existe amigao\n");
    }
    current_file->constant_pool->push_back(Entry);
    // reboco deselagante pq pelo visto esses trem pega 2 entradas
    if(Entry->tag == ConstantPoolTag::CONSTANT_Long || Entry->tag == ConstantPoolTag::CONSTANT_Double)
        current_file->constant_pool->push_back(new cp_info{});
}

attribute_info* ClassLoader::BuildAttributeInfo() {
    auto Entry = new attribute_info{};

    Entry->attribute_name_index = read_u2();
    Entry->attribute_length     = read_u4();
    Entry->info                 = read_vec<u1>(Entry->attribute_length);

    return Entry;
}

field_info* ClassLoader::BuildFieldInfo() {
    auto Entry = new field_info{};

    Entry->access_flags     = read_u2();
    Entry->name_index       = read_u2();
    Entry->descriptor_index = read_u2();
    Entry->attributes_count = read_u2();
    Entry->attributes       = new std::vector<attribute_info*>;

    BuildAttributes(Entry->attributes_count, *Entry->attributes);

    return Entry;
}

method_info* ClassLoader::BuildMethodInfo() {
    auto Entry = new method_info{};

    Entry->access_flags     = read_u2();
    Entry->name_index       = read_u2();
    Entry->descriptor_index = read_u2();
    Entry->attributes_count = read_u2();
    Entry->attributes       = new std::vector<attribute_info*>;
    BuildAttributes(Entry->attributes_count, *Entry->attributes);

    return Entry;
}

void ClassLoader::BuildConstantPoolTable() {
    current_file->constant_pool_count = read_u2();
    current_file->constant_pool = new std::vector<cp_info*>;
    current_file->constant_pool->reserve(current_file->constant_pool_count);
    current_file->constant_pool->push_back(new cp_info{}); // id 0 n conta
    for(int i = 0; i< current_file->constant_pool_count-1; i++){
        BuildConstantPoolInfo();
    }

}
void ClassLoader::BuildInterfaces(){
    current_file->interfaces_count = read_u2();
    current_file->interfaces = new std::vector<u2>;
    current_file->interfaces = read_vec<u2>(current_file->interfaces_count);
}

void ClassLoader::BuildFields() {
    current_file->fields_count = read_u2();
    current_file->fields = new std::vector<field_info*>;
    current_file->fields->reserve(current_file->fields_count);
    for (int i = 0; i < current_file->fields_count; ++i) {
        current_file->fields->push_back(BuildFieldInfo());
    }
}

void ClassLoader::BuildMethods() {
    current_file->methods_count = read_u2();
    current_file->methods = new std::vector<method_info*>;
    current_file->methods->reserve(current_file->methods_count);
    for (int i = 0; i < current_file->methods_count; ++i) {
        current_file->methods->push_back(BuildMethodInfo());
    }
}

void ClassLoader::BuildAttributes() {
    current_file->attributes_count = read_u2();
    current_file->attributes = new std::vector<attribute_info*>;
    current_file->attributes->reserve(current_file->attributes_count);
    for (int i = 0; i < current_file->attributes_count; ++i) {
        current_file->attributes->push_back(BuildAttributeInfo());
    }
}

void ClassLoader::BuildAttributes(int _attributes_count, std::vector<attribute_info *> &_attributes) {
    _attributes.reserve(_attributes_count);
    for (int i = 0; i < _attributes_count; ++i) {
        _attributes.push_back(BuildAttributeInfo());
    }
}
void ClassLoader::CheckMagic() {
    current_file->magic = read_u4();

    if (current_file->magic != 0xCAFEBABE)
        throw ClassFormatError("first four bytes must contain the right magic number");
}


void ClassLoader::CheckVersion() {
    current_file->minor_version = read_u2();
    current_file->major_version = read_u2();

    if(current_file->major_version > 52){
        throw UnsupportedClassVersionError(current_file->major_version);
    }
}

void ClassLoader::FormatCheck() {
    // magic e checado no comeco

    if(!(iter == file_buffer->end()))
        throw ClassFormatError("The class file must not be truncated or have extra bytes at the end.");

    /*
    TODO:he constant pool must satisfy the constraints documented throughout §4.4.
         For example, each CONSTANT_Class_info structure in the constant pool must contain
         in its name_index item a valid constant pool index for a CONSTANT_Utf8_info
         structure.
         • All field references and method references in the constant pool must have valid
         names, valid classes, and valid descriptors (§4.3).
     */
}


//TODO: tem que terminar os print
void ClassLoader::PrintConstantPoolTable(class_file* ClassFile) {
    int i = 0;

    for(auto Entry : *ClassFile->constant_pool){
       if(static_cast<uint8_t>(Entry->tag) == 0) continue; // pra pular a primeira entrada (index 0 n tem nada)
        std::cout<<"["<<++i<<"]";
        switch (Entry->tag) {
            case ConstantPoolTag::CONSTANT_Utf8: {

                std::cout<<"CONSTANT_Utf8\n";
                std::cout<<"  lenght: "<<Entry->length<<"\n  ";
                // futuramente precisamos de uma funcao pra lidar com utf8 e os char de 16 bits do java
                std::cout.write(reinterpret_cast<char*>(Entry->bytes_vec->data()), Entry->length);
                std::cout<<"\n\n";
                break;
            }

            case ConstantPoolTag::CONSTANT_Integer:{

                std::cout<<"CONSTANT_Integer\n";
                std::cout<<"  Bytes: 0x"<< std::hex << Entry->bytes<<"\n\n"<< std::dec;

                break;
            }
            case ConstantPoolTag::CONSTANT_Float: {

                std::cout<<"CONSTANT_Float\n";
                std::cout<<"  Bytes: 0x"<< std::hex << Entry->bytes<<"\n\n"<< std::dec;

                break;
            }

            case ConstantPoolTag::CONSTANT_Long:{

                std::cout<<"CONSTANT_Long\n";
                std::cout<<"  bytes: 0x" << std::hex << Entry->high_bytes << Entry->low_bytes <<"\n\n"<< std::dec;

                break;
            }
            case ConstantPoolTag::CONSTANT_Double: {

                std::cout<<"CONSTANT_Double\n";
                std::cout<<"  bytes: 0x" << std::hex << Entry->high_bytes << Entry->low_bytes <<"\n\n"<< std::dec;

                break;
            }
            case ConstantPoolTag::CONSTANT_Class: {

                std::cout<<"CONSTANT_Class\n";
                std::cout<<"  Class name: "<< Entry->name_index <<"\n\n";

                break;
            }
            case ConstantPoolTag::CONSTANT_String: {

                std::cout<<"CONSTANT_String\n";
                std::cout<<"  string index: "<< Entry->string_index <<"\n\n";

                break;
            }
            case ConstantPoolTag::CONSTANT_Fieldref:{
                std::cout<<"CONSTANT_Fieldref\n";

                std::cout<<"  class index:         "<<Entry->class_index<<"\n";
                std::cout<<"  name and type index: "<<Entry->name_and_type_index<<"\n\n";

                break;
            }
            case ConstantPoolTag::CONSTANT_Methodref:{
                std::cout<<"CONSTANT_Methodref\n";

                std::cout<<"  class index:         "<<Entry->class_index<<"\n";
                std::cout<<"  name and type index: "<<Entry->name_and_type_index<<"\n\n";

                break;
            }
            case ConstantPoolTag::CONSTANT_InterfaceMethodref: {

                std::cout<<"CONSTANT_InterfaceMethodref\n";

                std::cout<<"  class index:         "<<Entry->class_index<<"\n";
                std::cout<<"  name and type index: "<<Entry->name_and_type_index<<"\n\n";

                break;
            }

            case ConstantPoolTag::CONSTANT_NameAndType: {

                std::cout<<"CONSTANT_NameAndType\n";

                std::cout<<"  name index:       "<<Entry->name_index<<"\n";
                std::cout<<"  descriptor index: "<<Entry->descriptor_index<<"\n\n";


                break;
            }

            case ConstantPoolTag::CONSTANT_MethodHandle:
                std::cout<<"CONSTANT_MethodHandle\n";

                std::cout<<"  reference kind:  "<<Entry->reference_kind<<"\n";
                std::cout<<"  reference index: "<<Entry->reference_index<<"\n\n";

                break;
            case ConstantPoolTag::CONSTANT_MethodType:
                std::cout<<"CONSTANT_MethodType\n";
                std::cout<<"  descriptor index: "<<Entry->descriptor_index<<"\n\n";

                break;
            case ConstantPoolTag::CONSTANT_InvokeDynamic:
                std::cout<<"CONSTANT_InvokeDynamic\n";

                std::cout<<"  bootstrap method attr index:  "<<Entry->bootstrap_method_attr_index<<"\n";
                std::cout<<"  name and type index:          "<<Entry->name_and_type_index<<"\n\n";

                break;

            default:
                break;
        }
    }
}










