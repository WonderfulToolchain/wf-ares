#include <n64/n64.hpp>

namespace ares::Nintendo64 {

CPU cpu;
#include "core/core.cpp"
#include "serialization.cpp"

auto CPU::load(Node::Object parent) -> void {
  node = parent->append<Node::Component>("CPU");
}

auto CPU::unload() -> void {
  node = {};
}

auto CPU::main() -> void {
  instruction();
  step(5);
}

auto CPU::step(uint clocks) -> void {
  rsp.clock -= clocks;
  rdp.clock -= clocks;
  while(rsp.clock < 0) rsp.main();
  while(rdp.clock < 0) rdp.main();
}

auto CPU::power() -> void {
  Thread::reset();
  powerR4300();

  //PIF ROM simulation:
  core.r[20].u64 = 0x0000'0001;
  core.r[22].u64 = 0x0000'003f;
  core.r[29].u64 = 0xa400'1ff0;
  core.pc        = 0xa400'0040;

  for(uint offset = 0; offset < 0x1000; offset += 4) {
    auto data = bus.readWord(0xb000'0000 + offset);
    bus.writeWord(0xa400'0000 + offset, data);
  }
}

}
