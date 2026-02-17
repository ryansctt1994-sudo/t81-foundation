#ifndef T81_TISC_BINARY_EMITTER_HPP
#define T81_TISC_BINARY_EMITTER_HPP

#include <cstdint>
#include <vector>
#include "t81/tisc/ir.hpp"
#include "t81/tisc/program.hpp"

namespace t81 {
namespace tisc {

class BinaryEmitter {
public:
  Program emit(const ir::IntermediateProgram& program);
};

}  // namespace tisc
}  // namespace t81

#endif  // T81_TISC_BINARY_EMITTER_HPP
