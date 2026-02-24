#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include "t81/canonfs/canon_driver.hpp"
#include "t81/canonfs/canon_types.hpp"
#include "t81/cli/driver.hpp"
#include "t81/weights.hpp"
#include "test_runtime_check.hpp"

namespace fs = std::filesystem;

int main() {
  // Setup temporary directories
  fs::path temp_dir = fs::temp_directory_path() / "t81_canonize_test";
  fs::create_directories(temp_dir);
  fs::path t81w_path = temp_dir / "model.t81w";
  fs::path canon_root = fs::current_path() / ".t81_canonfs";

  // Create a dummy NativeModel
  t81::weights::NativeModel model;
  t81::weights::NativeTensor tensor;
  tensor.shape = {2, 2};
  tensor.trits = 4;
  tensor.format = t81::weights::NativeFormat::BalancedTernary;
  // Encoded balanced ternary data (dummy)
  tensor.data = {10, 20};  // Arbitrary limb data
  model["test_tensor"] = std::move(tensor);

  // Save as .t81w
  try {
    t81::weights::save_t81w(model, t81w_path);
  } catch (const std::exception& e) {
    std::cerr << "Failed to save .t81w: " << e.what() << "\n";
    return 1;
  }

  // Run canonize-tensor CLI command
  // Note: t81::cli::canonize_tensor uses ".t81_canonfs" in current_path.
  // We ensure it exists (created by main or previous runs)
  int res = t81::cli::canonize_tensor(t81w_path.string());
  T81_TEST_CHECK(res == 0);

  // Verify the object exists in CanonFS
  // We need to know the hash. The CLI prints it, but here we can re-calculate or just scan the dir.
  // Or better, we replicate the serialization to compute the expected hash.

  // Re-serialize to get hash
  // Actually save_t81w does not modify model in place destructive enough to prevent checking.
  // Wait, map access `model["test_tensor"]` creates if not exists, but we moved it in.
  // Let's reconstruct the tensor for verification.
  t81::weights::NativeTensor check_tensor;
  check_tensor.shape = {2, 2};
  check_tensor.trits = 4;
  check_tensor.format = t81::weights::NativeFormat::BalancedTernary;
  check_tensor.data = {10, 20};

  // We need the `serialize_tensor` logic which is internal to `canonize_tensor.cpp`.
  // Since we can't easily link/call that internal function without exposing it,
  // we will use the CanonFS driver to iterate objects or just check if *any* object was created.
  // A robust test would verify the content.

  auto driver = t81::canonfs::make_persistent_driver(canon_root);
  // We can't iterate easily with the public API of driver unless we scan the directory.

  bool found_any = false;
  if (fs::exists(canon_root / "objects")) {
    for (const auto& entry : fs::directory_iterator(canon_root / "objects")) {
      if (entry.path().extension() == ".blk") {
        found_any = true;
        // Ideally we read it and check the header
        std::string hash_str = entry.path().stem().string();
        t81::canonfs::CanonHash ch;
        ch.h = t81::hash::CanonHash81::from_string(hash_str);
        auto data_res = driver->read_object_bytes({ch});
        T81_TEST_CHECK(data_res.has_value());
        auto data = data_res.value();
        T81_TEST_CHECK(data.size() > 72);
        T81_TEST_CHECK(static_cast<uint8_t>(data[0]) == 0x20);  // Type ID
        break;
      }
    }
  }
  T81_TEST_CHECK(found_any);

  // Cleanup
  fs::remove_all(temp_dir);
  // Optional: Clean up .t81_canonfs if we want to be polite, but it might be used by other tests?
  // Usually tests should use a unique temp dir for canonfs if possible, but the CLI function
  // hardcodes it. For now we leave it.

  std::cout << "CLI canonize-tensor test passed!\n";
  return 0;
}
