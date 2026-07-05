

/*
 Read the ROM aif file and fill in the symbol table with the symbols from the
 aif file.
*/

#include "../symbols.h"
#include "../main.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>

namespace {

bool read_exact(std::istream& in, char* dst, std::streamsize size) {
    in.read(dst, size);
    return static_cast<std::streamsize>(in.gcount()) == size;
}

bool read_u32_be(std::istream& in, uint32_t& out) {
    unsigned char buf[4]{};
    if (!read_exact(in, reinterpret_cast<char*>(buf), 4)) {
        return false;
    }
    out = (static_cast<uint32_t>(buf[0]) << 24) |
          (static_cast<uint32_t>(buf[1]) << 16) |
          (static_cast<uint32_t>(buf[2]) << 8) |
          static_cast<uint32_t>(buf[3]);
    return true;
}

} // namespace

static bool cpp_symbols = false;

/**
 * Loads AIF ROM symbols into the symbol table.
 *
 * The loader skips the AIF header and reads the symbol information.
 * @param arg Input filename. Uses default file when empty.
 * @return 0 on success, negative value on failure.
 */
int aif_load_symbols(const std::string_view arg) {
    if (cpp_symbols) {
        return 0;
    }

    const std::set<std::string> ignored_symbols = {
        "kEntriesPerSlot",
        "kSlotsPerPage",
        "kEntriesPerPage",
        "kPageSize",
        "Image$$root$$ZI$$Length",
        "Image$$root$$Length",
    };

    const auto add_if_not_ignored = [&](const std::string& name, Addr address, uint8_t type) {
        if (ignored_symbols.find(name) == ignored_symbols.end()) {
            add_global_symbol(name, address, type);
        }
    };

    std::string filename = std::string(arg);
    if (filename.empty()) {
        filename = "input/Senior CirrusNoDebug image";
    }
    std::cout << "Loading AIF ROM symbols from file " << filename << std::endl;

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open AIF ROM file: " << filename << std::endl;
        return -1;
    }

    file.seekg(static_cast<std::streamoff>(kAifSymList + 8 * 4), std::ios::beg);
    if (!file) {
        std::cerr << "Failed to seek to AIF symbol list" << std::endl;
        return -1;
    }

    uint32_t num_symbols = 0;
    if (!read_u32_be(file, num_symbols)) {
        std::cerr << "Failed to read AIF symbol count" << std::endl;
        return -1;
    }

    for (uint32_t i = 0; i < num_symbols; ++i) {
        uint32_t raw_name_offs = 0;
        uint32_t value = 0;
        if (!read_u32_be(file, raw_name_offs) || !read_u32_be(file, value)) {
            std::cerr << "Failed to read AIF symbol entry at index " << i << std::endl;
            return -1;
        }

        const uint8_t flags = static_cast<uint8_t>((raw_name_offs >> 24) & 0xffU);
        const uint32_t name_offs = raw_name_offs & 0x00ffffffU;

        const std::streampos return_pos = file.tellg();
        if (return_pos == static_cast<std::streampos>(-1)) {
            std::cerr << "Failed to get stream position while reading symbols" << std::endl;
            return -1;
        }

        file.seekg(static_cast<std::streamoff>(kAifSymTable + name_offs), std::ios::beg);
        if (!file) {
            std::cerr << "Failed to seek to symbol name at offset " << name_offs << std::endl;
            return -1;
        }

        unsigned char name_len = 0;
        if (!read_exact(file, reinterpret_cast<char*>(&name_len), 1)) {
            std::cerr << "Failed to read symbol name length at index " << i << std::endl;
            return -1;
        }

        std::string sym_name(name_len, '\0');
        if (name_len > 0 && !read_exact(file, sym_name.data(), static_cast<std::streamsize>(name_len))) {
            std::cerr << "Failed to read symbol name bytes at index " << i << std::endl;
            return -1;
        }

        file.seekg(return_pos, std::ios::beg);
        if (!file) {
            std::cerr << "Failed to restore stream position after reading symbol name" << std::endl;
            return -1;
        }

        // flags are 1, 3, 5, or 7: the meaning is unclear
        // Symbols starting with k followed by a captial letter are usually constants and have no address
        // Symbols containing "$" are compiler and linker related
        // Symbols containing "__" are C++ mangled names
        // Symbols starting with "__" are usually C++ operators, constructors, etc.
        // Symbols starting with "_" are usually internal C functions or variables
        // FPE symbols are usually floating-point emulation functions
        // PublicFiller_nnn are empty entires in the patch table that call all the same exceptionHandler
        // MPnnnn are NS pointers to Magic Pointers, so they are offset by one
        // Single code blocks can be know by multiple symbols with the same address, but different names
        //   Note that the patch table entries have different addresses for different name, but may be pointingto the same code (memmove, memcpy, for example

        // if (flags == 1) {
        //     value_symbol_list[value] = new Symbol(sym_name, value, flags);
        // } else if (value >= 0 && value < 0x00800000) {
        //     rom_symbol_list[value] = new Symbol(sym_name, value, flags);
        // } else if (value >= 0x01a00000 && value < 0x01c20000) {
        //     jumptable_symbol_list[value] = new Symbol(sym_name, value, flags);
        // } else {
        //     memory_symbol_list[value] = new Symbol(sym_name, value, flags);
        // }

        // Change the name for entries in the patch table to be identifiable.
        // The patch table was probably generated after the original code was
        // compiled, so the symbols are not in the original source code.
        if (value >= 0x01a00000 && value < 0x01c20000) {
            sym_name = "VEC_" + sym_name;
        }

        add_if_not_ignored(sym_name, value, flags);
    }

    cpp_symbols = true;

    #if 0 // list all symbols in the symbol table
    for (const auto& [addr, sym] : gSymbolsByAddress) {
        std::cout << "Symbol: " << sym.name << " at 0x" << std::hex << std::setw(8) << std::setfill('0') << addr
                  << " type: " << std::dec << static_cast<int>(sym.type) << std::endl;
    }
    #endif

    #if 0 // list all symbols with a duplicate address
    bool found_duplicates = false;
    for (auto it = gSymbolsByAddress.begin(); it != gSymbolsByAddress.end(); ) {
        const Addr addr = it->first;
        const auto range = gSymbolsByAddress.equal_range(addr);

        size_t count = 0;
        for (auto jt = range.first; jt != range.second; ++jt) {
            ++count;
        }

        if (count > 1) {
            found_duplicates = true;
            std::cout << "Duplicate symbols at 0x"
                      << std::hex << std::setw(8) << std::setfill('0') << addr
                      << std::dec << " (" << count << ")" << std::endl;
            for (auto jt = range.first; jt != range.second; ++jt) {
                std::cout << "  - " << jt->second.name
                          << " (type " << static_cast<int>(jt->second.type) << ")"
                          << std::endl;
            }
        }

        it = range.second;
    }
    #endif

    #if 0 // list all symbols with a duplicate name, but different address
    bool found_duplicate_names = false;
    for (auto it = gSymbolsByName.begin(); it != gSymbolsByName.end(); ) {
        const std::string& name = it->first;
        const auto range = gSymbolsByName.equal_range(name);

        std::set<Addr> unique_addresses;
        for (auto jt = range.first; jt != range.second; ++jt) {
            unique_addresses.insert(jt->second.address);
        }

        if (unique_addresses.size() > 1) {
            found_duplicate_names = true;
            std::cout << "Duplicate symbol name '" << name << "' at multiple addresses:" << std::endl;
            for (auto jt = range.first; jt != range.second; ++jt) {
                std::cout << "  - 0x"
                          << std::hex << std::setw(8) << std::setfill('0') << jt->second.address
                          << std::dec << " (type " << static_cast<int>(jt->second.type) << ")"
                          << std::endl;
            }
        }

        it = range.second;
    }
    if (!found_duplicate_names) {
        std::cout << "No duplicate symbol names with different addresses found." << std::endl;
    }
    #endif

    add_if_not_ignored("sa110_reset", 0, 3);
    add_if_not_ignored("sa110_undefined_instruction", 4, 3);
    add_if_not_ignored("sa110_swi", 8, 3);
    add_if_not_ignored("sa110_prefetch_abort", 12, 3);
    add_if_not_ignored("sa110_data_abort", 16, 3);
    add_if_not_ignored("sa110_26bit", 20, 3);
    add_if_not_ignored("sa110_irq", 24, 3);
    add_if_not_ignored("sa110_fiq", 28, 3);

    return 0;
}

