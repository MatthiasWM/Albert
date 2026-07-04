

#include "../types.h"
#include "../memory.h"

#include <fstream>
#include <iostream>
#include <string>

static bool rex_loaded = false;

/**
 * Loads the REx extension block into memory at kRexDataOffset.
 * @param arg Input filename. Uses default file when empty.
 * @return 0 on success, negative value on failure.
 */
int rex_load_binary(const std::string_view arg) {
    if (rex_loaded) {
        return 0;
    }
    std::string filename = std::string(arg);
    std::cout << "Loading REx part of NewtonOS ROM file " << filename << std::endl;

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open REx file: " << filename << std::endl;
        return -1;
    }

    auto* dst = gMem.raw_data(kRexDataOffset, kRexDataSize);
    file.read(reinterpret_cast<char*>(dst), static_cast<std::streamsize>(kRexDataSize));
    if (file.gcount() != static_cast<std::streamsize>(kRexDataSize)) {
        std::cerr << "Failed to read full REx section from file: " << filename << std::endl;
        return -1;
    }

    rex_loaded = true;
    return 0;
}
