#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "t81/tisc/base81_view.hpp"
#include "t81/tisc/encoding.hpp"

using namespace t81::tisc;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <encode|decode> <file>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string filename = argv[2];

    if (mode == "decode") {
        // Binary -> Base81
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs) { std::cerr << "Cannot open " << filename << std::endl; return 1; }
        std::vector<std::byte> bytes((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        auto program_res = decode(bytes);
        if (!program_res) {
            std::cerr << "Failed to decode binary bytecode" << std::endl;
            return 1;
        }

        std::cout << base81_view::render(program_res.value()) << std::endl;
    } else if (mode == "encode") {
        // Base81 -> Binary
        std::ifstream ifs(filename);
        if (!ifs) { std::cerr << "Cannot open " << filename << std::endl; return 1; }
        std::string s((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        auto program_res = base81_view::parse(s);
        if (!program_res) {
            std::cerr << "Failed to parse Base-81 string: " << program_res.error().explain().str() << std::endl;
            return 1;
        }

        auto bytes = encode(program_res.value());
        std::ofstream ofs(filename + ".bin", std::ios::binary);
        ofs.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        std::cout << "Encoded to " << filename << ".bin" << std::endl;
    } else {
        std::cerr << "Unknown mode: " << mode << std::endl;
        return 1;
    }

    return 0;
}
