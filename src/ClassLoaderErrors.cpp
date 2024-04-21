//
// Created by matheus on 4/20/24.
//

#include "../include/ClassLoaderErrors.h"


//TODO: formatar msg de retorno
const char *ClassFormatError::what() const noexcept {
    return exception::what();
}


const char *UnsupportedClassVersionError::what() const noexcept {
    return exception::what();
}

const char *NoClassDefFoundError::what() const noexcept {
    return exception::what();
}
