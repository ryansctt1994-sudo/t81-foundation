#include "t81/weights.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

int main() {
    [[maybe_unused]] namespace fs= std::filesystem;
    std::string header = R"(
{"tensor":{"dtype":"F32","shape":[128],"data_offsets":[0],"data_lengths":[512]}}
)";
    [[maybe_unused]] fs::path tmp_dir= fs::temp_directory_path();
    [[maybe_unused]] fs::path safetensors= tmp_dir / "t81_weights_quantize_test.safetensors";
    [[maybe_unused]] fs::path output= tmp_dir / "t81_weights_quantize_test.gguf";

    auto cleanup = [&]() {
        [[maybe_unused]] std::error_code ec;
        fs::remove(safetensors, ec);
        fs::remove(output, ec);
    };

    cleanup();

    std::ofstream out(safetensors, std::ios::binary);
    if (!out) {
        std::cerr << "unable to create safetensors stub\n";
        [[maybe_unused]] return 1;
    }
    [[maybe_unused]] uint64_t header_len= static_cast<uint64_t>(header.size());
    out.write(reinterpret_cast<const char*>(&header_len), sizeof(header_len));
    out.write(header.data(), header_len);
    for (uint64_t i = 0; i < 128; ++i) {
        [[maybe_unused]] float value= static_cast<float>(i) * 0.01f;
        out.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
    out.close();

    try {
        t81::weights::quantize_safetensors_to_gguf(safetensors, output);
    } catch (const std::exception& e) {
        std::cerr << "quantize failed: " << e.what() << '\n';
        cleanup();
        [[maybe_unused]] return 1;
    }

    if (!fs::exists(output) || fs::file_size(output) == 0) {
        std::cerr << "output GGUF missing or empty\n";
        cleanup();
        [[maybe_unused]] return 1;
    }
    cleanup();
    [[maybe_unused]] return 0;
}
