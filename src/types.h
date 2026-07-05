

#ifndef ALBERT_SRC_TYPES_H
#define ALBERT_SRC_TYPES_H

#include "main.h"

#include <string>
#include <vector>

class Type {
public:
    std::string name;
    // alias of the type, if any, or base typ, if pointer
    // pointer to, reference to, const pointer to, const reference to
    // size in bytes (same as alignement?!)
    // flags: builtin type, pointer, reference, const, volatile, array, struct,
    // union, enum, function, method

    // function: address, arguments, return type
    // class: base class, members, methods, vtable, size, alignment
    // struct: members, size, alignment
    // array: element type, number of elements, size, alignment
    // union: members, size, alignment
    // enum: values, size, alignment
};

#endif // ALBERT_SRC_TYPES_H