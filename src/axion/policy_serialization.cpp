#include "t81/axion/policy.hpp"
#include <iostream>
#include <vector>

namespace t81::axion {

namespace {
void write_u8(std::ostream& os, uint8_t v) { os.put(static_cast<char>(v)); }
void write_u32(std::ostream& os, uint32_t v) {
    os.put(static_cast<char>(v & 0xFF));
    os.put(static_cast<char>((v >> 8) & 0xFF));
    os.put(static_cast<char>((v >> 16) & 0xFF));
    os.put(static_cast<char>((v >> 24) & 0xFF));
}
void write_u64(std::ostream& os, uint64_t v) {
    for (int i = 0; i < 8; ++i) {
        os.put(static_cast<char>((v >> (i * 8)) & 0xFF));
    }
}
void write_string(std::ostream& os, const std::string& s) {
    write_u32(os, static_cast<uint32_t>(s.size()));
    os.write(s.data(), static_cast<std::streamsize>(s.size()));
}

uint8_t read_u8(std::istream& is) { return static_cast<uint8_t>(is.get()); }
uint32_t read_u32(std::istream& is) {
    uint32_t v = 0;
    for (int i = 0; i < 4; ++i) {
        v |= (static_cast<uint32_t>(static_cast<uint8_t>(is.get())) << (i * 8));
    }
    return v;
}
uint64_t read_u64(std::istream& is) {
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) {
        v |= (static_cast<uint64_t>(static_cast<uint8_t>(is.get())) << (i * 8));
    }
    return v;
}
std::string read_string(std::istream& is) {
    uint32_t len = read_u32(is);
    std::string s(len, '\0');
    is.read(s.data(), static_cast<std::streamsize>(len));
    return s;
}
} // namespace

void Policy::serialize(std::ostream& os) const {
    write_u8(os, static_cast<uint8_t>(PolicyTag::Header));
    write_u32(os, 0x01); // Version

    write_u8(os, static_cast<uint8_t>(PolicyTag::Tier));
    write_u32(os, static_cast<uint32_t>(tier));

    if (max_stack) {
        write_u8(os, static_cast<uint8_t>(PolicyTag::MaxStack));
        write_u64(os, static_cast<uint64_t>(*max_stack));
    }
    if (max_instructions) {
        write_u8(os, static_cast<uint8_t>(PolicyTag::MaxInstructions));
        write_u64(os, static_cast<uint64_t>(*max_instructions));
    }
    if (max_recursion) {
        write_u8(os, static_cast<uint8_t>(PolicyTag::MaxRecursion));
        write_u64(os, static_cast<uint64_t>(*max_recursion));
    }
    if (max_reflections) {
        write_u8(os, static_cast<uint8_t>(PolicyTag::MaxReflections));
        write_u64(os, static_cast<uint64_t>(*max_reflections));
    }
    if (max_meta_writes) {
        write_u8(os, static_cast<uint8_t>(PolicyTag::MaxMetaWrites));
        write_u64(os, static_cast<uint64_t>(*max_meta_writes));
    }

    for (const auto& loop : loops) {
        write_u8(os, static_cast<uint8_t>(PolicyTag::LoopHint));
        write_u32(os, static_cast<uint32_t>(loop.id));
        write_string(os, loop.file);
        write_u32(os, static_cast<uint32_t>(loop.line));
        write_u32(os, static_cast<uint32_t>(loop.column));
        write_u8(os, loop.annotated ? 1 : 0);
        write_u32(os, static_cast<uint32_t>(loop.depth));
        write_u8(os, loop.bound_infinite ? 1 : 0);
        if (!loop.bound_infinite && loop.bound_value) {
            write_u64(os, static_cast<uint64_t>(*loop.bound_value));
        } else {
            write_u64(os, 0);
        }
    }

    for (const auto& mg : match_guards) {
        write_u8(os, static_cast<uint8_t>(PolicyTag::MatchGuard));
        write_string(os, mg.enum_name);
        write_string(os, mg.variant_name);
        write_u8(os, mg.payload ? 1 : 0);
        if (mg.payload) write_string(os, *mg.payload);
        write_string(os, mg.result);
    }

    for (const auto& sr : segment_requirements) {
        write_u8(os, static_cast<uint8_t>(PolicyTag::SegmentEvent));
        write_string(os, sr.segment);
        write_string(os, sr.action);
        write_u8(os, sr.addr ? 1 : 0);
        if (sr.addr) write_u64(os, static_cast<uint64_t>(*sr.addr));
    }

    for (const auto& ar : axion_event_requirements) {
        write_u8(os, static_cast<uint8_t>(PolicyTag::AxionEvent));
        write_string(os, ar.reason);
    }

    for (const auto& al : alignment_requirements) {
        write_u8(os, static_cast<uint8_t>(PolicyTag::Alignment));
        write_string(os, al.reason);
    }

    if (!bytecode.empty()) {
        write_u8(os, static_cast<uint8_t>(PolicyTag::BytecodeHeader));
        write_u32(os, static_cast<uint32_t>(symbol_table.size()));
        for (const auto& sym : symbol_table) {
            write_string(os, sym);
        }
        write_u32(os, static_cast<uint32_t>(bytecode.size()));
        os.write(reinterpret_cast<const char*>(bytecode.data()), static_cast<std::streamsize>(bytecode.size()));
    }

    write_u8(os, static_cast<uint8_t>(PolicyTag::End));
}

void Policy::compile_to_bytecode() {
    bytecode.clear();
    symbol_table.clear();
    auto add_sym = [&](const std::string& s) -> uint32_t {
        for (size_t i = 0; i < symbol_table.size(); ++i) {
            if (symbol_table[i] == s) return static_cast<uint32_t>(i);
        }
        symbol_table.push_back(s);
        return static_cast<uint32_t>(symbol_table.size() - 1);
    };
    auto emit_u8 = [&](uint8_t v) { bytecode.push_back(v); };
    auto emit_u32 = [&](uint32_t v) {
        emit_u8(static_cast<uint8_t>(v & 0xFF));
        emit_u8(static_cast<uint8_t>((v >> 8) & 0xFF));
        emit_u8(static_cast<uint8_t>((v >> 16) & 0xFF));
        emit_u8(static_cast<uint8_t>((v >> 24) & 0xFF));
    };
    auto emit_u64 = [&](uint64_t v) {
        for (int i = 0; i < 8; ++i) emit_u8(static_cast<uint8_t>((v >> (i * 8)) & 0xFF));
    };

    emit_u8(static_cast<uint8_t>(AxionOp::CheckTier));
    emit_u32(static_cast<uint32_t>(tier));

    if (max_instructions) {
        emit_u8(static_cast<uint8_t>(AxionOp::LimitInstructions));
        emit_u64(static_cast<uint64_t>(*max_instructions));
    }
    if (max_stack) {
        emit_u8(static_cast<uint8_t>(AxionOp::LimitStack));
        emit_u64(static_cast<uint64_t>(*max_stack));
    }
    if (max_recursion) {
        emit_u8(static_cast<uint8_t>(AxionOp::LimitRecursion));
        emit_u64(static_cast<uint64_t>(*max_recursion));
    }
    if (max_reflections) {
        emit_u8(static_cast<uint8_t>(AxionOp::LimitReflections));
        emit_u64(static_cast<uint64_t>(*max_reflections));
    }
    if (max_meta_writes) {
        emit_u8(static_cast<uint8_t>(AxionOp::LimitMetaWrites));
        emit_u64(static_cast<uint64_t>(*max_meta_writes));
    }

    for (const auto& loop : loops) {
        emit_u8(static_cast<uint8_t>(AxionOp::RequireLoop));
        emit_u32(static_cast<uint32_t>(loop.id));
        emit_u32(add_sym(loop.file));
        emit_u32(static_cast<uint32_t>(loop.line));
        emit_u32(static_cast<uint32_t>(loop.column));
        emit_u8(loop.bound_infinite ? 1 : 0);
        emit_u64(loop.bound_value.value_or(0));
    }

    for (const auto& mg : match_guards) {
        emit_u8(static_cast<uint8_t>(AxionOp::RequireMatchGuard));
        emit_u32(add_sym(mg.enum_name));
        emit_u32(add_sym(mg.variant_name));
        emit_u32(mg.payload ? add_sym(*mg.payload) : 0xFFFFFFFF);
        emit_u32(add_sym(mg.result));
    }

    for (const auto& sr : segment_requirements) {
        emit_u8(static_cast<uint8_t>(AxionOp::RequireSegmentEvent));
        emit_u32(add_sym(sr.segment));
        emit_u32(add_sym(sr.action));
        emit_u8(sr.addr ? 1 : 0);
        emit_u64(sr.addr.value_or(0));
    }

    for (const auto& ar : axion_event_requirements) {
        emit_u8(static_cast<uint8_t>(AxionOp::RequireAxionEvent));
        emit_u32(add_sym(ar.reason));
    }

    for (const auto& al : alignment_requirements) {
        emit_u8(static_cast<uint8_t>(AxionOp::RequireAlignment));
        emit_u32(add_sym(al.reason));
    }

    emit_u8(static_cast<uint8_t>(AxionOp::Ret));
}

t81::expected<Policy, std::string> Policy::deserialize(std::istream& is) {
    Policy policy;
    while (is.peek() != EOF) {
        int tag_int = is.get();
        if (tag_int == EOF) break;
        PolicyTag tag = static_cast<PolicyTag>(tag_int);
        if (tag == PolicyTag::End) break;

        switch (tag) {
            case PolicyTag::Header:
                read_u32(is); // Version
                break;
            case PolicyTag::Tier:
                policy.tier = static_cast<int>(read_u32(is));
                break;
            case PolicyTag::MaxStack:
                policy.max_stack = static_cast<int64_t>(read_u64(is));
                break;
            case PolicyTag::MaxInstructions:
                policy.max_instructions = static_cast<int64_t>(read_u64(is));
                break;
            case PolicyTag::MaxRecursion:
                policy.max_recursion = static_cast<int64_t>(read_u64(is));
                break;
            case PolicyTag::MaxReflections:
                policy.max_reflections = static_cast<int64_t>(read_u64(is));
                break;
            case PolicyTag::MaxMetaWrites:
                policy.max_meta_writes = static_cast<int64_t>(read_u64(is));
                break;
            case PolicyTag::LoopHint: {
                LoopHint hint;
                hint.id = static_cast<int>(read_u32(is));
                hint.file = read_string(is);
                hint.line = static_cast<int>(read_u32(is));
                hint.column = static_cast<int>(read_u32(is));
                hint.annotated = read_u8(is) != 0;
                hint.depth = static_cast<int>(read_u32(is));
                hint.bound_infinite = read_u8(is) != 0;
                uint64_t val = read_u64(is);
                if (!hint.bound_infinite) hint.bound_value = static_cast<int64_t>(val);
                policy.loops.push_back(std::move(hint));
                break;
            }
            case PolicyTag::MatchGuard: {
                MatchGuardRequirement mg;
                mg.enum_name = read_string(is);
                mg.variant_name = read_string(is);
                if (read_u8(is)) mg.payload = read_string(is);
                mg.result = read_string(is);
                policy.match_guards.push_back(std::move(mg));
                break;
            }
            case PolicyTag::SegmentEvent: {
                SegmentEventRequirement sr;
                sr.segment = read_string(is);
                sr.action = read_string(is);
                if (read_u8(is)) sr.addr = static_cast<int64_t>(read_u64(is));
                policy.segment_requirements.push_back(std::move(sr));
                break;
            }
            case PolicyTag::AxionEvent: {
                AxionEventRequirement ar;
                ar.reason = read_string(is);
                policy.axion_event_requirements.push_back(std::move(ar));
                break;
            }
            case PolicyTag::Alignment: {
                AlignmentRequirement al;
                al.reason = read_string(is);
                policy.alignment_requirements.push_back(std::move(al));
                break;
            }
            case PolicyTag::BytecodeHeader: {
                uint32_t sym_count = read_u32(is);
                policy.symbol_table.resize(sym_count);
                for (uint32_t i = 0; i < sym_count; ++i) {
                    policy.symbol_table[i] = read_string(is);
                }
                uint32_t code_size = read_u32(is);
                policy.bytecode.resize(code_size);
                is.read(reinterpret_cast<char*>(policy.bytecode.data()), static_cast<std::streamsize>(code_size));
                break;
            }
            default:
                return t81::expected<Policy, std::string>("Unknown policy tag");
        }
    }
    return policy;
}

} // namespace t81::axion
