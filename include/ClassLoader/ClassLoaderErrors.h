//
// Created by matheus on 4/20/24.
//

#ifndef JVM_CLASSLOADERERRORS_H
#define JVM_CLASSLOADERERRORS_H

#include <exception>
#include <string>
#include "../typedefs.h"
class ClassLoaderError : public std::exception {

};

//
class ClassFormatError : ClassLoaderError {
public:
    explicit ClassFormatError(const char* _class_name) : class_name(_class_name) {}
    const char* what() const noexcept override;
private:
    const char* class_name;
};

// usado pra indentificar um arquivo .class que usa representacao
// fora da suportada, ou seja, qualquer coisa dps do java8
class UnsupportedClassVersionError :  ClassLoaderError {
public:
    explicit UnsupportedClassVersionError(u2 _major_version) : major_version(_major_version) {};
    const char* what() const noexcept override;
private:
    const u2 major_version;
};

// caso nao seja possivel encontrar o arquivo
// que defina a classe sendo carregada
class NoClassDefFoundError : ClassLoaderError{
public:
    explicit NoClassDefFoundError(const char* _class_name) : class_name(_class_name) {}
    const char* what() const noexcept override;
private:
    const char* class_name;
};

// caso a superclasse de C seja uma interface, ou
// caso uma superinterface de C nao seja uma interface
class IncompatibleClassChangeError : ClassLoaderError{

};

// caso qualquer superclasse da classe C
// tenha C como superclasse, ou
// casp qualquer superinterface de C seja C
class ClassCircularityError: ClassLoaderError{

};

#endif //JVM_CLASSLOADERERRORS_H
