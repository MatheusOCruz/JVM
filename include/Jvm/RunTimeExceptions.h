//
// Created by matheus on 4/28/24.
//

#ifndef JVM_RUNTIMEEXCEPTIONS_H
#define JVM_RUNTIMEEXCEPTIONS_H


#include <exception>
/**
 * @brief Superclasse para os erros em tempo de execuçaõ da jvm
 */
class RunTimeExceptions : public std::exception  {};


// usado quando ocorre a tentativa de uma divisao por 0
class ArithmeticException : RunTimeExceptions{
public:
    ArithmeticException() = default;
    const char* what() const noexcept override;
};
class NegativeArraySizeException : RunTimeExceptions{
public:
    NegativeArraySizeException() = default;
    const char* what() const noexcept override;
};


#endif //JVM_RUNTIMEEXCEPTIONS_H
