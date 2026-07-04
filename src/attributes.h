

#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include "types.h"

/**
 * Classification tags for ROM addresses.
 */
enum class T: uint32_t {
    other = 0,
    arm,
    ns,
    data
};

/**
 * Metadata stored per ROM address.
 */
class Attr {
public:
    /** 2-bit type tag for address classification. */
    T type:2;
};

/**
 * Provides attribute lookup for ROM addresses.
 */
class AttributeManager {
public:
    /**
     * Returns attribute metadata for a ROM address.
     * @param index ROM address to query.
     * @return Reference to attribute record at the given address.
     */
    Attr& at(Addr index);
};

/** Global attribute manager used by memory access helpers. */
extern AttributeManager gAttr;

#endif // ATTRIBUTES_H
