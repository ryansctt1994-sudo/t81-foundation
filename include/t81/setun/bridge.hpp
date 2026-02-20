#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "t81/support/expected.hpp"
#include "t81/tisc/program.hpp"

namespace t81::setun {

enum class BridgeError {
  EmptyInput = 0,
  UnsupportedMnemonic,
  InvalidOperand,
  InvalidRegister,
  InvalidImmediate,
  InvalidLabel,
  DuplicateLabel,
  UndefinedLabel,
};

struct BridgeDiagnostic {
  // Explicitly defined default constructor - no NSDMI reliance
  BridgeDiagnostic()
      : error(BridgeError::EmptyInput), line(0), column(0), message(), source_line() {}

  BridgeDiagnostic(BridgeError err, std::size_t line_no, std::size_t column_no, std::string msg,
                   std::string source)
      : error(err),
        line(line_no),
        column(column_no),
        message(std::move(msg)),
        source_line(std::move(source)) {}

  // Explicit copy constructor
  BridgeDiagnostic(const BridgeDiagnostic& other)
      : error(other.error),
        line(other.line),
        column(other.column),
        message(other.message),
        source_line(other.source_line) {}

  // Explicit copy assignment
  BridgeDiagnostic& operator=(const BridgeDiagnostic& other) {
    if (this != &other) {
      error = other.error;
      line = other.line;
      column = other.column;
      message = other.message;
      source_line = other.source_line;
    }
    return *this;
  }

  // Explicit move constructor
  BridgeDiagnostic(BridgeDiagnostic&& other) noexcept
      : error(other.error),
        line(other.line),
        column(other.column),
        message(std::move(other.message)),
        source_line(std::move(other.source_line)) {}

  // Explicit move assignment
  BridgeDiagnostic& operator=(BridgeDiagnostic&& other) noexcept {
    if (this != &other) {
      error = other.error;
      line = other.line;
      column = other.column;
      message = std::move(other.message);
      source_line = std::move(other.source_line);
    }
    return *this;
  }

  // Member variables without NSDMI to force explicit init
  BridgeError error;
  std::size_t line;
  std::size_t column;
  std::string message;
  std::string source_line;
};

[[nodiscard]] std::string_view bridge_error_message(BridgeError error);

// Translate a single Setun-compatible source line into one TISC instruction.
// Supported mnemonics: NOP, HALT, LOADI, MOV, ADD, SUB, LOAD, STORE, JMP,
// JZ, JNZ, JN, JP.
[[nodiscard]] std::expected<t81::tisc::Insn, BridgeError> translate_line(std::string_view line);

// Translate a multi-line Setun-compatible program.
// Comments start with ';' or '#'.
[[nodiscard]] std::expected<t81::tisc::Program, BridgeError> translate_program(
    std::string_view source);

// Detailed program translation with deterministic source location on failure.
[[nodiscard]] std::expected<t81::tisc::Program, BridgeDiagnostic> translate_program_diagnostic(
    std::string_view source);

}  // namespace t81::setun
