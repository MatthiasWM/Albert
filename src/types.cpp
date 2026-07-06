


#include "types.h"

#include <iostream>
#include <fstream>

TypeMap gTypeMap;

TypeMap::TypeMap() {
    // Initialize the type map with built-in types
    type_map["void"] = std::make_unique<Type>("void", Type::kBuiltin, Type::T::kVoid, 0);
    type_map["..."] = std::make_unique<Type>("...", Type::kBuiltin, Type::T::kElipsis, 0);

    type_map["char"] = std::make_unique<Type>("char", Type::kBuiltin, Type::T::kInt, 1);
    type_map["unsigned char"] = std::make_unique<Type>("unsigned char", Type::kBuiltin, Type::T::kInt, 1);
    type_map["short"] = std::make_unique<Type>("short", Type::kBuiltin, Type::T::kInt, 2);
    type_map["unsigned short"] = std::make_unique<Type>("unsigned short", Type::kBuiltin, Type::T::kInt, 2);
    type_map["int"] = std::make_unique<Type>("int", Type::kBuiltin, Type::T::kInt, 4);
    type_map["unsigned int"] = std::make_unique<Type>("unsigned int", Type::kBuiltin, Type::T::kInt, 4);
    type_map["long"] = std::make_unique<Type>("long", Type::kBuiltin, Type::T::kInt, 4);
    type_map["unsigned long"] = std::make_unique<Type>("unsigned long", Type::kBuiltin, Type::T::kInt, 4);

    type_map["float"] = std::make_unique<Type>("float", Type::kBuiltin, Type::T::kInt, 4);
    type_map["double"] = std::make_unique<Type>("double", Type::kBuiltin, Type::T::kInt, 8);

    // Add more built-in types as needed
}

ClassType* TypeMap::make_class(const std::string& name) {
    if (type_map.find(name) == type_map.end()) {
        type_map[name] = std::make_unique<ClassType>(name);
    }
    return static_cast<ClassType*>(type_map[name].get());
}

Type* TypeMap::make(const std::string& name) {
    // TODO: this is not the final code!
    if (gTypeMap.type_map.find(name) == gTypeMap.type_map.end()) {
        gTypeMap.type_map[name] = std::make_unique<Type>();
        gTypeMap.type_map[name]->name = name;
    }
    return gTypeMap.type_map[name].get();
}

void TypeMap::print() {
    std::fstream out("output/types.txt", std::ios::out | std::ios::trunc);
    for (const auto& [name, type] : type_map) {
        out << name << std::endl;
    }
}

ClassType::ClassType(const std::string& name) : Type() {
    this->name = "class " + name;
    this->type = T::kClass;
}


