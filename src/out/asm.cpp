

#include "../types.h"
#include "../memory.h"

#include <fstream>
#include <iostream>
#include <string>

/**
 * Writes the ROM image as ARM32 assembler, collecting all available data.
 * @param arg Output filename. Uses default file when empty.
 * @return 0 on success, negative value on failure.
 */
int out_asm(const std::string_view arg) {
    std::string filename = std::string(arg);
    std::cout << "Outputting ARM32 assembler to " << filename << std::endl;
/*
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open output file: " << filename << std::endl;
        return -1;
    }

    const auto* src = gMem.raw_data(0, k8MB);
    file.write(reinterpret_cast<const char*>(src), static_cast<std::streamsize>(k8MB));
    if (!file) {
        std::cerr << "Failed to write assembler output to file: " << filename << std::endl;
        return -1;
    }
*/
    // TODO: Implement ARM32 disassembly output
    return 0;
}

