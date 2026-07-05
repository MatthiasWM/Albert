

#include "symbols.h"

std::multimap<Addr, Symbol> gSymbolsByAddress;
std::multimap<std::string, Symbol> gSymbolsByName;


void add_global_symbol(const std::string& name, Addr address, uint8_t type)
{
    Symbol sym(name, address, type);
    gSymbolsByAddress.emplace(sym.address, sym);
    gSymbolsByName.emplace(sym.name, sym);
}