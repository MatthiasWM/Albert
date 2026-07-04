

#include "memory.h"

#include "attributes.h"

#include <stdexcept>
#include <cstring>

Memory gMem;

Memory::Memory() {
    // Initialize memory to zero
    std::memset(data, 0xff, sizeof(data));
}

uint8_t Memory::get_byte(Addr index) const {
    if (index + 1 >= k8MB) {
        throw std::out_of_range("Index out of range for get_byte");
    }
    return data[index];
}

uint16_t Memory::get_short(Addr index) const {
    if (index + 1 >= k8MB) {
        throw std::out_of_range("Index out of range for get_short");
    }
    if (index % 2 != 0) {
        throw std::invalid_argument("Index must be even for get_short");
    }
    return (data[index] << 8) | data[index + 1];
}

uint32_t Memory::get_word(Addr index) const {
    if (index + 3 >= k8MB) {
        throw std::out_of_range("Index out of range for get_word");
    }
    if (index % 4 != 0) {
        throw std::invalid_argument("Index must be multiple of 4 for get_word");
    }
    return (data[index] << 24) | (data[index + 1] << 16) | (data[index + 2] << 8) | data[index + 3];
}

double Memory::get_double(Addr index) const {
    if (index + 7 >= k8MB) {
        throw std::out_of_range("Index out of range for get_double");
    }
    if (index % 8 != 0) {
        throw std::invalid_argument("Index must be multiple of 8 for get_double");
    }
    double value;
    std::memcpy(&value, &data[index], sizeof(double));
    return value;
}

Attr& Memory::get_attribute(Addr index) const {
    return gAttr.at(index);
}

uint8_t* Memory::raw_data(Addr addr, Addr size) {
    if (addr + size > k8MB) {
        throw std::out_of_range("Address range out of bounds");
    }
    return &data[addr];
}
