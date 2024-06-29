# Dependencias Windows
No windows é necessário instalar os seguintes programas [Mingw](https://www.msys2.org/) e [Cmake](https://cmake.org/download/).
É necessário também o compilador _g++_ de 32 bits e o programa _Make_.
Para instalar eles dentro do Mingw rode os comandos
```
pacman -Syu
pacman -Su
pacman -S mingw-w64-i686-gcc
pacman -S mingw32/mingw-w64-i686-make
```
É necessário adicionar os dois no path

# Compilação

## Linux:

```sh
cmake ./
make
```
## Windows:
```sh
cmake -G "MinGW Makefiles" ./

make # or
mingw32-make
```
# Uso:

```sh
Como usar:
    ./JVM <command> <package/class>
Comandos:
    leitor_exibidor
    jvm
Exemplos:
    ./JVM leitor_exibidor Main
    ./JVM jvm Main
```

# Documentação:
Documentaçaõ gerada por Doxygen, basta abrir o arquivo ./docs/html/index.html
