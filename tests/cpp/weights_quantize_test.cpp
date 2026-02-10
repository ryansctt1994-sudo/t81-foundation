#include "t81/weights.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

namespace {

uint32_t read_u32(const std::vector<uint8_t>& bytes, size_t& off) {
    if (off + 4 > bytes.size()) throw std::runtime_error("read_u32 out of bounds");
    uint32_t v = static_cast<uint32_t>(bytes[off]) |
                 (static_cast<uint32_t>(bytes[off + 1]) << 8) |
                 (static_cast<uint32_t>(bytes[off + 2]) << 16) |
                 (static_cast<uint32_t>(bytes[off + 3]) << 24);
    off += 4;
    return v;
}

uint64_t read_u64(const std::vector<uint8_t>& bytes, size_t& off) {
    if (off + 8 > bytes.size()) throw std::runtime_error("read_u64 out of bounds");
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) {
        v |= static_cast<uint64_t>(bytes[off + i]) << (8 * i);
    }
    off += 8;
    return v;
}

std::string read_str(const std::vector<uint8_t>& bytes, size_t& off) {
    uint64_t n = read_u64(bytes, off);
    if (off + n > bytes.size()) throw std::runtime_error("read_str out of bounds");
    std::string s(reinterpret_cast<const char*>(bytes.data() + off), static_cast<size_t>(n));
    off += static_cast<size_t>(n);
    return s;
}

size_t align_up(size_t v, size_t a) {
    return a == 0 ? v : ((v + a - 1) / a) * a;
}

} // namespace

int main() {
    namespace fs= std::filesystem;
    std::string header = R"(
{"tensor":{"dtype":"F32","shape":[129],"data_offsets":[0],"data_lengths":[516]}}
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
        return 1;
    }
    [[maybe_unused]] uint64_t header_len= static_cast<uint64_t>(header.size());
    out.write(reinterpret_cast<const char*>(&header_len), sizeof(header_len));
    out.write(header.data(), header_len);
    for (uint64_t i = 0; i < 129; ++i) {
        [[maybe_unused]] float value= static_cast<float>(i) * 0.01f;
        out.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
    out.close();

    try {
        t81::weights::quantize_safetensors_to_gguf(safetensors, output);
    } catch (const std::exception& e) {
        std::cerr << "quantize failed: " << e.what() << '\n';
        cleanup();
        return 1;
    }

    if (!fs::exists(output) || fs::file_size(output) == 0) {
        std::cerr << "output GGUF missing or empty\n";
        cleanup();
        return 1;
    }

    // GGUF v3 header and T3_K layout sanity checks.
    {
        std::ifstream gguf_in(output, std::ios::binary | std::ios::ate);
        if (!gguf_in) {
            std::cerr << "unable to reopen GGUF output\n";
            cleanup();
            return 1;
        }
        std::vector<uint8_t> bytes(static_cast<size_t>(gguf_in.tellg()));
        gguf_in.seekg(0, std::ios::beg);
        gguf_in.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        if (!gguf_in || bytes.size() < 24) {
            std::cerr << "unable to read GGUF bytes\n";
            cleanup();
            return 1;
        }
        if (!(bytes[0] == 'G' && bytes[1] == 'G' && bytes[2] == 'U' && bytes[3] == 'F')) {
            std::cerr << "invalid GGUF magic\n";
            cleanup();
            return 1;
        }
        size_t off = 4;
        uint32_t version = read_u32(bytes, off);
        if (version != 3) {
            std::cerr << "invalid GGUF version: " << version << '\n';
            cleanup();
            return 1;
        }
        uint64_t tensor_count = read_u64(bytes, off);
        uint64_t kv_count = read_u64(bytes, off);
        if (tensor_count != 1) {
            std::cerr << "unexpected tensor count: " << tensor_count << '\n';
            cleanup();
            return 1;
        }

        bool saw_version = false;
        bool saw_source_sha3 = false;
        for (uint64_t i = 0; i < kv_count; ++i) {
            std::string key = read_str(bytes, off);
            uint32_t type = read_u32(bytes, off);
            if (type == 8) {
                (void)read_str(bytes, off);
            } else if (type == 4) {
                uint32_t value = read_u32(bytes, off);
                if (key == "t3_k.version" && value == 1) {
                    saw_version = true;
                }
            } else if (type == 9) {
                uint32_t elem_type = read_u32(bytes, off);
                uint64_t elem_count = read_u64(bytes, off);
                if (off + elem_count > bytes.size()) {
                    std::cerr << "array kv out of bounds\n";
                    cleanup();
                    return 1;
                }
                if (key == "t3_k.source_sha3" && elem_type == 0 && elem_count == 64) {
                    saw_source_sha3 = true;
                }
                off += static_cast<size_t>(elem_count);
            } else {
                std::cerr << "unexpected kv type: " << type << '\n';
                cleanup();
                return 1;
            }
        }

        if (!saw_version || !saw_source_sha3) {
            std::cerr << "missing required t3_k metadata keys\n";
            cleanup();
            return 1;
        }

        std::string tensor_name = read_str(bytes, off);
        (void)tensor_name;
        uint32_t ndims = read_u32(bytes, off);
        uint64_t element_count = 1;
        for (uint32_t i = 0; i < ndims; ++i) {
            const uint64_t dim = read_u64(bytes, off);
            element_count *= dim;
        }
        uint32_t ggml_type = read_u32(bytes, off);
        uint64_t tensor_data_offset = read_u64(bytes, off);
        if (ggml_type != 99) {
            std::cerr << "unexpected ggml type: " << ggml_type << '\n';
            cleanup();
            return 1;
        }

        const size_t data_base = align_up(off, 32);
        const size_t block_offset = data_base + static_cast<size_t>(tensor_data_offset);
        const size_t block_count = (element_count + 127) / 128;
        const size_t block_size = 30; // 4-byte fp32 scale + 26 PT5 bytes
        const size_t tensor_bytes = block_count * block_size;
        if (block_offset + tensor_bytes > bytes.size()) {
            std::cerr << "t3 block out of bounds\n";
            cleanup();
            return 1;
        }
        for (size_t block = 0; block < block_count; ++block) {
            const uint8_t* packed = bytes.data() + block_offset + (block * block_size) + 4;
            for (size_t i = 0; i < 26; ++i) {
                if (packed[i] > 242) {
                    std::cerr << "invalid PT5 byte > 242\n";
                    cleanup();
                    return 1;
                }
            }
        }

        // Validate canonical padding on the final block: all logical padding trits must map to digit=1.
        const uint8_t* last_packed = bytes.data() + block_offset + ((block_count - 1) * block_size) + 4;
        const size_t final_count = element_count - (block_count - 1) * 128;
        size_t trit_index = 0;
        for (size_t i = 0; i < 26; ++i) {
            uint8_t value = last_packed[i];
            for (size_t d = 0; d < 5; ++d, ++trit_index) {
                uint8_t digit = static_cast<uint8_t>(value % 3);
                value = static_cast<uint8_t>(value / 3);
                if (trit_index >= final_count && digit != 1) {
                    std::cerr << "non-canonical PT5 padding digits\n";
                    cleanup();
                    return 1;
                }
            }
        }
    }

    cleanup();
    return 0;
}
