

#include "labels.h"
#include "symbols.h"

#include <iomanip>

std::multimap<Addr, Label*> gLabelByAddress;
std::map<std::string, Label*> gLabelByName;

void add_named_label(const std::string& name, Addr address, uint8_t type)
{
    Label* sym = new Label(name, address, type);
    gLabelByAddress.emplace(sym->address, sym);
    gLabelByName.emplace(sym->name, sym);
}

void Label::print(std::ostream& out) const {
    out << "Label: " << name << " at 0x" << std::hex << std::setw(8) << std::setfill('0') << address
        << " type: " << std::dec << static_cast<int>(type);
    out << std::endl;
    if (symbol) {
        symbol->print(out);
    }
}
