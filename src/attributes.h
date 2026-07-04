

#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include "types.h"

#include <functional>

/**
 * Metadata stored per ROM address.
 */
class Attr {
public:
    /**
     * Classification tags for ROM addresses.
     */
    enum class T: uint32_t {
        other = 0,
        arm,
        ns,
        data
    };

    /** 2-bit type tag for address classification. */
    T type:2;
    void set_unused() { type = T::other; }
    void set_unknown() { type = T::other; }
};

/**
 * Provides attribute lookup for ROM addresses.
 */
class AttributeManager {
    Attr attr[k8MB/4]; // 8 MB of attributes at word boundaries
public:
    AttributeManager();
    /**
     * Returns attribute metadata for a ROM address.
     * @param index ROM address to query.
     * @return Reference to attribute record at the given address.
     */
    Attr& at(Addr index);
    void range(Addr start, Addr end, std::function<void(Attr&)> callback);
};

/** Global attribute manager used by memory access helpers. */
extern AttributeManager gAttr;

#endif // ATTRIBUTES_H
