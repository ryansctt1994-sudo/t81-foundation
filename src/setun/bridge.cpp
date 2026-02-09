#include "t81/setun/bridge.hpp"

#include <algorithm>
#include <charconv>
#include <cctype>
#include <string>
#include <vector>

namespace t81::setun {
namespace {

std::string trim(std::string_view in) {
  const auto first = std::find_if_not(in.begin(), in.end(), [](unsigned char c) {
    return std::isspace(c) != 0;
  });
  if (first == in.end()) return {};
  const auto last = std::find_if_not(in.rbegin(), in.rend(), [](unsigned char c) {
    return std::isspace(c) != 0;
  }).base();
  return std::string(first, last);
}

std::string uppercase(std::string token) {
  std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c) {
    return static_cast<char>(std::toupper(c));
  });
  return token;
}

std::vector<std::string> tokenize(std::string line) {
  for (char& ch : line) {
    if (ch == ',') ch = ' ';
  }

  std::vector<std::string> out;
  std::string current;
  for (char ch : line) {
    if (std::isspace(static_cast<unsigned char>(ch)) != 0) {
      if (!current.empty()) {
        out.push_back(std::move(current));
        current.clear();
      }
      continue;
    }
    current.push_back(ch);
  }
  if (!current.empty()) out.push_back(std::move(current));
  return out;
}

std::expected<int, BridgeError> parse_register(std::string_view token) {
  if (token.size() < 2) return {std::unexpect, BridgeError::InvalidRegister};
  if (token.front() != 'R' && token.front() != 'r') {
    return {std::unexpect, BridgeError::InvalidRegister};
  }
  int index = -1;
  const char* begin = token.data() + 1;
  const char* end = token.data() + token.size();
  auto [ptr, ec] = std::from_chars(begin, end, index);
  if (ec != std::errc{} || ptr != end) {
    return {std::unexpect, BridgeError::InvalidRegister};
  }
  if (index < 0 || index > 242) {
    return {std::unexpect, BridgeError::InvalidRegister};
  }
  return index;
}

std::expected<std::int64_t, BridgeError> parse_immediate(std::string_view token) {
  std::int64_t value = 0;
  const char* begin = token.data();
  const char* end = token.data() + token.size();
  auto [ptr, ec] = std::from_chars(begin, end, value);
  if (ec != std::errc{} || ptr != end) {
    return {std::unexpect, BridgeError::InvalidImmediate};
  }
  return value;
}

}  // namespace

std::string_view bridge_error_message(BridgeError error) {
  switch (error) {
    case BridgeError::EmptyInput: return "empty input";
    case BridgeError::UnsupportedMnemonic: return "unsupported mnemonic";
    case BridgeError::InvalidOperand: return "invalid operand";
    case BridgeError::InvalidRegister: return "invalid register";
    case BridgeError::InvalidImmediate: return "invalid immediate";
  }
  return "unknown bridge error";
}

std::expected<t81::tisc::Insn, BridgeError> translate_line(std::string_view raw_line) {
  auto stripped = std::string(raw_line);
  const auto semicolon = stripped.find(';');
  if (semicolon != std::string::npos) stripped.resize(semicolon);
  const auto hash = stripped.find('#');
  if (hash != std::string::npos) stripped.resize(hash);

  auto line = trim(stripped);
  if (line.empty()) return {std::unexpect, BridgeError::EmptyInput};

  auto tokens = tokenize(line);
  if (tokens.empty()) return {std::unexpect, BridgeError::EmptyInput};
  tokens[0] = uppercase(std::move(tokens[0]));

  t81::tisc::Insn insn{};
  if (tokens[0] == "NOP") {
    if (tokens.size() != 1) return {std::unexpect, BridgeError::InvalidOperand};
    insn.opcode = t81::tisc::Opcode::Nop;
    return insn;
  }
  if (tokens[0] == "HALT") {
    if (tokens.size() != 1) return {std::unexpect, BridgeError::InvalidOperand};
    insn.opcode = t81::tisc::Opcode::Halt;
    return insn;
  }
  if (tokens[0] == "LOADI") {
    if (tokens.size() != 3) return {std::unexpect, BridgeError::InvalidOperand};
    auto dst = parse_register(tokens[1]);
    if (!dst.has_value()) return {std::unexpect, dst.error()};
    auto imm = parse_immediate(tokens[2]);
    if (!imm.has_value()) return {std::unexpect, imm.error()};
    insn.opcode = t81::tisc::Opcode::LoadImm;
    insn.a = dst.value();
    insn.b = imm.value();
    return insn;
  }
  if (tokens[0] == "MOV") {
    if (tokens.size() != 3) return {std::unexpect, BridgeError::InvalidOperand};
    auto dst = parse_register(tokens[1]);
    if (!dst.has_value()) return {std::unexpect, dst.error()};
    auto src = parse_register(tokens[2]);
    if (!src.has_value()) return {std::unexpect, src.error()};
    insn.opcode = t81::tisc::Opcode::Mov;
    insn.a = dst.value();
    insn.b = src.value();
    return insn;
  }
  if (tokens[0] == "ADD" || tokens[0] == "SUB") {
    if (tokens.size() != 3) return {std::unexpect, BridgeError::InvalidOperand};
    auto dst = parse_register(tokens[1]);
    if (!dst.has_value()) return {std::unexpect, dst.error()};
    auto src = parse_register(tokens[2]);
    if (!src.has_value()) return {std::unexpect, src.error()};
    insn.opcode = (tokens[0] == "ADD") ? t81::tisc::Opcode::Add : t81::tisc::Opcode::Sub;
    insn.a = dst.value();
    insn.b = dst.value();
    insn.c = src.value();
    return insn;
  }
  if (tokens[0] == "JMP") {
    if (tokens.size() != 2) return {std::unexpect, BridgeError::InvalidOperand};
    auto target = parse_immediate(tokens[1]);
    if (!target.has_value()) return {std::unexpect, target.error()};
    insn.opcode = t81::tisc::Opcode::Jump;
    insn.a = static_cast<std::int32_t>(target.value());
    return insn;
  }

  return {std::unexpect, BridgeError::UnsupportedMnemonic};
}

std::expected<t81::tisc::Program, BridgeError> translate_program(std::string_view source) {
  t81::tisc::Program program{};
  std::size_t start = 0;
  while (start <= source.size()) {
    const auto end = source.find('\n', start);
    auto line = source.substr(start, (end == std::string_view::npos) ? source.size() - start : end - start);
    auto insn = translate_line(line);
    if (insn.has_value()) {
      program.insns.push_back(insn.value());
    } else if (insn.error() != BridgeError::EmptyInput) {
      return {std::unexpect, insn.error()};
    }
    if (end == std::string_view::npos) break;
    start = end + 1;
  }
  return program;
}

}  // namespace t81::setun
