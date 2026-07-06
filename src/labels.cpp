

#include "labels.h"

std::multimap<Addr, Label*> gLabelByAddress;
std::map<std::string, Label*> gLabelByName;


void add_named_label(const std::string& name, Addr address, uint8_t type)
{
    Label* sym = new Label(name, address, type);
    gLabelByAddress.emplace(sym->address, sym);
    gLabelByName.emplace(sym->name, sym);
}