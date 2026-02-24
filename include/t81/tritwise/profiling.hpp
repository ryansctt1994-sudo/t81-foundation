#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <string_view>

#ifdef T81_TRITWISE_PROFILE

namespace t81::tritwise {

class Profiler {
public:
  struct OpStats {
    uint64_t calls = 0;
    uint64_t total_bytes = 0;
    // Bins: <64, 64-256, 256-1024, >1024
    uint64_t hist_lt_64 = 0;
    uint64_t hist_64_256 = 0;
    uint64_t hist_256_1024 = 0;
    uint64_t hist_gt_1024 = 0;
  };

  static Profiler& get() {
    static Profiler instance;
    return instance;
  }

  void record(std::string_view op, size_t bytes) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto& stats = stats_[std::string(op)];
    stats.calls++;
    stats.total_bytes += bytes;
    if (bytes < 64)
      stats.hist_lt_64++;
    else if (bytes < 256)
      stats.hist_64_256++;
    else if (bytes < 1024)
      stats.hist_256_1024++;
    else
      stats.hist_gt_1024++;
  }

  void report() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "\n=== Tritwise Profiling Report ===\n";
    for (const auto& [op, s] : stats_) {
      std::cout << "Op: " << op << "\n"
                << "  Calls: " << s.calls << "\n"
                << "  Total Bytes: " << s.total_bytes << "\n"
                << "  Histogram:\n"
                << "    <64 B:     " << s.hist_lt_64 << "\n"
                << "    64-256 B:  " << s.hist_64_256 << "\n"
                << "    256-1KB:   " << s.hist_256_1024 << "\n"
                << "    >1 KB:     " << s.hist_gt_1024 << "\n";
    }
    std::cout << "=================================\n";
  }

  ~Profiler() {
    // Only print on destruction if stats were collected
    if (!stats_.empty()) {
      report();
    }
  }

private:
  mutable std::mutex mutex_;
  std::map<std::string, OpStats> stats_;
};

}  // namespace t81::tritwise

#define T81_PROFILE_RECORD(op, bytes) t81::tritwise::Profiler::get().record(op, bytes)

#else

#define T81_PROFILE_RECORD(op, bytes) \
  do {                                \
  } while (0)

#endif
