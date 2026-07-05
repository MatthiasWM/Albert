

#ifndef ALBERT_SRC_ATTRIBUTES_H
#define ALBERT_SRC_ATTRIBUTES_H

#include "main.h"

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

    enum class OT: uint32_t {
        unknown = 0,
        unused,
    };

    enum class DT: uint32_t {
        unknown = 0,
        w, // numeric or pointer
        ptr, // pointer
        ref, // NS reference
        i, // 4 byte integer
        ss, // short short
        cccc, // chars or bytes
        fourcc, // 4 chars or bytes
        double_h, // first part of a double
        double_l, // second part of a double
    };

    union {
        // type = other:
        OT other_type:2;
        // type = arm:
        struct {
            uint32_t jump_target:1;
            uint32_t call_target:1;
        };
        // type = ns:
        // type = data:
        struct {
            DT data_type:4;
        };
    };
    uint32_t visited:1;
    uint32_t first:1;    // if set, this is the beginning of a struct, function,
                        // or newton script object. There will likely be more
                        // infor for this address

    /** 2-bit type tag for address classification. */
    T type:2;
    void set_unused() { type = T::other; other_type = OT::unused; }
    bool is_unused() const { return type == T::other && other_type == OT::unused; }
    void set_unknown() { type = T::other; other_type = OT::unknown; }
    bool is_unknown() const { return type == T::other && other_type == OT::unknown; }
    void set_arm() { type = T::arm; }
    void set_arm_call_target() { set_arm(); call_target = 1; }
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

#endif // ALBERT_SRC_ATTRIBUTES_H
