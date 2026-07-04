

#include "attributes.h"

AttributeManager gAttr;

AttributeManager::AttributeManager() {
    // Initialize all attributes to default values
    for (auto& a : attr) {
        a.set_unused();
    }
}

Attr& AttributeManager::at(Addr index) {
    static Attr attr_unknown;
    static bool first_call = true;
    if (first_call) {
        first_call = false;
        attr_unknown.set_unknown();
    }
    if (index >= k8MB) {
        return attr_unknown;
    }
    // maybe check that index is aligned to 4 bytes, since each Attr corresponds to 4 bytes of ROM?
    return attr[index / 4]; // Each Attr corresponds to 4 bytes of ROM
}

void AttributeManager::range(Addr start, Addr end, std::function<void(Attr&)> callback) {
    for (Addr addr = start; addr < end; addr += 4) {
        callback(at(addr));
    }
}
