#include <emulator/emulator.hpp>
#include "m68k.hpp"

namespace higan {

enum : uint { Byte, Word, Long };
enum : bool { Reverse = 1 };

#include "registers.cpp"
#include "memory.cpp"
#include "effective-address.cpp"
#include "traits.cpp"
#include "conditions.cpp"
#include "algorithms.cpp"
#include "instructions.cpp"
#include "disassembler.cpp"
#include "instruction.cpp"
#include "serialization.cpp"

auto M68K::power() -> void {
  for(auto& dr : r.d) dr = 0;
  for(auto& ar : r.a) ar = 0;
  r.sp = 0;
  r.pc = 0;

  r.c = 0;
  r.v = 0;
  r.z = 0;
  r.n = 0;
  r.x = 0;
  r.i = 7;
  r.s = 1;
  r.t = 0;

  r.irc = 0;
  r.ir  = 0;
//r.ird = 0;

  r.stop  = false;
  r.reset = false;
}

auto M68K::supervisor() -> bool {
  if(r.s) return true;

  r.pc -= 4;
  exception(Exception::Unprivileged, Vector::Unprivileged);
  return false;
}

auto M68K::exception(uint exception, uint vector, uint priority) -> void {
  idle(10);  //todo: not accurate

  auto pc = r.pc;
  auto sr = readSR();

  if(exception != Exception::Illegal) {
    if(!r.s) swap(r.a[7], r.sp);
    r.i = priority;
    r.s = 1;
    r.t = 0;
  }

  push<Long>(pc - 2);
  push<Word>(sr);

  r.pc = read<Long>(vector << 2);
  prefetch<Word>();
}

auto M68K::interrupt(uint vector, uint priority) -> void {
  return exception(Exception::Interrupt, vector, priority);
}

}
