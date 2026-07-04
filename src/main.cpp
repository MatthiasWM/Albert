

#include "types.h"
#include "operators.h"

#include <vector>
#include <string>

// constexpr Addr kAifFileDataStart = 0x00000080; // Offset by header size
// constexpr Addr kAifDataSize = 0x0071A95C + 0x000052F0; // RO size + R/W size, 0x71FCCC
// constexpr Addr kRexDataOffset = kAifDataSize; // Offset in binary ROM image
// constexpr Addr kRexDataSize = 0x000CE3FC; // REx size
// constexpr Addr kRexDataEnd = kRexDataOffset + kRexDataSize; // 0x7EE0C8


static_assert(sizeof(Addr) == 4, "Addr must be 32 bits");
static_assert(kAifDataSize == 0x0071FC4C, "Incorrect AIF data size, ROM$$Size = 0x0071FC4C");
static_assert(kRexDataSize == 0x000CE3FC, "Incorrect REx size");
static_assert(kRexDataEnd == 0x007EE048, "Incorrect REx data end, ROM$$Size + RExBlock = 0x007EE048");

std::vector<std::pair<std::string, std::string>> gArgs;

/**
 * Parses command-line options and dispatches operators.
 * @param argc Argument count.
 * @param argv Argument values.
 * @return Process exit code.
 */
int main(int argc, char **argv) {
    for (int i = 1; i < argc; ) {
        std::string op = argv[i++];
        std::string arg;
        if (i < argc) arg = argv[i];
        if (arg.starts_with("-")) {
            gArgs.emplace_back(op, "");
        } else {
            gArgs.emplace_back(op, arg);
            i++;
        }
    }
    return apply_operators(gArgs);
}