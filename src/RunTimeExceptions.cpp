//
// Created by matheus on 4/28/24.
//

#include "../include/RunTimeExceptions.h"

//TODO: formatar msg de erro
const char *ArithmeticException::what() const noexcept {
    return exception::what();
}

const char *NegativeArraySizeException::what() const noexcept {
    return exception::what();
}
