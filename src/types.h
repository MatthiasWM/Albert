

#ifndef ALBERT_SRC_TYPES_H
#define ALBERT_SRC_TYPES_H

#include "main.h"

#include <string>
#include <vector>
#include <map>
#include <memory>

class Member {
public:
    std::string name { };
    std::string type { };
    int offset { -1 }; // offset in bytes from start of struct/class
    uint32_t flags { 0 }; // flags for member: static, const, volatile, etc.
};

class Type {
public:
    std::string name { };
    static constexpr uint32_t kBuiltin = 0x00000001;
    uint32_t flags { 0 };
    enum class T {
        kUnknown = 0,
        kVoid,
        kElipsis,
        kInt,
        kClass,
    };
    T type { T::kUnknown };
    int size { -1 }; // size in bytes, -1 = unknown
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

class ClassType : public Type {
public:
    std::string base_class { };
    std::vector<Member> members { };
    std::vector<Member> methods { };
    std::vector<int> vtable { };    // index into methods?!
    ClassType(const std::string& name);
};

class TypeMap {
public:
    std::map<std::string, std::unique_ptr<Type>> type_map;
    TypeMap();
    Type* make(const std::string& name);
    ClassType* make_class(const std::string& name);
    void print();
};

extern TypeMap gTypeMap;

#endif // ALBERT_SRC_TYPES_H