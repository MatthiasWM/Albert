

#include "../labels.h"
#include "../attributes.h"
#include "../types.h"
#include "../symbols.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <utility>


static bool cpp_symbols_op_done = false;

// convert names like "10TObjectPtrFRC6RefVar" into "TObjectPtr"
// returns {name, bytes_to_skip}

// TODO: why doe this one slip through:
// lN21PiP13PS_point_typesPsT7P8ppd_typeUiP11xrdata_type
// ...(long, long, long, int *, PS_point_type *, short, short *, short *, ppd_type *, unsigned int, xrdata_type *)


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

static std::vector<std::string> args;  // use for templates and repeat templates


static std::pair<std::string, int> eval_cpp_arglist_r(const std::string& str, bool eval_one=false) { // TODO: static? const?
    if ((str == "v") && (!eval_one)) {
        return {"", 1};
    }

    std::string result;
    std::size_t i = 0;
    bool arglist_done = false;
    while (i < str.size() && !arglist_done) {
        int ix;
        int repeat_arg = 1;
        bool arg_done = false;
        bool is_unsigned = false;
        bool is_signed = false;
        bool is_volatile = false;
        uint32_t ptr = 0;
        std::string arg;
        while (i < str.size() && !arg_done) {
            char c = str[i++];
            switch (c) {
                case '_':
                    arglist_done = true;
                    arg_done = true;
                    i--; // don't consume the '_' character
                    break;
                case 'U': is_unsigned = true; break;
                case 'S': is_signed = true; break;
                case 'V': is_volatile = true; break;
                case 'P':
                    ptr <<= 4; // save previous pointer statements, if any
                    ptr |= 1;  // mark this as a pointer
                    break;
                case 'R':
                    ptr <<= 4; // save previous pointer statements, if any
                    ptr |= 2;  // mark this as a reference
                    break;
                case 'C':
                    ptr |= 4;  // mark this as const
                    break;
                case 'N':
                    c = str[i++];
                    repeat_arg = c - '0';
                    /* fall through */
                case 'T': // template
                    c = str[i++]; // assume we have never more than 9 template arguments
                    if (!isdigit(c)) {
                        std::cout << "WARNING: unexpected character after 'T' in C++ symbol argument list: " << c
                            << " in " << str << std::endl;
                        arg_done = true;
                        break;
                    }
                    ix = c - '1';
                    if ((ix < 0) || (ix >= static_cast<int>(args.size()))) {
                        std::cout << "WARNING: unexpected template argument index after 'T' in C++ symbol argument list: " << c
                            << " in " << str << std::endl;
                        arg_done = true;
                        break;
                    }
                    arg = args[ix];
                    arg_done = true;
                    break;
                case 'c':
                    if (is_unsigned) arg += "unsigned ";
                    if (is_signed) arg += "signed ";
                    if (is_volatile) arg += "volatile ";
                    arg += "char";
                    arg_done = true;
                    break;
                case 's':
                    if (is_unsigned) arg += "unsigned ";
                    if (is_signed) arg += "signed ";
                    if (is_volatile) arg += "volatile ";
                    arg += "short";
                    arg_done = true;
                    break;
                case 'i':
                    if (is_unsigned) arg += "unsigned ";
                    if (is_signed) arg += "signed ";
                    if (is_volatile) arg += "volatile ";
                    arg += "int";
                    arg_done = true;
                    break;
                case 'l':
                    if (is_unsigned) arg += "unsigned ";
                    if (is_signed) arg += "signed ";
                    if (is_volatile) arg += "volatile ";
                    arg += "long";
                    arg_done = true;
                    break;
                case 'v':
                    arg = "void";
                    arg_done = true;
                    break;
                case 'e':
                    arg = "...";
                    arg_done = true;
                    break;
                case 'd':
                    if (is_volatile) arg += "volatile ";
                    arg += "double";
                    arg_done = true;
                    break;
                case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                    {
                        auto [name, bytes_to_skip] = nn_symbol(str.substr(i-1));
                        if (!name.empty()) {
                            arg = name;
                            i += bytes_to_skip - 1;
                        } else {
                            std::cout << "WARNING: failed to parse C++ symbol argument type at index "<< (i-1) << " in " << str << std::endl;
                        }
                        arg_done = true;
                    }
                    break;
                case 'F': // function
                    {
                        auto [name, bytes_to_skip] = eval_cpp_arglist_r(str.substr(i));
                        arg = "(" + name + ")";
                        i += bytes_to_skip;
                        // TODO: what about the return type?
                        if ((i < str.size()) && (str[i] == '_')) {
                            i++; // skip the '_' character
                            auto [return_type, return_bytes_to_skip] = eval_cpp_arglist_r(str.substr(i), true);
                            arg = arg + " -> " + return_type;
                            i += return_bytes_to_skip;
                        }
                    }
                    arg_done = true;
                    break;
                case 'Q': // class::type
                    {
                        int depth = str[i++] - '0';
                        while (depth > 0) {
                            auto [klass, bytes_to_skip] = eval_cpp_arglist_r(str.substr(i), true);
                            i += bytes_to_skip;
                            if (!klass.empty()) {
                                if (!arg.empty()) {
                                    arg += "::";
                                }
                                arg += klass;
                            } else {
                                std::cout << "WARNING: failed to parse C++ symbol class at index "<< (i-1) << " in " << str << std::endl;
                            }
                            depth--;
                        }
                    }
                    arg_done = true;
                    break;
                case 'A': // array
                    {
                        int array_size = 0;
                        while (i < str.size() && isdigit(str[i])) {
                            array_size = (array_size * 10) + (str[i] - '0');
                            i++;
                        }
                        if (i < str.size() && str[i] == '_') {
                            i++; // skip the '_' character
                        }

                    }
                    // TODO: need to replce 'ptr' with a small stack for ptr, ref, and array
                    arg_done = true;
                    break;
                case 'M': // member function pointer
                    {
                        auto [klass, bytes_to_skip] = eval_cpp_arglist_r(str.substr(i), true);
                        i += bytes_to_skip;
                        auto [func, func_bytes_to_skip] = eval_cpp_arglist_r(str.substr(i), true);
                        i += func_bytes_to_skip;
                        arg = "(" + klass + "::)" + func;
                    }
                    arg_done = true;
                    break;
                default:
                    std::cout << "WARNING: unrecognized C++ symbol argument type: " << c
                        << " in " << str << std::endl;
                    arg_done = true;
                    break;
            }
        }
        if (!arg.empty()) {
            gTypeMap.tag_type(arg);
            std::string mod;
            while (ptr) { // TODO: different for pointer to function
                if (ptr & 0x4) mod += " const";
                if (ptr & 0x2) mod += "&";
                if (ptr & 0x1) mod += "*";
                ptr >>= 4;
            }
            if (arg.starts_with("(")) {
                arg = "(" + mod + ")" + arg;
            } else {
                arg = arg + mod;
            }
            gTypeMap.tag_type(arg);
            for (; repeat_arg > 0; --repeat_arg) {
                args.push_back(arg);
                if (!result.empty()) {
                    result += ", ";
                }
                result += arg;
            }
        }
        if (eval_one) {
            arglist_done = true;
        }
    }
    return {result, i};
}

static std::pair<std::string, int> eval_cpp_arglist(const std::string& str) { // TODO: static? const?
    args.clear();
    return eval_cpp_arglist_r(str);
}

/**
 * Decode a decorated C++ label.
 */
Symbol* eval_cpp_symbol(const Label& sym) {
    bool is_static = false;
    bool is_const = false;
    std::string label = sym.name;
    if (label.starts_with("__")) return nullptr;
    auto uu = label.find("__");
    if (uu == std::string::npos) return nullptr;

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
        gAttr.at(sym.address).set_unresolved_call_target();
        auto [arglist, bytes_to_skip] = eval_cpp_arglist(label.substr(uu));
        uu += bytes_to_skip;
        // TODO: static, const, return type, etc.
        arglist = "(" + arglist + ")";
        if (klass.empty()) {
            Type* t = gTypeMap.tag_function_type(arglist);
            FunctionSymbolInfo* info = new FunctionSymbolInfo{}; // arg names are not known
            Symbol* s = new Symbol{name, t, sym.address, info};
            gLabelByName[sym.name]->symbol = s;
        } else {
            Type* c = gTypeMap.tag_class(klass);
            Type* t = gTypeMap.tag_member_function_type(klass, arglist);
            Symbol* s = c->tag_member_function(name, t);
            s->address = sym.address;
            gLabelByName[sym.name]->symbol = s;
        }
    } else {
        std::cout << "WARNING: unexpected C++ symbol format: " << label
            << " at 0x" << std::hex << std::setw(8) << std::setfill('0')
            << sym.address << " type: " << std::dec << static_cast<int>(sym.type)
            << " ID: " << label[uu] << std::endl;
    }
    return nullptr;
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
    if (cpp_symbols_op_done) {
        return 0;
    }
    cpp_symbols_op_done = true;

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


    for (const auto& [addr, label] : gLabelByAddress) {
        if (addr >= k8MB) break; // TODO: too strict! See system variables!
        if (label->name.starts_with("SYM__")) continue;
        auto pos = label->name.find("__");
        if ((pos != std::string::npos) && (pos != 0)) { // TODO: avoid special cases for now
            eval_cpp_symbol(*label);
            // std::cout << "C++ symbol: " << sym.name << " at 0x" << std::hex << std::setw(8) << std::setfill('0') << addr
            //           << " type: " << std::dec << static_cast<int>(sym.type) << std::endl;
        }
    }

    gTypeMap.print();
    return 0;
}
