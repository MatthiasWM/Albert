

#ifndef ALBERT_SRC_TYPES_H
#define ALBERT_SRC_TYPES_H

#include "main.h"

#include <string>
#include <vector>
#include <map>
#include <memory>


class ClassTypeInfo;
class FunctionTypeInfo;
class Symbol;

/**
 * Hold the basic information for any supported C and C++ type.
 *
 * If the type is complex (class, struct, fucntion, union), one of the
 * TypeInfo classes is added.
 */
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
        kClass, // if set, `class_type` is valid
        kFunction, // if set, `function_type` is valid
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
    union {
        ClassTypeInfo* class_type = nullptr;
        FunctionTypeInfo* function_type;
    };
    Type(const std::string& name);
    constexpr Type(const std::string& name, uint32_t flags, T type, int size=-1)
        : name(name), flags(flags), type(type), size(size) {};
    ~Type();
    void print(std::ostream& out) const;
    bool promote_to_class();
    bool promote_to_function();
    // --- kClass helpers
    Symbol* tag_member_function(const std::string& name, Type* function);
};

class ClassTypeInfo {
public:
    std::string base_class { };
    std::vector<Symbol*> members { };
    std::map<std::string, Symbol*> methods { };
    Symbol* tag_member_function(const std::string& name, Type* klass, Type* function);
    void print(std::ostream& out) const;
};

class FunctionTypeInfo {
public:
    std::string return_type { };
    std::vector<Type*> args { };
    void print(std::ostream& out) const;
};

class TypeMap {
public:
    // using raw pointers for simplicity, we don;t really care about deleting these types
    std::map<std::string, Type*> map;
    TypeMap();
    Type* tag_type(const std::string& name);
    Type* tag_class(const std::string& name);
    Type* tag_function_type(const std::string& arglist);
    Type* tag_member_function_type(const std::string& klass, const std::string& arglist);
    void print();
};

extern TypeMap gTypeMap;

#endif // ALBERT_SRC_TYPES_H