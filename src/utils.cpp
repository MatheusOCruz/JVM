//
// Created by matheus on 6/15/24.
//

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif
#include <string>
#include <iostream>

namespace utils {
    /**
     * @return Path em que a JVM esta sendo executada
     */
    std::string GetCWD() {
        char cwd[PATH_MAX];
        #ifdef _WIN32
        if (!GetCurrentDirectoryA(sizeof(cwd), cwd)){
                    std::cerr<<"num deu pra pegar o dir atual";
                }
        #else
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            std::cerr << "num deu pra pegar o dir atual";
        }
        #endif
        return {cwd};
    }
}