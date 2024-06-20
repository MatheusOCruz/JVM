//
// Created by matheus on 4/2/24.
//

#ifndef JVM_CLASSLOADER_H
#define JVM_CLASSLOADER_H
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <map>
#include <iterator>
#include <memory>
#include <cstring>
#include <regex>
#include <unordered_map>
#include "ClassFileEnums.h"
#include "ClassFileStructs.h"
#include "../typedefs.h"
#include "ClassLoaderErrors.h"

/**
 * @brief Classe responsavel por abrir arquivo .class e formar a struct class_file que o representa
 */
class ClassLoader {
public:
     //! Construtor para o leitor-exibidor, não recebe argumentos
	ClassLoader() {class_files = new std::unordered_map<std::string,class_file*>;}
     //! Constutor para o interpretador, recebe um ponteiro pro method area para atualizar a lista de classes carregadas
	explicit ClassLoader(std::unordered_map<std::string ,class_file*>* _class_files) : class_files(_class_files) {}

    ~ClassLoader() = default;


    /**
     * @brief Checa se a classe ja esta carregada, se sim retorna sua representação, caso contrário,
     * carrega a classe
     * @param ClassName Nome da classe a ser retornada
     * @return
     */
    class_file* GetClass(const std::string ClassName);

    /**
     * @brief recebe o nome de uma classe, e retorna sua forma class_file a partir de
     * arquivo .class, além de fazer verificações como versão do arquivo e magic bytes
     * @param ClassName Nome da classe a ser carregada
     */
    void LoadClass(const std::string ClassName);

private:

    /**
     * @brief Transforma o nome da classe no path para carregada a partir do
     * diretorio do executável, ex: Main -> ./Main.cpp
     * @param ClassName Nome da classe a ser carregada
     */
    void LoadFile(const std::string& ClassName);



    // funcoes que leem o iterador do buffer  retoram a proxima entrada
    // convertida pra little endian e incrementam iterador

    //! le 1 byte do arquivo e incrementa iterador
    u1 read_u1();
    //! le 2 bytes do arquivo e incrementa iterador
    u2 read_u2();
    //! le 4 bytes do arquivo e incrementa iterador
    u4 read_u4();
    /**
     * @brief cria vetor de u2 a partir de .class
     * @param length quantidade de elementos
     * @return vetor com lenght elementos u2
     */
	std::vector<u2> *read_vec_u2(int length);
    /**
     * @brief cria vetor de u1 a partir de .class
     * @param length quantidade de elementos
     * @return vetor com lenght elementos u1
     */
	std::vector<u1> *read_vec_u1(int length);

    /**
     * @brief Constrói uma única entrada da tabela de pool constante a partir do buffer do arquivo .class.
     *
     * @return Retorna 1 caso a entrada ocupe 2 posições(long e double)
     */
    int  BuildConstantPoolInfo();
    /**
     *
     * @brief Constrói uma única entrada na tabela de fields a partir do buffer do arquivo .class.
     *
     */
    void BuildFieldInfo();
    /**
     * @brief Constrói uma única entrada de informações de método a partir do buffer do arquivo .class.
     */
    void BuildMethodInfo();
    /**
     * @brief Constrói uma única entrada de informações de atributo a partir do buffer do arquivo .class.
     *
     * Este método lê uma entrada de atributo da classe, campos ou métodos. Atributos podem incluir dados como
     * o código do método, exceções,entre outros. A estrutura resultante armazena essas informações adicionais.
     *
     * @return attribute_info* Ponteiro para a estrutura attribute_info preenchida.
     */
    attribute_info* BuildAttributeInfo();
    //! verifica se bytes iniciais do arquivo sao 0xcafebabe
    void CheckMagic();
    //! verifica se arquivo foi compilado em versão até Java 8
    void CheckVersion();


    /**
      * @brief Itera sobre o contador de pool constante para gerar todas as entradas na tabela de pool constante.
      *
      * Este método lê o número de entradas especificado pelo campo `constant_pool_count` no arquivo .class e utiliza
      * o método `BuildConstantPoolInfo` para construir cada entrada do pool constante. O pool constante contém
      * diversas constantes usadas em outras partes do arquivo .class.
      */
    void BuildConstantPoolTable();
    /**
     * @brief Itera sobre o contador de interfaces para gerar todas as entradas na tabela de interfaces.
     *
     * Este método lê o número de entradas especificado pelo campo `interfaces_count` no arquivo .class e constrói
     * cada entrada na tabela de interfaces. A tabela de interfaces lista todas as interfaces que a classe implementa.
     */
    void BuildInterfaces();
    /**
     * @brief Itera sobre o contador de campos para gerar todas as entradas na tabela de campos.
     *
     * Este método lê o número de entradas especificado pelo campo `fields_count` no arquivo .class e utiliza
     * o método `BuildFieldInfo` para construir cada entrada na tabela de campos. A tabela de campos armazena
     * informações sobre todas as variáveis de instância ou de classe (campos) da classe.
     */
    void BuildFields();
    /**
     * @brief Itera sobre o contador de métodos para gerar todas as entradas na tabela de métodos.
     *
     * Este método lê o número de entradas especificado pelo campo `methods_count` no arquivo .class e utiliza
     * o método `BuildMethodInfo` para construir cada entrada na tabela de métodos. A tabela de métodos armazena
     * informações sobre todos os métodos definidos na classe.
     */
    void BuildMethods();
    /**
     * @brief Itera sobre o contador de atributos para gerar todas as entradas na tabela de atributos.
     *
     * Este método lê o número de entradas especificado pelo campo `attributes_count` no arquivo .class e utiliza
     * o método `BuildAttributeInfo` para construir cada entrada na tabela de atributos. A tabela de atributos
     * armazena informações adicionais sobre a classe, como o código dos métodos, exceções, anotações, entre outros.
     */
    void BuildAttributes();
    /**
     * @brief Versão alternativa do build attributes usada por method_info e field_info
     *
     * Assim como o class_file em si, os method_info e field_info também possuem atributos,
     * essa função constoi seus respectivos vetores de attribute_info
     *
     * @param attribute_count quantidade de atributos
     * @param attributes ponteiro pro vetor de atributos do field/method_info
     */
    void BuildAttributes(int attribute_count, std::vector<attribute_info*> &attributes); // pro attributes dentro do field e method info

    //! verifica formatação de arquivos
    void FormatCheck();

    std::vector<uint8_t>*       file_buffer{}; // pra poder liberar o arquivo dps
    buffer_iterator             iter; // ler bytes sem ter q recalcular o offset
    class_file*                 current_file{};
    std::unordered_map<std::string ,class_file*>* class_files;


};


#endif //JVM_CLASSLOADER_H
