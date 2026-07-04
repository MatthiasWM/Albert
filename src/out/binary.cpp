

#include "../types.h"
#include "../memory.h"

#include <fstream>
#include <iostream>
#include <string>

/**
 * Writes the full 8 MB memory image to a binary output file.
 * @param arg Output filename. Uses default file when empty.
 * @return 0 on success, negative value on failure.
 */
int out_binary(const std::string_view arg) {
    std::string filename = std::string(arg);
    std::cout << "Generating binary dump of ROM data: " << filename << std::endl;

    std::ofstream file(filename, std::ios::binary|std::ios::trunc|std::ios::out);
    if (!file) {
        std::cerr << "Failed to open output file: " << filename << std::endl;
        return -1;
    }

    const auto* src = gMem.raw_data(0, k8MB);
    file.write(reinterpret_cast<const char*>(src), static_cast<std::streamsize>(k8MB));
    if (!file) {
        std::cerr << "Failed to write full ROM dump to file: " << filename << std::endl;
        return -1;
    }

    return 0;
}

