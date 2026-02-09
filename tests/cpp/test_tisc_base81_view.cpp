#include <iostream>
#include <vector>
#include <cstdlib>
#include "t81/tisc/base81_view.hpp"

using namespace t81::tisc;

#define EXPECT(cond) \
    if (!(cond)) { \
        std::cerr << "Assertion failed: " << #cond << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        std::exit(1); \
    }

void test_view_roundtrip() {
    std::cout << "Testing TISC Base-81 view roundtrip..." << std::endl;
    Program p;
    Insn i1;
    i1.opcode = Opcode::Add;
    i1.a = 1; i1.b = 2; i1.c = 3;
    p.insns.push_back(i1);

    Insn i2;
    i2.opcode = Opcode::LoadImm;
    i2.a = 10; i2.b = 1000; i2.c = -5;
    p.insns.push_back(i2);

    std::string rendered = base81_view::render(p);
    std::cout << "Rendered: " << rendered << std::endl;

    auto parsed_res = base81_view::parse(rendered);
    EXPECT(parsed_res.is_ok());
    const auto& parsed = parsed_res.value();

    EXPECT(parsed.insns.size() == 2);
    EXPECT(parsed.insns[0].opcode == i1.opcode);
    EXPECT(parsed.insns[0].a == i1.a);
    EXPECT(parsed.insns[1].c == i2.c);
    std::cout << "OK" << std::endl;
}

int main() {
    test_view_roundtrip();
    std::cout << "All TISC Base-81 view tests passed!" << std::endl;
    return 0;
}
