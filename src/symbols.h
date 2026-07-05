

#ifndef ALBERT_SRC_SYMBOLS_H
#define ALBERT_SRC_SYMBOLS_H

#include "main.h"
#include <string>
#include <vector>
#include <map>

class Symbol {
public:
    std::string name;
    Addr address;
    uint8_t type;
    Symbol(const std::string& name, Addr address, uint8_t type)
        : name(name), address(address), type(type) {}
};

extern std::multimap<Addr, Symbol> gSymbolsByAddress;
extern std::multimap<std::string, Symbol> gSymbolsByName;

void add_global_symbol(const std::string& name, Addr address, uint8_t type);

#endif // ALBERT_SRC_SYMBOLS_H