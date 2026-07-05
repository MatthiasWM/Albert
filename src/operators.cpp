


#include "operators.h"

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <string_view>


class Operators {
public:
    std::string_view name;
    std::string_view summary;
    std::string_view fallback;
    int (*runner)(const std::string_view arg);
    const Operators* dependencies[10];
    int run(const std::string_view arg) const {
        int ret = 0;
        // run all dependencies first
        for (const auto* dep : dependencies) {
            if (!dep) break;
            ret = dep->run(dep->fallback);
            if (ret < 0) return ret;
        }
        if (runner) {
            if (!arg.empty()) {
                ret = runner(arg);
            } else {
                ret = runner(fallback);
            }
        }
        return ret;
    }
};


extern int aif_load_binary(const std::string_view arg);
constexpr Operators op_aif {
     "--aif", "load aif ROM file", "input/Senior CirrusNoDebug image",
     &aif_load_binary, {} };
extern int aif_load_symbols(const std::string_view arg);
constexpr Operators op_aif_symbols {
     "--aif-symbols", "load aif ROM symbols", "input/Senior CirrusNoDebug image",
     &aif_load_symbols, {} };
extern int eval_cpp_symbols(const std::string_view arg);
constexpr Operators op_eval_cpp_symbols {
     "--cpp-symbols", "evaluate C++ symbols", "",
     &eval_cpp_symbols, { &op_aif_symbols } };
extern int rex_load_binary(const std::string_view arg);
constexpr Operators op_rex = {
    "--rex", "load the ROM extension", "input/Senior CirrusNoDebug high",
    &rex_load_binary, { &op_aif } };
extern int out_asm(const std::string_view arg);
constexpr Operators op_out_asm = {
    "--out-asm", "generate an annotated ARM32 disassembly", "output/newtonos.s",
    &out_asm, { &op_aif, &op_rex, &op_eval_cpp_symbols } };
extern int out_binary(const std::string_view arg);
constexpr Operators op_out_binary = {
    "--out-bin", "generate a binary dump of the ROM data", "output/717006.bin",
    &out_binary, { &op_aif, &op_rex } };
extern int print_help(const std::string_view arg);
constexpr Operators op_help = {
    "-h", "print help text", "",
    &print_help, { } };

static constexpr Operators const* ops[] = {
    &op_aif,
    &op_rex,
    &op_out_asm,
    &op_out_binary,
    &op_help
};

/**
 * Executes operators in the order provided by parsed command-line arguments.
 * @param args Operator/argument pairs.
 * @return 0 on success, negative value when an operator is unknown.
 */
int apply_operators(const std::vector<std::pair<std::string, std::string>>& args) {
    bool need_help = true; // Assume help is needed unless an operator is found.
    for (const auto& [op_name, arg] : args) {
        bool found = false;
        for (const auto* op : ops) {
            if (op->name == op_name) {
                found = true;
                op->run(arg);
                break;
            }
        }
        if (!found) {
            std::cerr << "Error: Unknown operator " << op_name << "\n";
            return -1;
        } else {
            need_help = false;
        }
    }
    if (need_help) {
        print_help("");
    }
    return 0;
}

int print_help(const std::string_view arg) {
    std::cout << "Albert ROM image tool\n";
    std::cout << "Usage: albert [operator] [argument]\n";
    std::cout << "Available operators:\n";
    for (const auto* op : ops) {
        std::cout << "  " << op->name << ": " << op->summary;
        if (!op->fallback.empty()) {
            std::cout << " (default: " << op->fallback << ")";
        }
        std::cout << "\n";
    }
    return 0;
}
