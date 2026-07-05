

#include "arm.h"

std::vector<Addr> gArmFunctionEntryPoints = {
    0x00000000, // Reset vector
    0x00000004, // Undefined instruction vector
    0x00000008, // Software interrupt vector
    0x0000000C, // Prefetch abort vector
    0x00000010, // Data abort vector
    0x00000014, // Reserved vector
    0x00000018, // IRQ vector
    0x0000001C, // FIQ vector
};
