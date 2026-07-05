

#ifndef ALBERT_SRC_ARM_H
#define ALBERT_SRC_ARM_H

#include "main.h"

#include <vector>

// arm function entry points
// found by finding functions and method in C++ symbols
// finding vtables and following the patch table
// static analysis of code flow
// evaluating protocol classinofs (handmade vtables)
// guessing by finding typical prolog and epilo code
// human generated list

extern std::vector<Addr> gArmFunctionEntryPoints;

#endif // ALBERT_SRC_ARM_H