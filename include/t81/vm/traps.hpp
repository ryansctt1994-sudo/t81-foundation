#pragma once

namespace t81::vm {
enum class Trap {
  None = 0,
  DecodeFault,
  TypeFault,
  BoundsFault,
  StackFault,
  DivisionFault,
  SecurityFault,
  ShapeFault,
  TrapInstruction,
};
}  // namespace t81::vm
