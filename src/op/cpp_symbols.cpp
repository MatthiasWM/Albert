

#include "../symbols.h"
#include "../attributes.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <utility>


static bool cpp_symbols = false;

// convert names like "10TObjectPtrFRC6RefVar" into "TObjectPtr"
// returns {name, bytes_to_skip}
static std::pair<std::string, std::size_t> nn_symbol(const std::string& str)
{
    std::size_t digits_end = 0;
    while (digits_end < str.size() && str[digits_end] >= '0' && str[digits_end] <= '9') {
        ++digits_end;
    }
    if (digits_end == 0) {
        return {"", 0};
    }

    std::size_t name_len = 0;
    for (std::size_t i = 0; i < digits_end; ++i) {
        name_len = (name_len * 10) + static_cast<std::size_t>(str[i] - '0');
    }

    if (digits_end + name_len > str.size()) {
        return {"", 0};
    }

    return {str.substr(digits_end, name_len), digits_end + name_len};
}

/**
 * Decode a decorated C++ label.
 */
static void eval_cpp_symbol(const Symbol& sym) {
    bool is_static = false;
    bool is_const = false;
    std::string label = sym.name;
    if (label.starts_with("__")) return;
    auto uu = label.find("__");
    if (uu == std::string::npos) return;

    // handle names that end in a "_", followed by "__"
    while (label[uu+2] == '_') {
        uu++;
    }

    // extract the name
    std::string name = label.substr(0, uu);
    std::string klass;
    uu += 2; // skip the "__"

    // Check if the label is part of a class
    char c = label[uu];
    if ( (c >= '1') && ( c <= '9') ) {
        auto [parsed_klass, bytes_to_skip] = nn_symbol(label.substr(uu));
        klass = parsed_klass;
        uu += bytes_to_skip;
    }

    // static
    if (label[uu] == 'S') {
        is_static = true;
        uu++;
    }

    // const
    if (label[uu] == 'C') {
        is_const = true;
        uu++;
    }

    // function
    if (label[uu] == 'F') {
        uu++;
        gAttr.at(sym.address).set_arm_call_target();
        // if (!klass.empty()) gTypes.make_class(klass);
        // TODO: parse arguments
        std::cout << "Args: " << label.substr(uu) << std::endl;
        // v = void
        // P = pointer to...
        // U = unsigned
        // i = int
        // c = char
        // R = reference to...
        // C = const
        // 1..9 = type name follows
        // l = long?
        // Tnumber = template?
        // s = ?
        // Nnumbernumber = repeat template
        // d = double
    } else {
        std::cout << "WARNING: unexpected C++ symbol format: " << label
            << " at 0x" << std::hex << std::setw(8) << std::setfill('0')
            << sym.address << " type: " << std::dec << static_cast<int>(sym.type)
            << " ID: " << label[uu] << std::endl;
    }
}

/**
 * Evaluates C++ symbols in the symbol table.
 *
 * This adds ARM entry points and types as we find them.
 *
 * @param arg ignored
 * @return 0 on success, negative value on failure.
 */
int eval_cpp_symbols(const std::string_view arg) {
    if (cpp_symbols) {
        return 0;
    }
    cpp_symbols = true;

    std::cout << "Evaluating C++ symbols\n";

    // TODO: we want to find and decode decorated C++ symbols.
    // Symbols can be just types (unlikely), orfunctions or methods
    // Decorated function symbols contain class and function name, all parameter
    // types, static, virtual, const, but unfortunatley no return type.

    // Ignore:
    // C++ symbol: SYM__xxtemplabel at 0x0049d204 type: 3
    // C++ symbol: __ctype at 0x0038053c type: 3
    // C++ symbol: __huge_val at 0x00380a94 type: 3
    // C++ symbol: __main at 0x0038c89c type: 3
    // C++ symbol: __multiply at 0x0038c8d4 type: 3
    // C++ symbol: __rt_divtest at 0x0038c8f0 type: 3
    // C++ symbol: __rt_udiv at 0x0038c8fc type: 3
    // C++ symbol: __uremainder at 0x0038c9cc type: 3
    // C++ symbol: __rt_udiv10 at 0x0038c9dc type: 3
    // C++ symbol: __udiv10 at 0x0038c9dc type: 3
    // C++ symbol: __rt_sdiv at 0x0038ca10 type: 3
    // C++ symbol: __remainder at 0x0038cafc type: 3
    // C++ symbol: __rt_sdiv10 at 0x0038cb0c type: 3
    // C++ symbol: __fp_decodeNJT at 0x0038d870 type: 3
    // C++ symbol: __fp_decode at 0x0038d870 type: 3
    // C++ symbol: __ptr_ddtorvec at 0x0c1056f4 type: 5
    // C++ symbol: __stdin at 0x0c105704 type: 5
    // C++ symbol: __stdout at 0x0c105744 type: 5
    // C++ symbol: __stderr at 0x0c105784 type: 5
    // C++ symbol: __fp_regs at 0x0c105a5c type: 5
    // C++ symbol: __pure_virtual_called at 0x002ba330 type: 3
    // C++ symbol: __fpclassifyd at 0x002bee04 type: 3
    // C++ symbol: __isnanf at 0x002e2d68 type: 3
    // C++ symbol: __inf at 0x002e2d9c type: 3
    // C++ symbol: __isnormald at 0x002e2ea4 type: 3
    // C++ symbol: __isfinited at 0x002e2ed4 type: 3
    // C++ symbol: __isnand at 0x002e2f04 type: 3
    // C++ symbol: __rt_lib_shutdown at 0x00319014 type: 3
    // C++ symbol: __rt_exit at 0x00319290 type: 3
    // C++ symbol: __vfprintf at 0x0033dc28 type: 3
    // C++ symbol: __signbitd at 0x0034713c type: 3
    // C++ symbol: __flsbuf at 0x0034f234 type: 3
    // C++ symbol: __filbuf at 0x0034f480 type: 3
    // C++ symbol: __backspace at 0x0034f72c type: 3

    // Symbols after 0x0c100800 are type 5 and 7
    // They are the location of static member variables - nice!
    // TODO: enable those

    // Note method starting wwith ClassInfo__, nice!
    // Also find classinfos in REx

    // ignore the patch table

    // special cases:
    // __ct__
    // __dt__
    // __as__
    // __opl__
    // __lt__
    // __gt__
    // __eq__
    // __nw__
    // __dl__
    // __ls__
    // __rs__
    // __vc__
    // __vec_new__ __vec_delete__
    // __opUs__
    // __opPc__
    // __opd__
    // __nw_v__
    // __dl_v__
    // __push_ddtor__
    // __pvfn__


    for (const auto& [addr, sym] : gSymbolsByAddress) {
        if (addr >= k8MB) break; // TODO: too strict! See system variables!
        auto pos = sym.name.find("__");
        if ((pos != std::string::npos) && (pos != 0)) { // TODO: avoid special cases for now
            eval_cpp_symbol(sym);
            // std::cout << "C++ symbol: " << sym.name << " at 0x" << std::hex << std::setw(8) << std::setfill('0') << addr
            //           << " type: " << std::dec << static_cast<int>(sym.type) << std::endl;
        }
    }

    return 0;
}
