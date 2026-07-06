

/*
    Header
        00 - NOP
        04 - BL Self relocating code
        08 - BL Zero Init
        0C - BL Entry Point
        10 - Exit instruction
        14 - 0x0071A95C: Image ReadOnly Size
        18 - 0x000052F0: Image ReadWrite Size
        1C - 0c001CADF0: Image Debug Size
        20 - 0x00002324: Image Zero Init Size
        34 - 0x0C100800: Data Base
        24...80 - not needed
    Read-only area
    Read-write area
    Debugging data
*/

#include "../main.h"
#include "../memory.h"
#include "../attributes.h"

#include <fstream>
#include <iostream>
#include <string>

static bool cpp_symbols_op_done = false;

/**
 * Loads AIF ROM payload into memory at address 0.
 *
 * The loader skips the AIF header and copies exactly kAifDataSize bytes.
 * @param arg Input filename. Uses default file when empty.
 * @return 0 on success, negative value on failure.
 */
int aif_load_binary(const std::string_view arg) {
    if (cpp_symbols_op_done) {
        return 0;
    }
    std::string filename = std::string(arg);
    std::cout << "Loading binary part of AIF ROM file " << filename << std::endl;

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open AIF ROM file: " << filename << std::endl;
        return -1;
    }

    file.seekg(static_cast<std::streamoff>(kAifFileDataStart), std::ios::beg);
    if (!file) {
        std::cerr << "Failed to seek to AIF data start: " << kAifFileDataStart << std::endl;
        return -1;
    }

    auto* dst = gMem.raw_data(0, kAifDataSize);
    file.read(reinterpret_cast<char*>(dst), static_cast<std::streamsize>(kAifDataSize));
    if (file.gcount() != static_cast<std::streamsize>(kAifDataSize)) {
        std::cerr << "Failed to read full AIF data section from file: " << filename << std::endl;
        return -1;
    }

    gAttr.range(0, kAifDataSize, [](Attr& a) { a.set_unknown(); });

    cpp_symbols_op_done = true;
    return 0;
}

