#include <md/md.hpp>

namespace higan::MegaDrive {

APU apu;
#include "bus.cpp"
#include "serialization.cpp"

auto APU::main() -> void {
  if(!state.enabled) {
    return step(1);
  }

  if(state.nmiLine) {
    state.nmiLine = 0;  //edge-sensitive
    irq(0, 0x0066, 0xff);
  }

  if(state.intLine) {
    //level-sensitive
    irq(1, 0x0038, 0xff);
  }

  instruction();
}

auto APU::step(uint clocks) -> void {
  Thread::step(clocks);
  scheduler.synchronize();
}

auto APU::synchronizing() const -> bool {
  return scheduler.synchronizing();
}

auto APU::setNMI(bool value) -> void {
  state.nmiLine = value;
}

auto APU::setINT(bool value) -> void {
  state.intLine = value;
}

auto APU::enable(bool value) -> void {
  //68K cannot disable the Z80 without bus access
  if(!bus->granted() && !value) return;
  if(state.enabled && !value) reset();
  state.enabled = value;
}

auto APU::power(bool reset) -> void {
  Z80::bus = this;
  Z80::power();
  bus->grant(false);
  Thread::create(system.frequency() / 15.0, [&] {
    while(true) scheduler.resume(), main();
  });
  if(!reset) ram.allocate(8_KiB);
  state = {};
}

auto APU::reset() -> void {
  Z80::power();
  bus->grant(false);
  Thread::create(system.frequency() / 15.0, [&] {
    while(true) scheduler.resume(), main();
  });
  state = {};
}

}
