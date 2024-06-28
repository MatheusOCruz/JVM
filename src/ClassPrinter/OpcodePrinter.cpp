//
// Created by matheus on 4/20/24.
//

#include <iomanip>
#include <sstream>
#include "../../include/ClassPrinter/OpcodePrinter.h"

/**
 * @brief Construtor da classe OpcodePrinter
 * @param _printer Ponteiro para a classe ClassPrinter
 */
std::string OpcodePrinter::CodeToString(u1 _code[], const u4 code_lenght) {
    StringBuffer = "";
    code_iterator = 0;
    code = _code;
    while(code_iterator != code_lenght){
        StringBuffer.append("      ");
        std::ostringstream oss;
        oss << std::setw(3) << code_iterator << ":";
        std::string formatedIndex = oss.str();
        StringBuffer.append(formatedIndex);
        u1 bytecode = code[code_iterator++];
        (this->*bytecodeFuncs[bytecode])();
    }
    return StringBuffer;
}

void OpcodePrinter::nop() {
    StringBuffer.append(" aconst_null\n");
}
void OpcodePrinter::aconst_null() {
    StringBuffer.append(" aconst_null\n");
}
void OpcodePrinter::iconst_m1() {
    StringBuffer.append(" iconst_m1\n");
}
void OpcodePrinter::iconst_0() {
    StringBuffer.append(" iconst_0\n");
}
void OpcodePrinter::iconst_1() {
    StringBuffer.append(" iconst_1\n");
}
void OpcodePrinter::iconst_2() {
    StringBuffer.append(" iconst_2\n");
}
void OpcodePrinter::iconst_3() {
    StringBuffer.append(" iconst_3\n");
}
void OpcodePrinter::iconst_4() {
    StringBuffer.append(" iconst_4\n");
}
void OpcodePrinter::iconst_5() {
    StringBuffer.append(" iconst_5\n");
}
void OpcodePrinter::lconst_0() {
    StringBuffer.append(" lconst_0\n");
}
void OpcodePrinter::lconst_1() {
    StringBuffer.append(" lconst_1\n");
}
void OpcodePrinter::fconst_0() {
    StringBuffer.append(" fconst_0\n");
}
void OpcodePrinter::fconst_1() {
    StringBuffer.append(" fconst_1\n");
}
void OpcodePrinter::fconst_2() {
    StringBuffer.append(" fconst_2\n");
}
void OpcodePrinter::dconst_0() {
    StringBuffer.append(" dconst_0\n");
}
void OpcodePrinter::dconst_1() {
    StringBuffer.append(" dconst_1\n");
}

void OpcodePrinter::bipush() {
    s1 byte = code[code_iterator++];
    StringBuffer.append(" bipush ");
    StringBuffer.append(std::to_string(byte));
    StringBuffer.append("\n");
}

void OpcodePrinter::sipush() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" sipush "); StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::ldc() {
    u1 index = code[code_iterator++];
    StringBuffer.append(" ldc #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("  // "+ Printer->CpEntryAsString(index));
    StringBuffer.append("\n");
}
//TODO: incluir constant_pool como argumento da classe pra pegar valor no indice
void OpcodePrinter::ldc_w() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" ldc_w #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("  // "+ Printer->CpEntryAsString(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::ldc2_w() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" ldc2_w #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("  // "+ Printer->CpEntryAsString(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::iload() {
    u1 index = code[code_iterator++];
    StringBuffer.append(" iload "); StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::lload() {
    u1 index = code[code_iterator++];
    StringBuffer.append(" lload "); StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::fload() {
    u1 index = code[code_iterator++];
    StringBuffer.append(" fload "); StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::dload() {
    u1 index = code[code_iterator++];
    StringBuffer.append(" dload "); StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::aload() {
    u1 index = code[code_iterator++];
    StringBuffer.append(" aload "); StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::iload_0() {
    StringBuffer.append(" iload_0\n");
}
void OpcodePrinter::iload_1() {
    StringBuffer.append(" iload_1\n");
}
void OpcodePrinter::iload_2() {
    StringBuffer.append(" iload_2\n");
}
void OpcodePrinter::iload_3() {
    StringBuffer.append(" iload_3\n");
}
void OpcodePrinter::lload_0() {
    StringBuffer.append(" lload_0\n");
}
void OpcodePrinter::lload_1() {
    StringBuffer.append(" lload_1n");
}
void OpcodePrinter::lload_2() {
    StringBuffer.append(" lload_2\n");
}
void OpcodePrinter::lload_3() {
    StringBuffer.append(" lload_3\n");
}
void OpcodePrinter::fload_0() {
    StringBuffer.append(" fload_0\n");
}
void OpcodePrinter::fload_1() {
    StringBuffer.append(" fload_1\n");
}
void OpcodePrinter::fload_2() {
    StringBuffer.append(" fload_2\n");
}
void OpcodePrinter::fload_3() {
    StringBuffer.append(" fload_3\n");
}
void OpcodePrinter::dload_0() {
    StringBuffer.append(" dload_0\n");
}
void OpcodePrinter::dload_1() {
    StringBuffer.append(" dload_1\n");
}
void OpcodePrinter::dload_2() {
    StringBuffer.append(" dload_2\n");
}
void OpcodePrinter::dload_3() {
    StringBuffer.append(" dload_3\n");
}
void OpcodePrinter::aload_0() {
    StringBuffer.append(" aload_0\n");
}
void OpcodePrinter::aload_1() {
    StringBuffer.append(" aload_1\n");
}
void OpcodePrinter::aload_2() {
    StringBuffer.append(" aload_2\n");
}
void OpcodePrinter::aload_3() {
    StringBuffer.append(" aload_3\n");
}
void OpcodePrinter::iaload() {
    StringBuffer.append(" iaload\n");
}
void OpcodePrinter::laload() {
    StringBuffer.append(" laload\n");
}
void OpcodePrinter::faload() {
    StringBuffer.append(" faload\n");
}
void OpcodePrinter::daload() {
    StringBuffer.append(" daload\n");
}
void OpcodePrinter::aaload() {
    StringBuffer.append(" aaload\n");
}
void OpcodePrinter::baload() {
    StringBuffer.append(" baload\n");
}
void OpcodePrinter::caload() {
    StringBuffer.append(" caload\n");
}
void OpcodePrinter::saload() {
    StringBuffer.append(" saload\n");
}
void OpcodePrinter::istore() {
    u1 index = code[code_iterator++];
    StringBuffer.append(" istore "); StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::lstore() {
    u1 index = code[code_iterator++];
    StringBuffer.append( " lstore "); StringBuffer.append(std::to_string(index));
    StringBuffer.append(" \n ");
}
void OpcodePrinter::fstore() {
    u1 index = code[code_iterator++];
    StringBuffer.append( " fstore "); StringBuffer.append(std::to_string(index));
    StringBuffer.append( " \n " );
}
void OpcodePrinter::dstore() {
    u1 index = code[code_iterator++];
    StringBuffer.append( " dstore "); StringBuffer.append(std::to_string(index));
    StringBuffer.append(" \n ");
}
void OpcodePrinter::astore() {
    u1 index = code[code_iterator++];
    StringBuffer.append( " astore "); StringBuffer.append(std::to_string(index));
    StringBuffer.append(" \n ");
}
void OpcodePrinter::istore_0() {
    StringBuffer.append(" istore_0\n");
}
void OpcodePrinter::istore_1() {
    StringBuffer.append(" istore_1\n");
}
void OpcodePrinter::istore_2() {
    StringBuffer.append(" istore_2\n");
}
void OpcodePrinter::istore_3() {
    StringBuffer.append(" istore_3\n");
}
void OpcodePrinter::lstore_0() {
    StringBuffer.append(" lstore_0\n");
}
void OpcodePrinter::lstore_1() {
    StringBuffer.append(" lstore_1\n");
}
void OpcodePrinter::lstore_2() {
    StringBuffer.append(" lstore_2\n");
}
void OpcodePrinter::lstore_3() {
    StringBuffer.append(" lstore_3\n");
}
void OpcodePrinter::fstore_0() {
    StringBuffer.append(" fstore_0\n");
}
void OpcodePrinter::fstore_1() {
    StringBuffer.append(" fstore_1\n");
}
void OpcodePrinter::fstore_2() {
    StringBuffer.append(" fstore_2\n");
}
void OpcodePrinter::fstore_3() {
    StringBuffer.append(" fstore_3\n");
}
void OpcodePrinter::dstore_0() {
    StringBuffer.append(" dstore_0\n");
}
void OpcodePrinter::dstore_1() {
    StringBuffer.append(" dstore_1\n");
}
void OpcodePrinter::dstore_2() {
    StringBuffer.append(" dstore_2\n");
}
void OpcodePrinter::dstore_3() {
    StringBuffer.append(" dstore_3\n");
}
void OpcodePrinter::astore_0() {
    StringBuffer.append(" astore_0\n");
}
void OpcodePrinter::astore_1() {
    StringBuffer.append(" astore_1\n");
}
void OpcodePrinter::astore_2() {
    StringBuffer.append(" astore_2\n");
}
void OpcodePrinter::astore_3() {
    StringBuffer.append(" astore_3\n");
}
void OpcodePrinter::iastore() {
    StringBuffer.append(" iastore\n");
}
void OpcodePrinter::lastore() {
    StringBuffer.append(" lastore\n");
}
void OpcodePrinter::fastore() {
    StringBuffer.append(" lastore\n");
}
void OpcodePrinter::dastore() {
    StringBuffer.append(" dastore\n");
}
void OpcodePrinter::aastore() {
    StringBuffer.append(" aastore\n");
}
void OpcodePrinter::bastore() {
    StringBuffer.append(" bastore\n");
}
void OpcodePrinter::castore() {
    StringBuffer.append(" castore\n");
}
void OpcodePrinter::sastore() {
    StringBuffer.append(" sastore\n");
}
void OpcodePrinter::pop() {
    StringBuffer.append(" pop\n");
}
void OpcodePrinter::pop2() {
    StringBuffer.append(" pop2\n");
}
void OpcodePrinter::dup() {
    StringBuffer.append(" dup\n");
}
void OpcodePrinter::dup_x1() {
    StringBuffer.append(" dup_x1\n");
}
void OpcodePrinter::dup_x2() {
    StringBuffer.append(" dup_x2\n");
}
void OpcodePrinter::dup2() {
    StringBuffer.append(" dup2\n");
}
void OpcodePrinter::dup2_x1() {
    StringBuffer.append(" dup2_x1\n");
}
void OpcodePrinter::dup2_x2() {
    StringBuffer.append(" dup2_x2\n");
}
void OpcodePrinter::swap() {
    StringBuffer.append(" swap\n");
}
void OpcodePrinter::iadd() {
    StringBuffer.append(" iadd\n");
}
void OpcodePrinter::ladd() {
    StringBuffer.append(" ladd\n");
}
void OpcodePrinter::fadd() {
    StringBuffer.append(" fadd\n");
}
void OpcodePrinter::dadd() {
    StringBuffer.append(" dadd\n");
}
void OpcodePrinter::isub() {
    StringBuffer.append(" isub\n");
}
void OpcodePrinter::lsub() {
    StringBuffer.append(" lsub\n");
}
void OpcodePrinter::fsub() {
    StringBuffer.append(" fsub\n");
}
void OpcodePrinter::dsub() {
    StringBuffer.append(" dsub\n");
}
void OpcodePrinter::imul() {
    StringBuffer.append(" imul\n");
}
void OpcodePrinter::lmul() {
    StringBuffer.append(" lmul\n");
}
void OpcodePrinter::fmul() {
    StringBuffer.append(" fmul\n");
}
void OpcodePrinter::dmul() {
    StringBuffer.append(" dmul\n");
}
void OpcodePrinter::idiv() {
    StringBuffer.append(" idiv\n");
}
void OpcodePrinter::ldiv() {
    StringBuffer.append(" ldiv\n");
}
void OpcodePrinter::fdiv() {
    StringBuffer.append(" fdiv\n");
}
void OpcodePrinter::ddiv() {
    StringBuffer.append(" ddiv\n");
}
void OpcodePrinter::irem() {
    StringBuffer.append(" irem\n");
}
void OpcodePrinter::lrem() {
    StringBuffer.append(" lrem\n");
}
void OpcodePrinter::frem() {
    StringBuffer.append(" frem\n");
}
void OpcodePrinter::drem() {
    StringBuffer.append(" drem\n");
}
void OpcodePrinter::ineg() {
    StringBuffer.append(" ineg\n");
}
void OpcodePrinter::lneg() {
    StringBuffer.append(" lneg\n");
}
void OpcodePrinter::fneg() {
    StringBuffer.append(" fneg\n");
}
void OpcodePrinter::dneg() {
    StringBuffer.append(" dneg\n");
}
void OpcodePrinter::ishl() {
    StringBuffer.append(" ishl\n");
}
void OpcodePrinter::lshl() {
    StringBuffer.append(" lshl\n");
}
void OpcodePrinter::ishr() {
    StringBuffer.append(" ishr\n");
}
void OpcodePrinter::lshr() {
    StringBuffer.append(" lshr\n");
}
void OpcodePrinter::iushr() {
    StringBuffer.append(" iushr\n");
}
void OpcodePrinter::lushr() {
    StringBuffer.append(" lushr\n");
}
void OpcodePrinter::iand() {
    StringBuffer.append(" iand\n");
}
void OpcodePrinter::land() {
    StringBuffer.append(" land\n");
}
void OpcodePrinter::ior() {
    StringBuffer.append(" ior\n");
}
void OpcodePrinter::lor() {
    StringBuffer.append(" lor\n");
}
void OpcodePrinter::ixor() {
    StringBuffer.append(" ixor\n");
}
void OpcodePrinter::lxor() {
    StringBuffer.append(" lxor\n");
}
// index e const
void OpcodePrinter::iinc() {
    u1 index = code[code_iterator++];
    u1 _const = code[code_iterator++];
    StringBuffer.append(" iinc ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append(" by ");
    StringBuffer.append(std::to_string(_const));
    StringBuffer.append("\n");

}
void OpcodePrinter::i2l() {

    StringBuffer.append(" i2l\n");
}
void OpcodePrinter::i2f() {
    StringBuffer.append(" i2f\n");
}
void OpcodePrinter::i2d() {
    StringBuffer.append(" i2d\n");
}
void OpcodePrinter::l2i() {
    StringBuffer.append(" l2i\n");
}
void OpcodePrinter::l2f() {
    StringBuffer.append(" l2f\n");
}
void OpcodePrinter::l2d() {
    StringBuffer.append(" l2d\n");
}
void OpcodePrinter::f2i() {
    StringBuffer.append(" f2i\n");
}
void OpcodePrinter::f2l() {
    StringBuffer.append(" f2l\n");
}
void OpcodePrinter::f2d() {
    StringBuffer.append(" f2d\n");
}
void OpcodePrinter::d2i() {
    StringBuffer.append(" d2i\n");
}
void OpcodePrinter::d2l() {
    StringBuffer.append(" d2l\n");
}
void OpcodePrinter::d2f() {
    StringBuffer.append(" d2f\n");
}
void OpcodePrinter::i2b() {
    StringBuffer.append(" i2b\n");
}
void OpcodePrinter::i2c() {
    StringBuffer.append(" i2c\n");
}
void OpcodePrinter::i2s() {
    StringBuffer.append(" i2s\n");
}
void OpcodePrinter::lcmp() {
    StringBuffer.append(" lcmp\n");
}
void OpcodePrinter::fcmpl() {
    StringBuffer.append(" fcmpl\n");
}
void OpcodePrinter::fcmpg() {
    StringBuffer.append(" fcmpg\n");
}
void OpcodePrinter::dcmpl() {
    StringBuffer.append(" dcmpl\n");
}
void OpcodePrinter::dcmpg() {
    StringBuffer.append(" dcmpg\n");
}
// branchbyte1, branchbyte2
void OpcodePrinter::ifeq() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" ifeq ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::ifne() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" ifne ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::iflt() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" iflt ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::ifge() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" ifge ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::ifgt() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" ifgt ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::ifle() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" ifle ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::if_icmpeq() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" if_icmpeq ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::if_icmpne() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" if_icmpne ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::if_icmplt() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" if_icmplt ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::if_icmpge() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" if_icmpge ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::if_icmpgt() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" if_icmpgt ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::if_icmple() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" if_icmple ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::if_acmpeq() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" if_acmpeq ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::if_acmpne() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" if_acmpne ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::goto_() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" goto ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
} // Usando goto_ para evitar conflito com a palavra reservada goto
void OpcodePrinter::jsr_() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" jsr #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("  // "+ Printer->CpEntryAsString(index));
    StringBuffer.append("\n");
} // Usando jsr_ para evitar conflito com a palavra reservada jsr
void OpcodePrinter::ret_() {
    u1 index = code[code_iterator++];
    StringBuffer.append(" ret ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
} // Usando ret para evitar conflito com a palavra reservada ret

// metodo setCode para configurar o array code e seu tamanho.
// o metodo será chamado antes de qualquer operação
// vai em nome de jeje
void OpcodePrinter::setCode(u1* code, size_t code_size) {
    this->code = code;
    this->code_size = code_size;
    this->code_iterator = 0; // Inicializa code_iterator
}

void OpcodePrinter::tableswitch() {
    std::string buffer;
    size_t start_index = code_iterator;
    u1 index = code[code_iterator++];  // Índice do opcode

    // Verifica se há bytes suficientes para ajustar o padding e ler default, low e high (12 bytes no total)
    size_t padding_end = (code_iterator + 3) & ~3;
    if (padding_end + 12 > code_size) {
        std::cerr << "Erro: Acesso fora dos limites do array 'code' em table.A tester aguento nada" << std::endl;
        return;
    }

    // Ajusta o padding
    code_iterator = padding_end;

    // Lendo default (4 bytes)
    int defaultoffset = (code[code_iterator] << 24) |
                        (code[code_iterator + 1] << 16) |
                        (code[code_iterator + 2] << 8) |
                        code[code_iterator + 3];
    code_iterator += 4;

    // Lendo low (4 bytes)
    int low = (code[code_iterator] << 24) |
              (code[code_iterator + 1] << 16) |
              (code[code_iterator + 2] << 8) |
              code[code_iterator + 3];
    code_iterator += 4;

    // Lendo high (4 bytes)
    int high = (code[code_iterator] << 24) |
               (code[code_iterator + 1] << 16) |
               (code[code_iterator + 2] << 8) |
               code[code_iterator + 3];
    code_iterator += 4;

    // Print default, low e high
    StringBuffer.append(" tableswitch ");
    StringBuffer.append(std::to_string(index));
    //StringBuffer.append(" default: ").append(std::to_string(defaultoffset));
    //StringBuffer.append(" ").append(std::to_string(low));
    StringBuffer.append(" to ").append(std::to_string(high));
    StringBuffer.append("\n");

    // Calcula o número de jump offsets
    int num_offsets = high - low + 1;

    // Verifica se há bytes suficientes para ler os jump offsets
    if (code_iterator + 4 * num_offsets > code_size) {
        std::cerr << "Erro: Acesso fora dos limites do array 'code' em table.B vai em nome de jesus" << std::endl;
        return;
    }

    // Lê os jump offsets
    for (int i = 0; i < num_offsets; i++) {
        int offset = (code[code_iterator] << 24) |
                     (code[code_iterator + 1] << 16) |
                     (code[code_iterator + 2] << 8) |
                     code[code_iterator + 3];
        code_iterator += 4;
        /*
        StringBuffer.append(" offset: ").append(std::to_string(offset));
        StringBuffer.append("\n");*/

        size_t displacement = start_index + offset - 1;
        StringBuffer.append("            "); // Indentação
        // deslocamento
        StringBuffer.append("").append(std::to_string(i)).append(": ").append(std::to_string(displacement));
        //offset
        StringBuffer.append(" (+").append(std::to_string(offset)).append(") ").append("\n");
    }
    StringBuffer.append("            "); // Indentação
    StringBuffer.append("default: ").append(std::to_string(start_index + defaultoffset -1));
    StringBuffer.append(" (+").append(std::to_string(defaultoffset)).append(")\n");

    StringBuffer.append(buffer);
    // mensagem completa
    //std::cout << StringBuffer << std::endl;
}

void OpcodePrinter::lookupswitch() {
    //StringBuffer.clear();  // Limpa o buffer antes de iniciar
    std::string buffer;
    size_t start_index = code_iterator;
    u1 index = code[code_iterator++];  // Índice do opcode

    // Verifica se há padding
    while (code_iterator % 4 != 0) {
        code_iterator++;
    }

    // Verifica se há bytes suficientes para ler o default e npairs (8 bytes no total)
    if (code_iterator + 8 > code_size) {
        std::cerr << "Erro: Acesso fora dos limites do array 'code'.A" << std::endl;
        //StringBuffer.append(buffer);
        return;
    }

    // Lendo default (4 bytes)
    int defaultoffset = (code[code_iterator] << 24) |
                        (code[code_iterator + 1] << 16) |
                        (code[code_iterator + 2] << 8) |
                        code[code_iterator + 3];
    code_iterator += 4;

    // Lendo npairs (4 bytes)
    int npairs = (code[code_iterator] << 24) |
                 (code[code_iterator + 1] << 16) |
                 (code[code_iterator + 2] << 8) |
                 code[code_iterator + 3];
    code_iterator += 4;

    // Print defaultoffset e npairs
    StringBuffer.append(" lookupswitch");
    //StringBuffer.append(std::to_string(index));
    //StringBuffer.append(" default: ").append(std::to_string(defaultoffset));
    StringBuffer.append(" ").append(std::to_string(npairs));
    StringBuffer.append("\n");

    // Verifica se há bytes suficientes para ler os pares de valores de caso / deslocamento de ramificação
    if (code_iterator + 8 * npairs > code_size) {
        std::cerr << "Erro: Acesso fora dos limites do array 'code'. B" << std::endl;
        //StringBuffer.append(buffer);
        return;
    }

    // Lê os pares de valores de caso / deslocamento de ramificação
    for (int i = 0; i < npairs; i++) {
        int match = (code[code_iterator] << 24) |
                    (code[code_iterator + 1] << 16) |
                    (code[code_iterator + 2] << 8) |
                    code[code_iterator + 3];
        code_iterator += 4;

        int offset = (code[code_iterator] << 24) |
                     (code[code_iterator + 1] << 16) |
                     (code[code_iterator + 2] << 8) |
                     code[code_iterator + 3];
        code_iterator += 4;

        size_t displacement = start_index + offset - 1;
        StringBuffer.append("            "); // Indentação
        StringBuffer.append("").append(std::to_string(match)).append(": ");
        // deslocamento
        StringBuffer.append("").append(std::to_string(displacement));
        //offset
        StringBuffer.append(" (+").append(std::to_string(offset)).append(") ").append("\n");

        //size_t displacement = start_index + offset - 1;
        // deslocamento
        //StringBuffer.append("").append(std::to_string(i)).append(": ").append(std::to_string(displacement));
        //offset
       // StringBuffer.append(" (+").append(std::to_string(offset)).append(") ").append("\n");
    }
    StringBuffer.append("            "); // Indentação
    StringBuffer.append("default: ").append(std::to_string(start_index + defaultoffset -1));
    StringBuffer.append(" (+").append(std::to_string(defaultoffset)).append(")\n");

    StringBuffer.append(buffer);
    //  mensagem completa
    //std::cout << StringBuffer << std::endl;
}
void OpcodePrinter::ireturn() {
    StringBuffer.append(" ireturn\n");
}
void OpcodePrinter::lreturn() {
    StringBuffer.append(" lreturn\n");
}
void OpcodePrinter::freturn() {
    StringBuffer.append(" freturn\n");
}
void OpcodePrinter::dreturn() {
    StringBuffer.append(" dreturn\n");
}
void OpcodePrinter::areturn() {
    StringBuffer.append(" areturn\n");
}
void OpcodePrinter::return_() {
    StringBuffer.append(" return\n");
}
void OpcodePrinter::getstatic() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" getstatic #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("  // "+ Printer->CpEntryAsString(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::putstatic() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" putstatic #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("  // "+ Printer->CpEntryAsString(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::getfield() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" getfield #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("  // "+ Printer->CpEntryAsString(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::putfield() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" putfield #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("  // "+ Printer->CpEntryAsString(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::invokevirtual() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" invokevirtual #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("  // "+ Printer->CpEntryAsString(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::invokespecial() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" invokespecial #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("  // "+ Printer->CpEntryAsString(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::invokestatic() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" invokestatic #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("  // "+ Printer->CpEntryAsString(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::invokeinterface() {
// vai ter algo aqui - bloco de notas
}
void OpcodePrinter::invokedynamic() {
    // vai ter algo aqui - bloco de notas
}
void OpcodePrinter::new_() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" new #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("  // "+ Printer->CpEntryAsString(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::newarray() {

    std::string buffer;

    u1 atype = code[code_iterator++];


    //  atype (1 byte) non nexecisa
    //u1 atype = code[code_iterator++];

    // Mapeando o atype para o tipo de array correspondente
    std::string array_type;
    switch (atype) {
        case 4: array_type = "boolean"; break;
        case 5: array_type = "char"; break;
        case 6: array_type = "float"; break;
        case 7: array_type = "double"; break;
        case 8: array_type = "byte"; break;
        case 9: array_type = "short"; break;
        case 10: array_type = "int"; break;
        case 11: array_type = "long"; break;
        default: array_type = "UNKNOWN"; break;
    }

    StringBuffer.append(" newarray ");
    //tipo do array
    StringBuffer.append(" ").append(std::to_string(atype));
    StringBuffer.append(" (").append(array_type).append(")\n");
}
void OpcodePrinter::anewarray() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" anewarray #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::arraylength() {
    StringBuffer.append(" arraylength\n");
}
void OpcodePrinter::athrow() {
    StringBuffer.append(" athrow\n");
}
void OpcodePrinter::checkcast() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" checkcast #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::instanceof() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" instanceof #"); StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::monitorenter() {
    StringBuffer.append(" monitorenter\n");
}
void OpcodePrinter::monitorexit() {
    StringBuffer.append(" monitorexit\n");
}
void OpcodePrinter::wide() {
	u1 instruction = code[code_iterator++];
	u1 indexbyte1 = code[code_iterator++];
	u1 indexbyte2 = code[code_iterator++];
	u2 index = (((u2) indexbyte1) << 8) | indexbyte2;

	StringBuffer.append(" wide");
	switch((WideOp) instruction) {
	case(WideOp::WIDE_iload):
		StringBuffer.append(" iload ");
		StringBuffer.append(std::to_string(index));
		StringBuffer.append("\n");
		break;
	case(WideOp::WIDE_fload):
		StringBuffer.append(" fload ");
		StringBuffer.append(std::to_string(index));
		StringBuffer.append("\n");
		break;
	case(WideOp::WIDE_lload):
		StringBuffer.append(" lload ");
		StringBuffer.append(std::to_string(index));
		StringBuffer.append("\n");
		break;
	case(WideOp::WIDE_aload):
		StringBuffer.append(" aload ");
		StringBuffer.append(std::to_string(index));
		StringBuffer.append("\n");
		break;
	case(WideOp::WIDE_dload):
		StringBuffer.append(" dload ");
		StringBuffer.append(std::to_string(index));
		StringBuffer.append("\n");
		break;
	case(WideOp::WIDE_istore):
		StringBuffer.append(" istore ");
		StringBuffer.append(std::to_string(index));
		StringBuffer.append("\n");
		break;
	case(WideOp::WIDE_fstore):
		StringBuffer.append(" fstore ");
		StringBuffer.append(std::to_string(index));
		StringBuffer.append("\n");
		break;
	case(WideOp::WIDE_astore):
		StringBuffer.append(" astore ");
		StringBuffer.append(std::to_string(index));
		StringBuffer.append("\n");
		break;
	case(WideOp::WIDE_lstore):
		StringBuffer.append(" lstore ");
		StringBuffer.append(std::to_string(index));
		StringBuffer.append("\n");
		break;
	case(WideOp::WIDE_dstore):
		StringBuffer.append(" dstore ");
		StringBuffer.append(std::to_string(index));
		StringBuffer.append("\n");
		break;
	case(WideOp::WIDE_ret):
		StringBuffer.append(" ret ");
		StringBuffer.append(std::to_string(index));
		StringBuffer.append("\n");
		break;
	case(WideOp::WIDE_iinc):
		u1 constbyte1 = code[code_iterator++];
		u1 constbyte2 = code[code_iterator++];
		u2 _const = (((u2)constbyte1) << 8) | constbyte2;

		StringBuffer.append(" iinc ");
		StringBuffer.append(std::to_string(index));
		StringBuffer.append(" by ");
		StringBuffer.append(std::to_string(_const));
		StringBuffer.append("\n");
		break;
	}
}

void OpcodePrinter::multianewarray() {


    // Ler indexbyte1, indexbyte2 e dimensõess
    u1 indexbyte1 = code[code_iterator++];
    u1 indexbyte2 = code[code_iterator++];
    u1 dimensions = code[code_iterator++];

    // índice
    u2 index = (indexbyte1 << 8) | indexbyte2;

    // Verificadimensão
    if (dimensions < 1) {
        StringBuffer.append("Erro: O valor de 'dimensions' deve ser maior ou igual a 1.\n");
        return;
    }


    StringBuffer.append(" multianewarray ");
    StringBuffer.append("# ").append(std::to_string(index));
    StringBuffer.append(" dim ").append(std::to_string(dimensions)).append("\n");

}

void OpcodePrinter::ifnull() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" ifnull ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::ifnonnull() {
    short index = code_iterator - 1;
    index += static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++];

    StringBuffer.append(" ifnonnull ");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::goto_w() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" goto_w #");
    StringBuffer.append(std::to_string(index));
    StringBuffer.append("\n");
}
void OpcodePrinter::jsr_w() {
    u2 index = static_cast<short>(code[code_iterator++])<<8 | code[code_iterator++] ;

    StringBuffer.append(" jsr_w #"); StringBuffer.append(std::to_string(index));
	StringBuffer.append("  // "+ Printer->CpEntryAsString(index));
    StringBuffer.append("\n");
}
