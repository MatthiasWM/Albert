

#include "attributes.h"

AttributeManager gAttr;


Attr& AttributeManager::at(Addr index) {
    // For simplicity, we will just return a static Attr object.
    // In a real implementation, you would manage a collection of Attributes.
    static Attr attr;
    return attr;
}