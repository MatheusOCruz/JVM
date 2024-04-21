//
// Created by matheus on 4/9/24.
//

#include "../include/ClassPrinter.h"

void ClassPrinter::Run() {
    auto TesteLoader = new ClassLoader();
    TesteLoader->LoadMain(main_file);
}

