#include <benchmark/benchmark.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <map>
#include <mutex>
#include <array>
#include <cstdio>
#include <limits>
#include <sstream>
#include <cmath>
#include <cctype>

struct BenchmarkResult {
    std::string name;
    std::string t81_result_str;
    std::string t81_native_result_str;
    std::string binary_result_str;
    double t81_result_val = 0.0;
    double t81_native_result_val = 0.0;
    double binary_result_val = 0.0;
    double t81_latency_seconds = 0.0;
    double t81_native_latency_seconds = 0.0;
    double binary_latency_seconds = 0.0;
    std::string bandwidth_result_str;
    double t81_bandwidth_val = 0.0;
    double t81_native_bandwidth_val = 0.0;
    double binary_bandwidth_val = 0.0;
    std::string t81_classic_advantage;
    std::string t81_native_advantage;
    std::string t81_classic_note;
    std::string t81_native_note;
    std::string binary_note;
    std::string t81_latency_str;
    std::string t81_native_latency_str;
    std::string binary_latency_str;
    std::string analysis;
    bool t81_classic_skipped = false;
    bool t81_native_skipped = false;
    bool binary_skipped = false;
    std::string t81_classic_skip_reason;
    std::string t81_native_skip_reason;
    std::string binary_skip_reason;
    bool has_t81_flow = false;
    bool has_t81_native_flow = false;
    bool has_binary_flow = false;
    bool throughput_ratio_computed = false;
    std::string throughput_ratio_str;
    double throughput_ratio_val = 0.0;
    bool latency_speedup_computed = false;
    std::string latency_speedup_str;
    double latency_speedup_val = 0.0;
};

std::map<std::string, BenchmarkResult> final_results;
std::mutex final_results_mutex;

const std::map<std::string, std::pair<std::string, std::string>> T81_ADVANTAGES = {
    {"BM_T81LangCompile", {"Deterministic frontend compile", {}}},
    {"BM_ArithThroughput", {"Exact rounding, no FP error", {}}},
    {"BM_NegationSpeed", {"Free negation (no borrow)", {}}},
    {"BM_RoundtripAccuracy", {"No sign-bit tax", {}}},
    {"BM_OverflowDetection", {"Deterministic, provable", {}}},
    {"BM_PackingDensity_Theoretical", {"Theoretical maximum", {}}},
    {"BM_PackingDensity_Achieved", {"Achieved bits/trit", {}}},
    {"BM_PackingDensity_Practical", {"Practical size ratio", {}}},
    {"BM_LimbArithThroughput", {"48-trit Kogge-Stone addition", {}}},
    {"BM_NegationSpeed_T81Native", {{}, "PSHUFB-powered native negation"}},
    {"BM_LimbAdd_T81Native", {{}, "Register-native prefix addition"}}
};

static FILE* OpenCommandPipe(const std::string& command) {
#ifdef _WIN32
    return _popen(command.c_str(), "r");
#else
    return popen(command.c_str(), "r");
#endif
}

static int CloseCommandPipe(FILE* pipe) {
#ifdef _WIN32
    return _pclose(pipe);
#else
    return pclose(pipe);
#endif
}

std::string RunCommand(const std::string& command) {
    std::array<char, 128> buffer;
    std::string output;
    FILE* pipe = OpenCommandPipe(command);
    if (!pipe) {
        return {};
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        output += buffer.data();
    }
    CloseCommandPipe(pipe);
    while (!output.empty() && (output.back() == '\n' || output.back() == '\r')) {
        output.pop_back();
    }
    return output;
}

double ExtractLatency(const ::benchmark::BenchmarkReporter::Run& run) {
    static constexpr std::array<const char*, 2> kLatencyKeys = {"cpu_time", "real_time"};
    for (const auto* key : kLatencyKeys) {
        auto it = run.counters.find(key);
        if (it != run.counters.end()) {
            return it->second;
        }
    }
    double latency_seconds = 0.0;
    const double real_time = run.GetAdjustedRealTime();
    const double real_multiplier = GetTimeUnitMultiplier(run.time_unit);
    if (real_multiplier > 0.0) {
        latency_seconds = real_time / real_multiplier;
    } else {
        latency_seconds = real_time;
    }
    if (latency_seconds > 0.0) {
        return latency_seconds;
    }
    const double cpu_time = run.GetAdjustedCPUTime();
    const double cpu_multiplier = GetTimeUnitMultiplier(run.time_unit);
    if (cpu_multiplier > 0.0) {
        latency_seconds = cpu_time / cpu_multiplier;
    } else {
        latency_seconds = cpu_time;
    }
    if (latency_seconds > 0.0) {
        return latency_seconds;
    }
    return 0.0;
}

std::string FormatLatency(double seconds) {
    if (seconds <= 0.0) {
        return {};
    }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    if (seconds < 1e-6) {
        oss << (seconds * 1e9) << " ns";
    } else if (seconds < 1e-3) {
        oss << (seconds * 1e6) << " µs";
    } else if (seconds < 1.0) {
        oss << (seconds * 1e3) << " ms";
    } else {
        oss << seconds << " s";
    }
    return oss.str();
}

std::string FormatBandwidth(double bytes_per_second) {
    if (bytes_per_second <= 0.0) {
        return {};
    }
    struct Scale {
        double threshold;
        const char* suffix;
    };
    constexpr Scale kScales[] = {
        {1e9, "GB/s"},
        {1e6, "MB/s"},
        {1e3, "KB/s"},
    };
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    for (const auto& scale : kScales) {
        if (bytes_per_second >= scale.threshold) {
            oss << (bytes_per_second / scale.threshold) << " " << scale.suffix;
            return oss.str();
        }
    }
    oss << bytes_per_second << " B/s";
    return oss.str();
}

std::string FormatThroughput(double items_per_second) {
    if (items_per_second <= 0.0) {
        return {};
    }
    struct Scale {
        double threshold;
        const char* suffix;
    };
    constexpr Scale kScales[] = {
        {1e9, "Gops/s"},
        {1e6, "Mops/s"},
        {1e3, "Kops/s"},
    };
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    for (const auto& scale : kScales) {
        if (items_per_second >= scale.threshold) {
            oss << (items_per_second / scale.threshold) << " " << scale.suffix;
            return oss.str();
        }
    }
    oss << items_per_second << " ops/s";
    return oss.str();
}

enum class FlowKind {
    kUnknown,
    kT81Classic,
    kT81Native,
    kBinary,
};

static std::string ToLower(std::string_view input) {
    std::string result;
    result.reserve(input.size());
    for (unsigned char c : input) {
        result += static_cast<char>(std::tolower(c));
    }
    return result;
}

static bool HasFlowSuffix(std::string_view suffix) {
    if (suffix.empty()) return false;
    const std::string lower = ToLower(suffix);
    static constexpr std::array<std::string_view, 8> kIndicators = {
        "binary", "int", "t81", "packed", "cell", "native", "float", "ternary"
    };
    for (const auto indicator : kIndicators) {
        if (lower.find(indicator) != std::string::npos) {
            return true;
        }
    }
    return false;
}

static FlowKind DetermineFlowKind(const std::string& base_name, const std::string& suffix) {
    const std::string lower_base = ToLower(base_name);
    const std::string lower_suffix = ToLower(suffix);
    if (!suffix.empty()) {
        if (lower_suffix.find("binary") != std::string::npos ||
            lower_suffix.find("int") != std::string::npos) {
            return FlowKind::kBinary;
        }
        if (lower_suffix.find("native") != std::string::npos) {
            return FlowKind::kT81Native;
        }
        if (lower_suffix.find("t81") != std::string::npos ||
            lower_suffix.find("ternary") != std::string::npos ||
            lower_suffix.find("packed") != std::string::npos ||
            lower_suffix.find("cell") != std::string::npos) {
            return FlowKind::kT81Classic;
        }
    }
    if (lower_base.find("native") != std::string::npos) {
        return FlowKind::kT81Native;
    }
    if (lower_base.find("t81") != std::string::npos ||
        lower_base.find("ternary") != std::string::npos ||
        lower_base.find("packed") != std::string::npos) {
        return FlowKind::kT81Classic;
    }
    if (lower_base.find("int64") != std::string::npos ||
        lower_base.find("int128") != std::string::npos ||
        lower_base.find("binary") != std::string::npos) {
        return FlowKind::kBinary;
    }
    return FlowKind::kUnknown;
}

std::string BuildT81AdvantageDisplay(const BenchmarkResult& r) {
    std::string display;
    if (!r.t81_classic_advantage.empty()) {
        display = "Classic: " + r.t81_classic_advantage;
    }
    if (!r.t81_native_advantage.empty()) {
        if (!display.empty()) {
            display += " | ";
        }
        display += "Native: " + r.t81_native_advantage;
    }
    return display;
}

std::string BuildNotesDisplay(const BenchmarkResult& r) {
    std::ostringstream oss;
    bool first = true;
    if (!r.t81_classic_note.empty()) {
        oss << "Classic: " << r.t81_classic_note;
        first = false;
    }
    if (!r.t81_native_note.empty()) {
        if (!first) oss << " | ";
        oss << "Native: " << r.t81_native_note;
        first = false;
    }
    if (!r.binary_note.empty()) {
        if (!first) oss << " | ";
        oss << "Binary: " << r.binary_note;
    }
    return oss.str();
}

static bool BinaryBaselineSemanticallyUnavoidable(const BenchmarkResult& r);
static bool T81ClassicSemanticallyUnavoidable(const BenchmarkResult& r);

std::string BuildAnalysis(const BenchmarkResult& r) {
    std::ostringstream oss;
    if (!r.throughput_ratio_computed) {
        if (BinaryBaselineSemanticallyUnavoidable(r)) {
            oss << "Throughput ratio not computed (semantic: no binary baseline)";
        } else if (T81ClassicSemanticallyUnavoidable(r) &&
                   !(r.has_t81_flow || r.has_t81_native_flow)) {
            oss << "Throughput ratio not computed (semantic)";
        } else if ((r.t81_classic_skipped || r.t81_native_skipped || r.binary_skipped)) {
            oss << "Throughput ratio skipped (no counters emitted)";
        } else {
            oss << "Throughput ratio not-implemented (missing baseline)";
        }
    } else {
        const double ratio = r.throughput_ratio_val;
        oss << std::fixed << std::setprecision(2) << ratio << "x throughput ratio";
        if (ratio > 1.05) {
            oss << " — T81 leads";
            const auto advantage = BuildT81AdvantageDisplay(r);
            if (!advantage.empty()) {
                oss << " (" << advantage << ")";
            }
        } else if (ratio < 0.95) {
            oss << " — binary wins";
        } else {
            oss << " — throughputs comparable";
        }
    }
    if (r.latency_speedup_computed) {
        oss << "; " << std::fixed << std::setprecision(2) << r.latency_speedup_val
            << "x latency speedup (Binary/T81)";
    }
    if (!r.throughput_ratio_computed && !r.latency_speedup_computed) {
        return oss.str();
    }
    const std::string& comparable_t81_latency =
        (!r.t81_native_latency_str.empty() ? r.t81_native_latency_str : r.t81_latency_str);
    if (!comparable_t81_latency.empty() && !r.binary_latency_str.empty()) {
        oss << "; latencies " << comparable_t81_latency << " vs " << r.binary_latency_str;
    }
    return oss.str();
}

static bool NameContains(std::string_view name, std::string_view token) {
    const std::string lower_name = ToLower(name);
    const std::string lower_token = ToLower(token);
    return lower_name.find(lower_token) != std::string::npos;
}

static bool BinaryBaselineSemanticallyUnavoidable(const BenchmarkResult& r) {
    // Packing-density metrics are ternary-encoding properties, not direct throughput parity tests.
    if (NameContains(r.name, "PackingDensity")) return true;
    return false;
}

static bool T81ClassicSemanticallyUnavoidable(const BenchmarkResult& r) {
    // Binary silent-overflow semantics have no ternary equivalent in this project.
    if (NameContains(r.name, "overflow_binary_silent")) return true;
    return false;
}

class CustomReporter : public ::benchmark::BenchmarkReporter {
public:
    CustomReporter() {}
    bool ReportContext(const Context&) override { return true; }

    void ReportRuns(const std::vector<Run>& reports) override {
        std::lock_guard<std::mutex> guard(final_results_mutex);
        for (const auto& run : reports) {
            std::string run_name = run.benchmark_name();
            std::string family = run_name;
            std::string suffix;
            const auto slash_pos = run_name.find('/');
            if (slash_pos != std::string::npos) {
                family = run_name.substr(0, slash_pos);
                suffix = run_name.substr(slash_pos + 1);
            } else {
                family = run_name;
            }

            // Further strip flow suffixes from family (e.g., BM_Add_T81 -> BM_Add)
            const auto last_underscore = family.find_last_of('_');
            if (last_underscore != std::string::npos) {
                std::string candidate = family.substr(last_underscore + 1);
                if (HasFlowSuffix(candidate)) {
                    if (suffix.empty()) {
                        suffix = candidate;
                    } else {
                        suffix = candidate + "/" + suffix;
                    }
                    family = family.substr(0, last_underscore);
                }
            }
            if (family.empty()) {
                family = run_name;
            }

            const FlowKind flow_kind = DetermineFlowKind(family, suffix);
            const bool is_t81_classic = flow_kind == FlowKind::kT81Classic;
            const bool is_t81_native = flow_kind == FlowKind::kT81Native;
            const bool is_binary = flow_kind == FlowKind::kBinary;

            if (final_results.find(family) == final_results.end()) {
                final_results[family].name = family;
                if (auto it = T81_ADVANTAGES.find(family); it != T81_ADVANTAGES.end()) {
                    final_results[family].t81_classic_advantage = it->second.first;
                    final_results[family].t81_native_advantage = it->second.second;
                }
            }
            if (is_t81_classic) {
                final_results[family].t81_classic_note = run.report_label;
            }
            if (is_t81_native) {
                final_results[family].t81_native_note = run.report_label;
            }
            if (is_binary) {
                final_results[family].binary_note = run.report_label;
            }

            const bool skipped = run.skipped != benchmark::internal::NotSkipped;
            const std::string skip_reason = run.skip_message.empty() ?
                "no counters emitted" : run.skip_message;
            if (skipped) {
                if (is_t81_classic) {
                    final_results[family].t81_classic_skipped = true;
                    final_results[family].t81_classic_skip_reason = skip_reason;
                } else if (is_t81_native) {
                    final_results[family].t81_native_skipped = true;
                    final_results[family].t81_native_skip_reason = skip_reason;
                } else if (is_binary) {
                    final_results[family].binary_skipped = true;
                    final_results[family].binary_skip_reason = skip_reason;
                } else {
                    final_results[family].t81_classic_skipped = true;
                    final_results[family].t81_classic_skip_reason = skip_reason;
                }
                continue;
            }

            std::string summary;
            double gops = 0.0;
            bool throughput_recorded = false;
            double items_per_second = 0.0;
            auto items_it = run.counters.find("items_per_second");
            auto bandwidth_it = run.counters.find("bytes_per_second");
            if (items_it != run.counters.end()) {
                items_per_second = items_it->second;
            } else if (bandwidth_it == run.counters.end()) {
                // Fallback for benchmark libraries that don't surface items_per_second as a counter.
                const double latency_seconds = ExtractLatency(run);
                if (latency_seconds > 0.0 && run.iterations > 0) {
                    items_per_second = 1.0 / latency_seconds;
                }
            }
            if (items_per_second > 0.0) {
                if (items_per_second > 0.0) {
                    gops = items_per_second / 1e9;
                    throughput_recorded = true;
                }
                summary = FormatThroughput(items_per_second);
                if (summary.empty()) {
                    summary = "0 ops/s";
                }
            } else {
                std::stringstream ss;
                bool first = true;
                for (auto const& [key, val] : run.counters) {
                    if (!first) ss << ", ";
                    ss << key << ": " << std::fixed << std::setprecision(2) << val;
                    first = false;
                }
                summary = ss.str();
            }
            bool bandwidth_recorded = false;
            double bandwidth = 0.0;
            if (bandwidth_it != run.counters.end()) {
                bandwidth = bandwidth_it->second;
                if (bandwidth > 0.0) {
                    bandwidth_recorded = true;
                    summary = FormatBandwidth(bandwidth);
                    if (final_results[family].bandwidth_result_str.empty()) {
                        final_results[family].bandwidth_result_str = summary;
                    }
                }
            }

            double latency = ExtractLatency(run);
            std::string latency_str = FormatLatency(latency);

            if (is_t81_classic) {
                final_results[family].t81_result_str = summary;
                final_results[family].t81_latency_seconds = latency;
                final_results[family].t81_latency_str = latency_str;
                if (throughput_recorded) {
                    final_results[family].t81_result_val = gops;
                    final_results[family].has_t81_flow = true;
                }
                if (bandwidth_recorded) {
                    final_results[family].t81_bandwidth_val = bandwidth;
                    final_results[family].has_t81_flow = true;
                }
            } else if (is_t81_native) {
                final_results[family].t81_native_result_str = summary;
                final_results[family].t81_native_latency_seconds = latency;
                final_results[family].t81_native_latency_str = latency_str;
                if (throughput_recorded) {
                    final_results[family].t81_native_result_val = gops;
                    final_results[family].has_t81_native_flow = true;
                }
                if (bandwidth_recorded) {
                    final_results[family].t81_native_bandwidth_val = bandwidth;
                    final_results[family].has_t81_native_flow = true;
                }
            } else if (is_binary) {
                final_results[family].binary_result_str = summary;
                final_results[family].binary_latency_seconds = latency;
                final_results[family].binary_latency_str = latency_str;
                if (throughput_recorded) {
                    final_results[family].binary_result_val = gops;
                    final_results[family].has_binary_flow = true;
                }
                if (bandwidth_recorded) {
                    final_results[family].binary_bandwidth_val = bandwidth;
                    final_results[family].has_binary_flow = true;
                }
            } else if (final_results[family].t81_result_str.empty()) {
                final_results[family].t81_result_str = summary;
                final_results[family].t81_latency_seconds = latency;
                final_results[family].t81_latency_str = latency_str;
                final_results[family].has_t81_flow = throughput_recorded || bandwidth_recorded;
                final_results[family].t81_result_val = throughput_recorded ? gops : 0.0;
                if (bandwidth_recorded) final_results[family].t81_bandwidth_val = bandwidth;
            }
        }
    }
};

void GenerateMarkdownReport();

int main(int argc, char** argv) {
    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;

    CustomReporter reporter;
    ::benchmark::RunSpecifiedBenchmarks(&reporter);
    ::benchmark::Shutdown();

    GenerateMarkdownReport();
    return 0;
}

std::string get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm buf;
#ifdef _WIN32
    localtime_s(&buf, &in_time_t);
#else
    localtime_r(&in_time_t, &buf);
#endif
    std::stringstream ss;
    ss << std::put_time(&buf, "%Y-%m-%d %X UTC");
    return ss.str();
}

void GenerateMarkdownReport() {
    std::lock_guard<std::mutex> guard(final_results_mutex);
    std::cout << "\nGenerating benchmark report...\n";

    auto DisplayValue = [](const std::string& value) -> std::string {
        return value.empty() ? "not-applicable (no annotation)" : value;
    };
    auto EscapePipes = [](const std::string& value) -> std::string {
        std::string result;
        result.reserve(value.size());
        for (char c : value) {
            if (c == '|') {
                result += "\\|";
            } else {
                result += c;
            }
        }
        return result;
    };

    auto ResolveT81ClassicResultCell = [&](const BenchmarkResult& r) -> std::string {
        if (!r.t81_result_str.empty()) return r.t81_result_str;
        if (r.t81_classic_skipped) return "skipped (no counters emitted)";
        if (T81ClassicSemanticallyUnavoidable(r)) return "not-applicable (semantic)";
        const bool has_native_only = (r.has_t81_native_flow || !r.t81_native_result_str.empty() || !r.t81_native_note.empty()) &&
                                     !(r.has_t81_flow || !r.t81_classic_note.empty());
        if (has_native_only) return "not-applicable (native-only family)";
        return "not-implemented (missing baseline)";
    };

    auto ResolveT81ClassicLatencyCell = [&](const BenchmarkResult& r) -> std::string {
        if (!r.t81_latency_str.empty()) return r.t81_latency_str;
        if (r.t81_classic_skipped) return "skipped (no counters emitted)";
        if (T81ClassicSemanticallyUnavoidable(r)) return "not-applicable (semantic)";
        const bool has_native_only = (r.has_t81_native_flow || !r.t81_native_result_str.empty() || !r.t81_native_note.empty()) &&
                                     !(r.has_t81_flow || !r.t81_classic_note.empty());
        if (has_native_only) return "not-applicable (native-only family)";
        return "not-implemented (missing baseline)";
    };

    auto ResolveT81NativeResultCell = [&](const BenchmarkResult& r) -> std::string {
        if (!r.t81_native_result_str.empty()) return r.t81_native_result_str;
        if (r.t81_native_skipped) return "skipped (no counters emitted)";
        const bool has_native_variant = r.has_t81_native_flow || !r.t81_native_note.empty();
        if (!has_native_variant) return "not-applicable (no native variant)";
        return "not-implemented (missing baseline)";
    };

    auto ResolveT81NativeLatencyCell = [&](const BenchmarkResult& r) -> std::string {
        if (!r.t81_native_latency_str.empty()) return r.t81_native_latency_str;
        if (r.t81_native_skipped) return "skipped (no counters emitted)";
        const bool has_native_variant = r.has_t81_native_flow || !r.t81_native_note.empty();
        if (!has_native_variant) return "not-applicable (no native variant)";
        return "not-implemented (missing baseline)";
    };

    auto ResolveBinaryResultCell = [&](const BenchmarkResult& r) -> std::string {
        if (!r.binary_result_str.empty()) return r.binary_result_str;
        if (r.binary_skipped) return "skipped (no counters emitted)";
        if (BinaryBaselineSemanticallyUnavoidable(r)) {
            return "not-applicable (semantic: no binary baseline)";
        }
        return "not-implemented (missing baseline)";
    };

    auto ResolveBinaryLatencyCell = [&](const BenchmarkResult& r) -> std::string {
        if (!r.binary_latency_str.empty()) return r.binary_latency_str;
        if (r.binary_skipped) return "skipped (no counters emitted)";
        if (BinaryBaselineSemanticallyUnavoidable(r)) {
            return "not-applicable (semantic: no binary baseline)";
        }
        return "not-implemented (missing baseline)";
    };

    auto ResolveBandwidthCell = [&](const BenchmarkResult& r) -> std::string {
        if (!r.bandwidth_result_str.empty()) return r.bandwidth_result_str;
        return "not-applicable (non-bandwidth benchmark)";
    };

    auto ResolveThroughputRatioCell = [&](const BenchmarkResult& r) -> std::string {
        if (r.throughput_ratio_computed) return r.throughput_ratio_str;
        const bool t81_semantic_gap = T81ClassicSemanticallyUnavoidable(r) &&
                                      !(r.has_t81_flow || r.has_t81_native_flow);
        if (!r.has_binary_flow && BinaryBaselineSemanticallyUnavoidable(r)) {
            return "not-computable (semantic: no binary baseline)";
        }
        if (t81_semantic_gap) {
            return "not-computable (semantic)";
        }
        if (r.t81_classic_skipped || r.t81_native_skipped || r.binary_skipped) {
            return "skipped (no counters emitted)";
        }
        return "not-implemented (missing baseline)";
    };

    auto ResolveLatencySpeedupCell = [&](const BenchmarkResult& r) -> std::string {
        if (r.latency_speedup_computed) return r.latency_speedup_str;
        const bool t81_semantic_gap = T81ClassicSemanticallyUnavoidable(r) &&
                                      !(r.has_t81_flow || r.has_t81_native_flow);
        if (!r.has_binary_flow && BinaryBaselineSemanticallyUnavoidable(r)) {
            return "not-computable (semantic: no binary baseline)";
        }
        if (t81_semantic_gap) {
            return "not-computable (semantic)";
        }
        if (r.t81_classic_skipped || r.t81_native_skipped || r.binary_skipped) {
            return "skipped (no counters emitted)";
        }
        return "not-implemented (missing baseline)";
    };

    for (auto& [name, r] : final_results) {
        const bool has_any_t81_flow = r.has_t81_flow || r.has_t81_native_flow;
        const double t81_comparable_val = r.has_t81_native_flow ?
            r.t81_native_result_val : r.t81_result_val;
        const double t81_comparable_bw = r.has_t81_native_flow ?
            r.t81_native_bandwidth_val : r.t81_bandwidth_val;

        bool throughput_ratio_ready = has_any_t81_flow && r.has_binary_flow &&
                                      r.binary_result_val > 0.0 && t81_comparable_val > 0.0;
        double throughput_ratio = 0.0;
        if (throughput_ratio_ready) {
            throughput_ratio = t81_comparable_val / r.binary_result_val;
        } else if (has_any_t81_flow && r.has_binary_flow &&
                   r.binary_bandwidth_val > 0.0 && t81_comparable_bw > 0.0) {
            throughput_ratio = t81_comparable_bw / r.binary_bandwidth_val;
            throughput_ratio_ready = true;
        }

        if (throughput_ratio_ready) {
            r.throughput_ratio_val = throughput_ratio;
            std::ostringstream temp;
            temp << std::fixed << std::setprecision(2) << throughput_ratio << "x";
            r.throughput_ratio_str = temp.str();
            r.throughput_ratio_computed = true;
        } else {
            r.throughput_ratio_str.clear();
            r.throughput_ratio_computed = false;
        }

        const double t81_latency = (r.t81_native_latency_seconds > 0.0) ?
            r.t81_native_latency_seconds : r.t81_latency_seconds;
        if (t81_latency > 0.0 && r.binary_latency_seconds > 0.0) {
            r.latency_speedup_val = r.binary_latency_seconds / t81_latency;
            std::ostringstream temp;
            temp << std::fixed << std::setprecision(2) << r.latency_speedup_val << "x";
            r.latency_speedup_str = temp.str();
            r.latency_speedup_computed = true;
        } else {
            r.latency_speedup_str.clear();
            r.latency_speedup_computed = false;
        }
        r.analysis = BuildAnalysis(r);
    }

    std::cout << std::left << std::setw(25) << "Benchmark"
              << std::setw(20) << "T81 Result"
              << std::setw(16) << "T81 Latency"
              << std::setw(20) << "Binary Result"
              << std::setw(16) << "Binary Latency"
              << std::setw(18) << "Thru Ratio"
              << std::setw(18) << "Lat Speedup"
              << std::setw(25) << "T81 Advantage"
              << "Notes\n";
    std::cout << std::string(140, '-') << "\n";
    for (auto const& [name, r] : final_results) {
        const std::string t81_result_cell = ResolveT81ClassicResultCell(r);
        const std::string t81_latency_cell = ResolveT81ClassicLatencyCell(r);
        const std::string binary_result_cell = ResolveBinaryResultCell(r);
        const std::string binary_latency_cell = ResolveBinaryLatencyCell(r);
        const std::string throughput_ratio_cell = ResolveThroughputRatioCell(r);
        const std::string latency_speedup_cell = ResolveLatencySpeedupCell(r);
        const std::string advantage_display = BuildT81AdvantageDisplay(r);
        const std::string notes_display = BuildNotesDisplay(r);
        std::cout << std::left << std::setw(25) << r.name
                  << std::setw(20) << DisplayValue(t81_result_cell)
                  << std::setw(16) << DisplayValue(t81_latency_cell)
                  << std::setw(20) << DisplayValue(binary_result_cell)
                  << std::setw(16) << DisplayValue(binary_latency_cell)
                  << std::setw(18) << DisplayValue(throughput_ratio_cell)
                  << std::setw(18) << DisplayValue(latency_speedup_cell)
                  << std::setw(25) << DisplayValue(advantage_display)
                  << DisplayValue(notes_display) << "\n";
    }

    std::ofstream md_file("docs/benchmarks.md");
    if (!md_file.is_open()) {
        std::cerr << "Error: Could not open docs/benchmarks.md for writing.\n";
        return;
    }

    const auto git_branch = RunCommand("git rev-parse --abbrev-ref HEAD");
    const auto git_sha = RunCommand("git rev-parse --short HEAD");

    md_file << "# TCB-Core v0.1: Official T81 Foundation Core Benchmarks\n\n";
    md_file << "This document is auto-generated by the `benchmark_runner`.\n\n";
    md_file << "*Last Updated: " << get_current_timestamp() << "*  ";
    if (!git_branch.empty()) {
        md_file << "*Branch: " << git_branch << "*  ";
    }
    if (!git_sha.empty()) {
        md_file << "*Commit: " << git_sha << "*";
    }
    md_file << "\n\n";
    md_file << "## Summary\n\n";

    md_file << "| Benchmark               | T81 Result     | T81 Latency    | T81 Native Result | T81 Native Latency | Binary Result  | Binary Latency | Memory Bandwidth | Throughput Ratio (T81/Binary) | Latency Speedup (Binary/T81) | T81 Advantage                   | Notes                               |\n";
    md_file << "|-------------------------|----------------|----------------|------------------|--------------------|----------------|----------------|--------------------|-------------------------------|------------------------------|---------------------------------|-------------------------------------|\n";

    double best_t81_ratio = 1.0;
    double best_binary_ratio = 1.0;
    std::string best_name;
    std::string worst_name;
    int t81_wins = 0;
    int binary_wins = 0;
    int ties = 0;

    for (auto& [name, r] : final_results) {
        if (r.throughput_ratio_computed) {
            double ratio = r.throughput_ratio_val;
            if (ratio > 1.0 && ratio > best_t81_ratio) {
                best_t81_ratio = ratio;
                best_name = r.name;
            }
            if (ratio < 1.0 && ratio < best_binary_ratio) {
                best_binary_ratio = ratio;
                worst_name = r.name;
            }
            if (ratio > 1.05) {
                ++t81_wins;
            } else if (ratio < 0.95) {
                ++binary_wins;
            } else {
                ++ties;
            }
        }
        const std::string advantage_display = BuildT81AdvantageDisplay(r);
        const std::string notes_display = BuildNotesDisplay(r);
        const std::string t81_result_cell = ResolveT81ClassicResultCell(r);
        const std::string t81_latency_cell = ResolveT81ClassicLatencyCell(r);
        const std::string t81_native_result_cell = ResolveT81NativeResultCell(r);
        const std::string t81_native_latency_cell = ResolveT81NativeLatencyCell(r);
        const std::string binary_result_cell = ResolveBinaryResultCell(r);
        const std::string binary_latency_cell = ResolveBinaryLatencyCell(r);
        const std::string bandwidth_cell = ResolveBandwidthCell(r);
        const std::string throughput_ratio_cell = ResolveThroughputRatioCell(r);
        const std::string latency_speedup_cell = ResolveLatencySpeedupCell(r);
        const std::string advantage_display_md = EscapePipes(advantage_display);
        const std::string notes_display_md = EscapePipes(notes_display);
        md_file << "| " << std::left << std::setw(23) << r.name
                << "| " << std::setw(14) << DisplayValue(t81_result_cell)
                << "| " << std::setw(14) << DisplayValue(t81_latency_cell)
                << "| " << std::setw(14) << DisplayValue(t81_native_result_cell)
                << "| " << std::setw(14) << DisplayValue(t81_native_latency_cell)
                << "| " << std::setw(14) << DisplayValue(binary_result_cell)
                << "| " << std::setw(14) << DisplayValue(binary_latency_cell)
                << "| " << std::setw(14) << DisplayValue(bandwidth_cell)
                << "| " << std::setw(14) << DisplayValue(throughput_ratio_cell)
                << "| " << std::setw(14) << DisplayValue(latency_speedup_cell)
                << "| " << std::setw(31) << DisplayValue(advantage_display_md)
                << "| " << std::setw(35) << DisplayValue(notes_display_md) << "|\n";
    }

    md_file << "\n## Analysis\n\n";
    for (auto const& [name, r] : final_results) {
        md_file << "- `" << r.name << "`: ";
        if (r.analysis.empty()) {
            md_file << "no throughput summary available yet.\n";
        } else {
            md_file << r.analysis << '\n';
        }
    }

    md_file << "\n## Highlights\n\n";
    if (!best_name.empty()) {
        md_file << "- Largest T81 advantage: `" << best_name << "` (" << std::fixed << std::setprecision(2)
                << best_t81_ratio << "x) using Gops/s throughput.\n";
    }
    if (!worst_name.empty() && best_binary_ratio < 1.0) {
        md_file << "- Largest binary advantage: `" << worst_name << "` (" << std::fixed << std::setprecision(2)
                << best_binary_ratio << "x) reflects where deterministic handling lags.\n";
    }
    md_file << "- T81 wins: " << t81_wins << ", Binary wins: " << binary_wins << ", Comparable: " << ties << ".\n";

    md_file.close();
    std::cout << "Successfully wrote report to docs/benchmarks.md\n";
}
