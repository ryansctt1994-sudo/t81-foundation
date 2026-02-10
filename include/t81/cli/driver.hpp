#pragma once

#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "t81/tisc/program.hpp"

namespace t81::weights {
struct ModelFile;
}

namespace t81::cli {

std::optional<t81::tisc::Program> build_program_from_source(
    const std::string& source,
    const std::string& diag_name,
    const std::shared_ptr<t81::weights::ModelFile>& weights_model = nullptr);

int compile(const std::filesystem::path& input,
            const std::filesystem::path& output,
            const std::string& source_override = {},
            const std::string& source_name = {},
            std::shared_ptr<t81::weights::ModelFile> weights_model = nullptr);
int run_tisc(const std::filesystem::path& path);
int disasm_tisc(const std::filesystem::path& path);
int debug_tisc(const std::filesystem::path& path);
int check_syntax(const std::filesystem::path& path);
int repl(const std::shared_ptr<t81::weights::ModelFile>& weights_model = nullptr,
         std::istream& input = std::cin);
int init_project(const std::string& name);
int init_package(const std::string& name);

struct TraceArgs {
    std::string subcommand;
    std::vector<std::string> args;
};
int run_trace(const TraceArgs& args);

} // namespace t81::cli
