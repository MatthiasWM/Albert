

#include "symbols.h"
#include "types.h"

#include <iomanip>

void Symbol::print(std::ostream& out) const {
    out << "Symbol: " << name;
    if (type) {
        out << " type: " << type->name;
    }
    out << " at 0x" << std::hex << std::setw(8) << std::setfill('0') << address;
    if (info) {
        out << " (function)";
    }
    out << std::endl;
}