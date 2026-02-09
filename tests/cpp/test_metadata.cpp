#include <iostream>
#include <vector>
#include <cstdlib>
#include "t81/codec/metadata.hpp"

using namespace t81::codec::metadata;

#define EXPECT(cond) \
    if (!(cond)) { \
        std::cerr << "Assertion failed: " << #cond << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        std::exit(1); \
    }

void test_metadata_roundtrip() {
    std::cout << "Testing metadata roundtrip..." << std::endl;
    std::vector<uint8_t> payload = {0xDE, 0xAD, 0xBE, 0xEF};
    uint64_t trit_count = 12345;
    auto wrapped = wrap_encoded_buffer(EncodingType::B81_DIGITS, trit_count, payload);

    EXPECT(wrapped.size() == 16 + payload.size());

    auto unwrapped_res = unwrap_encoded_buffer(wrapped);
    EXPECT(unwrapped_res.is_ok());
    auto [header, extracted_payload] = unwrapped_res.value();

    EXPECT(header.magic == 0x43313854);
    EXPECT(header.version == 1);
    EXPECT(header.encoding == EncodingType::B81_DIGITS);
    EXPECT(header.trit_count == trit_count);
    EXPECT(extracted_payload == payload);
    std::cout << "OK" << std::endl;
}

int main() {
    test_metadata_roundtrip();
    std::cout << "All metadata tests passed!" << std::endl;
    return 0;
}
