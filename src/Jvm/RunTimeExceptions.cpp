//
// Created by matheus on 4/28/24.
//

#include "../../include/Jvm/RunTimeExceptions.h"


const char *ArithmeticException::what() const noexcept {
    return exception::what();
}

const char *NegativeArraySizeException::what() const noexcept {
    return exception::what();
}
