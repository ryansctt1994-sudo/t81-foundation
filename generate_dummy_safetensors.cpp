#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
  if (argc < 2) {
      std::cerr << "Usage: " << argv[0] << " <output_path>\n";
      return 1;
  }
  std::string output_path = argv[1];

  std::string header = R"(
{"tensor":{"dtype":"F32","shape":[129],"data_offsets":[0],"data_lengths":[516]}}
)";

  std::ofstream out(output_path, std::ios::binary);
  if (!out) {
    std::cerr << "unable to create safetensors stub\n";
    return 1;
  }
  uint64_t header_len = static_cast<uint64_t>(header.size());
  out.write(reinterpret_cast<const char*>(&header_len), sizeof(header_len));
  out.write(header.data(), header_len);
  for (uint64_t i = 0; i < 129; ++i) {
    float value = static_cast<float>(i) * 0.01f;
    out.write(reinterpret_cast<const char*>(&value), sizeof(value));
  }
  out.close();
  std::cout << "Generated " << output_path << "\n";
  return 0;
}
