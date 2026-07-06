

#ifndef ALBERT_SRC_SYMBOLS_H
#define ALBERT_SRC_SYMBOLS_H

#include "main.h"
#include <string>
#include <vector>

class Type;

class FunctionSymbolInfo {
public:
    std::vector<std::string> arg_names; // names of the arguments, if known
};

/**
 * A symbol is an entity with a name and a type.
 *
 * Symbols can have an absolute address in memory. They are referenced from
 * labels.
 *
 * Alternatively, Symbols can be located at an offset from a base address,
 * such as a member of a class or struct.
 *
 * TODO: not sure yet if we want to store constants as well.
 *
 * If a symbol defines a function, it hold additional information. It stores
 * the names of the arguments (the types are stored in the Type object), and
 * the return type.
 *
 * TODO: `virtual` should be stored here as well!
 */
class Symbol {
public:
    // The undecorated name of the symbol
    std::string name { };
    Type* type { nullptr }; // the type of the symbol, if known
    Addr address { 0xFFFFFFFF };
    FunctionSymbolInfo *info { nullptr }; // additional info for functions
    Symbol(const std::string& name, Type* type=nullptr, Addr address=0xFFFFFFFF, FunctionSymbolInfo* info=nullptr)
        : name(name), type(type), address(address), info(info) {}
    ~Symbol() {
        delete info;
    }
};



#endif // ALBERT_SRC_SYMBOLS_H