

#ifndef ALBERT_SRC_LABELS_H
#define ALBERT_SRC_LABELS_H

#include "main.h"
#include <string>
#include <vector>
#include <map>

class Symbol;

/**
 * A Label is a named symbol that lives at a fixed address in memory.
 *
 * This includes functions and data in ROM, as well as global variables and
 * hardware periphery registers. One address can have none, one, or multiple
 * labels. One label can only ever have a single address. There are no duplicate
 * labels allowed.
 *
 * Note that the ROM patch table is mapped into virtaul RAM and has the same
 * label name as the original function. We add "VEC_" to the front of the label
 * name to distinguish it from the original function.
 *
 * Note that multiple patch vectors may point to the same addresss. For example,
 * `memcpy` and `memmove` are both implemented by the same function.
 *
 * When we find out more about a label, it may point to a `Symbol` object that
 * has more information about the label, such as its unmangled name, type, size,
 * and other attributes.
 */
class Label {
public:
    // the decorated name of the label, as found in the ROM symbol table
    std::string name { };
    Addr address { 0xFFFFFFFF };
    Symbol* symbol { nullptr };
    uint8_t type; // TODO: not yet defined at all
    Label(const std::string& name, Addr address, uint8_t type)
        : name(name), address(address), type(type) {}
};

extern std::multimap<Addr, Label*> gLabelByAddress;
extern std::map<std::string, Label*> gLabelByName;

void add_named_label(const std::string& name, Addr address, uint8_t type);

#endif // ALBERT_SRC_LABELS_H