#include "t81/cli/driver.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

int main() {
  // Setup a temporary directory for the test
  std::random_device rd;
  std::mt19937_64 rng(rd());
  std::uniform_int_distribution<uint64_t> dist;
  fs::path temp_dir =
      fs::temp_directory_path() / ("t81_security_test_" + std::to_string(dist(rng)));

  try {
    fs::create_directories(temp_dir);
    fs::current_path(temp_dir);
  } catch (const std::exception& e) {
    std::cerr << "Failed to create/enter temp dir: " << e.what() << std::endl;
    return 1;
  }

  // Case 1: Malicious name injection
  std::string malicious_name = "mypkg\")\n  (dangerous-command \"rm -rf /\"";

  // Call init_package
  int rc = t81::cli::init_package(malicious_name);

  // Verify the file content
  std::ifstream pkg_file("package.t81");
  std::stringstream buffer;
  if (pkg_file) {
    buffer << pkg_file.rdbuf();
  }
  std::string content = buffer.str();

  // Cleanup
  try {
    fs::current_path(fs::temp_directory_path());  // Move out before deleting
    fs::remove_all(temp_dir);
  } catch (...) {
  }

  // Check for vulnerability
  bool injection_successful = content.find("(dangerous-command \"rm -rf /\"") != std::string::npos;

  if (rc == 0 && injection_successful) {
    std::cerr << "SECURITY FAILURE: Configuration injection successful!" << std::endl;
    std::cerr << "Generated content:\n" << content << std::endl;
    return 1;
  }

  if (rc != 0) {
    std::cout << "SUCCESS: init_package rejected the malicious name." << std::endl;
    return 0;
  }

  if (!injection_successful) {
    std::cout << "SUCCESS: init_package sanitized the output." << std::endl;
    return 0;
  }

  return 0;
}
