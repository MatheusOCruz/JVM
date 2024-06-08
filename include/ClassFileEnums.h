//
// Created by matheus on 4/2/24.
//

#ifndef JVM_CLASSFILEENUMS_H
#define JVM_CLASSFILEENUMS_H
#include <cstdint>


enum class ClassAccessFlag: uint16_t {
	ACC_PUBLIC 	= 0x0001, // Declared public; may be accessed from outside its package.
	ACC_FINAL =	0x0010, // Declared final; no subclasses allowed.
	ACC_SUPER = 0x0020, // Treat superclass methods specially when invoked by the invokespecial instruction.
	ACC_INTERFACE = 0x0200, // Is an interface, not a class.
	ACC_ABSTRACT = 0x0400, // Declared abstract; must not be instantiated.
	ACC_SYNTHETIC = 0x1000, // Declared synthetic; not present in the source code.
	ACC_ANNOTATION = 0x2000, // Declared as an annotation type.
	ACC_ENUM = 0x4000 // Declared as an enum type. 
};

enum class FieldAccessFlag: uint16_t {
    ACC_PUBLIC = 0x0001,         // Declarado público; pode ser acessado de fora do seu pacote.
    ACC_PRIVATE = 0x0002,        // Declarado privado; acessível apenas dentro da classe definidora.
    ACC_PROTECTED = 0x0004,      // Declarado protegido; pode ser acessado dentro de subclasses.
    ACC_STATIC = 0x0008,         // Declarado estático.
    ACC_FINAL = 0x0010,          // Declarado final; não pode ser sobrescrito.
    ACC_SYNCHRONIZED = 0x0020,   // Declarado synchronized; a invocação é envolta por um lock de monitor.
    ACC_VOLATILE = 0x0040,       // Declarado volatile; não pode ser armazenado em cache.
    ACC_TRANSIENT = 0x0080,      // Declarado transient; não é escrito ou lido por um gerenciador de objeto persistente.
    ACC_NATIVE = 0x0100,         // Declarado native; implementado em uma linguagem diferente de Java.
    ACC_INTERFACE = 0x0200,      // Declarado uma interface, não uma classe.
    ACC_ABSTRACT = 0x0400,       // Declarado abstract; não pode ser instanciado.
    ACC_STRICT = 0x0800,         // Declarado strictfp; modo de ponto flutuante é FP-strict.
    ACC_SYNTHETIC = 0x1000,      // Declarado synthetic; não está presente no código-fonte.
    ACC_ANNOTATION = 0x2000,     // Declarado como um tipo de anotação.
    ACC_ENUM = 0x4000            // Declarado como um tipo enum.
};

enum class MethodAccessFlag: uint16_t {
	ACC_PUBLIC = 0x0001,
	ACC_PRIVATE = 0x0002,
	ACC_PROTECTED = 0x0004,
	ACC_STATIC = 0x0008,
	ACC_FINAL = 0x0010,
	ACC_SYNCHRONIZED = 0x0020,
	ACC_BRIDGE = 0x0040,
	ACC_VARARGS = 0x0080,
	ACC_NATIVE = 0x0100,
	ACC_ABSTRACT = 0x0400,
	ACC_STRICT = 0x0800,
	ACC_SYNTHETIC = 0x1000,
};


enum class ConstantPoolTag : uint8_t {
    CONSTANT_Utf8 = 1,
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_Class = 7,
    CONSTANT_String = 8,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_NameAndType = 12,
    CONSTANT_MethodHandle = 15,
    CONSTANT_MethodType = 16,
    CONSTANT_InvokeDynamic = 18
};

enum class AttributeType {
    ConstantValue  = 0,
    Code           = 1,
    Exceptions     = 3,
    InnerClasses   = 4,
    Synthetic      = 6,
    SourceFile     = 8,
    NotImplemented = -1
};
#endif //JVM_CLASSFILEENUMS_H
