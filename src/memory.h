

#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

class Memory;
class Attr;

/** Shared base for typed read accessors bound to one memory instance. */
class DataAccessor {
public:
    Memory &m;
    DataAccessor(Memory& memory) : m(memory) {}
};

/** Byte-wise read accessor. */
class ByteAccessor : public DataAccessor {
public:
    ByteAccessor(Memory& memory) : DataAccessor(memory) {}
    /** Reads one byte from the given address. */
    uint8_t operator[](Addr index) const;
};

/** 16-bit read accessor. */
class ShortAccessor : public DataAccessor {
public:
    ShortAccessor(Memory& memory) : DataAccessor(memory) {}
    /** Reads one 16-bit value from an even address. */
    uint16_t operator[](Addr index) const;
};

/** 32-bit read accessor. */
class WordAccessor : public DataAccessor {
public:
    WordAccessor(Memory& memory) : DataAccessor(memory) {}
    /** Reads one 32-bit value from a 4-byte aligned address. */
    uint32_t operator[](Addr index) const;
};

/** 64-bit floating-point read accessor. */
class DoubleAccessor : public DataAccessor {
public:
    DoubleAccessor(Memory& memory) : DataAccessor(memory) {}
    /** Reads one double from an 8-byte aligned address. */
    double operator[](Addr index) const;
};

/** Attribute read accessor. */
class AttributeAccessor : public DataAccessor {
public:
    AttributeAccessor(Memory& memory) : DataAccessor(memory) {}
    /** Reads classification metadata for an address. */
    Attr& operator[](Addr index) const;
};

/**
 * ROM memory model and typed accessor facade.
 */
class Memory {
    uint8_t data[k8MB]; // 8 MB of memory
public:
    /** Initializes the backing storage. */
    Memory();
    /** Byte accessor instance. */
    ByteAccessor b { *this };
    /** 16-bit accessor instance. */
    ShortAccessor s { *this };
    /** 32-bit accessor instance. */
    WordAccessor w { *this };
    /** Double accessor instance. */
    DoubleAccessor d { *this };
    /** Attribute accessor instance. */
    AttributeAccessor a { *this };

    /** Returns one byte at address index. */
    uint8_t get_byte(Addr index) const;
    /** Returns one 16-bit value at an even address. */
    uint16_t get_short(Addr index) const;
    /** Returns one 32-bit value at a 4-byte aligned address. */
    uint32_t get_word(Addr index) const;
    /** Returns one double at an 8-byte aligned address. */
    double get_double(Addr index) const;
    /** Returns attribute metadata for address index. */
    Attr& get_attribute(Addr index) const;
    /**
     * Returns writable raw memory pointer for a bounded address range.
     * @param addr Start address in ROM memory.
     * @param size Number of bytes in the requested window.
     * @return Pointer to the first byte in the requested range.
     */
    uint8_t* raw_data(Addr addr, Addr size);
};

inline uint8_t ByteAccessor::operator[](Addr index) const {
    return m.get_byte(index);
}
inline uint16_t ShortAccessor::operator[](Addr index) const {
    return m.get_short(index);
}
inline uint32_t WordAccessor::operator[](Addr index) const {
    return m.get_word(index);
}
inline double DoubleAccessor::operator[](Addr index) const {
    return m.get_double(index);
}
inline Attr& AttributeAccessor::operator[](Addr index) const {
    return m.get_attribute(index);
}

extern Memory gMem;

#endif // MEMORY_H