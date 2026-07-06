


#include "types.h"

#include "symbols.h"

#include <iostream>
#include <fstream>

TypeMap gTypeMap;

TypeMap::TypeMap() {
    // Initialize the type map with built-in types
    map["void"] = new Type("void", Type::kBuiltin, Type::T::kVoid, 0);
    map["..."] = new Type("...", Type::kBuiltin, Type::T::kElipsis, 0);

    map["char"] = new Type("char", Type::kBuiltin, Type::T::kInt, 1);
    map["unsigned char"] = new Type("unsigned char", Type::kBuiltin, Type::T::kInt, 1);
    map["short"] = new Type("short", Type::kBuiltin, Type::T::kInt, 2);
    map["unsigned short"] = new Type("unsigned short", Type::kBuiltin, Type::T::kInt, 2);
    map["int"] = new Type("int", Type::kBuiltin, Type::T::kInt, 4);
    map["unsigned int"] = new Type("unsigned int", Type::kBuiltin, Type::T::kInt, 4);
    map["long"] = new Type("long", Type::kBuiltin, Type::T::kInt, 4);
    map["unsigned long"] = new Type("unsigned long", Type::kBuiltin, Type::T::kInt, 4);

    map["float"] = new Type("float", Type::kBuiltin, Type::T::kInt, 4);
    map["double"] = new Type("double", Type::kBuiltin, Type::T::kInt, 8);

    // Add more built-in types as needed
}

/**
 * Find or create any type by this name.
 * If the type is not already in the map, create a new `kUnknown` Type object
 * and add it to the map.
 *
 * \arg[in] name The name of the type to find or create.
 * \return Always return a pointer to a type object.
 */
Type* TypeMap::tag_type(const std::string& name) {
    auto it = map.find(name);
    if (it == map.end()) {
        // Type not found, create a new one
        Type* new_type = new Type(name);
        map[name] = new_type;
        return new_type;
    } else {
        // Type found, return it
        return it->second;
    }
}

/**
 * Find or create a class type by this name.
 * If the type is not already in the map, create a new `kClass` Type object
 * and add it to the map. If an existing object is not a class and can't be
 * upgraded, report a collision.
 */
Type* TypeMap::tag_class(const std::string& name) {
    Type* type = tag_type(name);
    type->promote_to_class();
    return type;
}

/**
 * Find or create a function type by this name.
 * If the type is not already in the map, create a new `kFunction` Type object
 * and add it to the map. If an existing object is not a function and can't be
 * upgraded, report a collision.
 */
Type* TypeMap::tag_function_type(const std::string& arglist) {
    Type* type = tag_type(arglist);
    type->promote_to_function();
    return type;
}

Type* TypeMap::tag_member_function_type(const std::string& klass, const std::string& arglist) {
    tag_class(klass);
    std::string full_name = klass + "::" + arglist;
    return tag_function_type(full_name);
}

void TypeMap::print() {
    std::fstream out("output/types.txt", std::ios::out | std::ios::trunc);
    for (const auto& [name, type] : map) {
        type->print(out);
    }
}

/**
 * Create a Type object of the type `kUnknown`.
 */
Type::Type(const std::string& name)
: name(name) {
}

Type::~Type() {
    if (type == T::kClass && class_type) {
        delete class_type;
    } else if (type == T::kFunction && function_type) {
        delete function_type;
    }
}

void Type::print(std::ostream& out) const {
    out << name;
    if (type == T::kClass && class_type) {
        out << " (class)" << std::endl;
        class_type->print(out);
    } else if (type == T::kFunction && function_type) {
        out << " (function)" << std::endl;
        function_type->print(out);
    } else {
        out << std::endl;
    }
}

bool Type::promote_to_class() {
    if (type == T::kUnknown) {
        type = T::kClass;
        class_type = new ClassTypeInfo();
    } else if (type != T::kClass) {
        std::cerr << "COLLISION: Type `" << name << "` is already defined as a non-class type." << std::endl;
        return false;
    }
    return true;
}

bool Type::promote_to_function() {
    if (type == T::kUnknown) {
        type = T::kFunction;
        function_type = new FunctionTypeInfo();
    } else if (type != T::kFunction) {
        std::cerr << "COLLISION: Type `" << name << "` is already defined as a non-function type." << std::endl;
        return false;
    }
    return true;
}

/**
 * Add a function type to this class type.
 */
Symbol* Type::tag_member_function(const std::string& name, Type* funtion) {
    if (type != T::kClass) {
        std::cerr << "ERROR: Type `" << this->name << "` is not a class type, cannot add member function." << std::endl;
        return nullptr;
    }
    return class_type->tag_member_function(name, this, funtion);
}

Symbol* ClassTypeInfo::tag_member_function(const std::string& name, Type* klass, Type* function) {
    std::string fullname = name + " " + function->name;
    auto it = methods.find(fullname);
    if (it == methods.end()) {
        // Symbol not found, create a new one
        Symbol* new_symbol = new Symbol{name, function, 0xFFFFFFFF, new FunctionSymbolInfo{}};
        methods[fullname] = new_symbol;
        return new_symbol;
    } else {
        // Symbol already found, return it
        // TODO: check if there is a conflict (must be same type)
        return it->second;
    }
}

void ClassTypeInfo::print(std::ostream& out) const {
    for (const auto& [name, symbol] : methods) {
        out << "  Method: " << symbol->name << " (type: " << symbol->type->name << ")" << std::endl;
    }
}

void FunctionTypeInfo::print(std::ostream& out) const {
}


