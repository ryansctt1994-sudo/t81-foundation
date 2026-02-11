#include "t81/cli/driver.hpp"
#include "test_runtime_check.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

int main() {
    // Create temporary files for the test
    [[maybe_unused]] fs::path temp_dir= fs::temp_directory_path();
    [[maybe_unused]] fs::path t81_file= temp_dir / "test.t81";
    [[maybe_unused]] fs::path tisc_file= temp_dir / "test.tisc";

    // Write a simple T81Lang program to the source file
    {
        std::ofstream out(t81_file);
        out << R"(
            fn main() -> i32 {
                return 42;
            }
        )";
    }

    // Compile the source file
    [[maybe_unused]] int compile_result= t81::cli::compile(t81_file, tisc_file);
    T81_TEST_CHECK(compile_result == 0);

    // Run the compiled TISC binary
    [[maybe_unused]] int run_result= t81::cli::run_tisc(tisc_file);
    T81_TEST_CHECK(run_result == 0);

    // Clean up temporary files
    fs::remove(t81_file);
    fs::remove(tisc_file);

    std::cout << "CLI e2e test passed!" << std::endl;
    return 0;
}
