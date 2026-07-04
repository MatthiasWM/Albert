


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
int rex_load_binary(const std::string_view arg);
constexpr Operators op_rex = {
    "--rex", "load the ROM extension", "input/Senior CirrusNoDebug high",
    &rex_load_binary, { &op_aif } };
constexpr Operators op_out_asm = {
    "--out-asm", "generate an annotated ARM32 disassembly", "output/newtonos.s",
    nullptr, { &op_aif, &op_rex } };
extern int out_binary(const std::string_view arg);
constexpr Operators op_out_binary = {
    "--out-bin", "generate a binary dump of the ROM data", "output/717006.bin",
    &out_binary, { &op_aif, &op_rex } };

static constexpr Operators const* ops[] = {
    &op_aif,
    &op_rex,
    &op_out_asm,
    &op_out_binary,
};

/**
 * Executes operators in the order provided by parsed command-line arguments.
 * @param args Operator/argument pairs.
 * @return 0 on success, negative value when an operator is unknown.
 */
int apply_operators(const std::vector<std::pair<std::string, std::string>>& args) {
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
        }
    }
    return 0;
}