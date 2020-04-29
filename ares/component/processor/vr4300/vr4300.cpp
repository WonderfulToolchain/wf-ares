#include <ares/ares.hpp>
#include "vr4300.hpp"

namespace ares {

#include "registers.hpp"
#include "memory.cpp"
#include "instruction.cpp"
#include "instructions.cpp"
#include "serialization.cpp"
#include "disassembler.cpp"

auto VR4300::power() -> void {
  for(uint n : range(32)) GPR[n] = 0;
  LO = 0;
  HI = 0;
  PC = 0xbfc00000;
}

}
