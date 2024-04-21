//
// Created by matheus on 4/11/24.
//

#include "../include/Jvm.h"

void Jvm::Init() {

}

void Jvm::Run(){
    auto TesteLoader = new ClassLoader();

    TesteLoader->LoadMain(main_file);


}

