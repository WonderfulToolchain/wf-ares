#include <md/md.hpp>

namespace ares::MegaDrive {

static uint16 Unmapped = 0;

MCD mcd;
#include "bus.cpp"
#include "bus-external.cpp"
#include "io.cpp"
#include "io-external.cpp"
#include "irq.cpp"
#include "cdc.cpp"
#include "cdc-transfer.cpp"
#include "cdd.cpp"
#include "cdd-dac.cpp"
#include "timer.cpp"
#include "gpu.cpp"
#include "pcm.cpp"
#include "serialization.cpp"

auto MCD::load(Node::Object parent, Node::Object from) -> void {
  node = Node::append<Node::Component>(parent, from, "Mega CD");
  from = Node::scan(parent = node, from);

  debugPRAM = Node::append<Node::Memory>(parent, from, "Mega CD PRAM");
  debugPRAM->setSize(512_KiB);
  debugPRAM->setRead([&](uint32 address) -> uint8 {
    return pram.read(address >> 1).byte(!address.bit(0));
  });
  debugPRAM->setWrite([&](uint32 address, uint8 data) -> void {
    auto value = pram.read(address >> 1);
    value.byte(!address.bit(0)) = data;
    return pram.write(address >> 1, value);
  });

  debugWRAM = Node::append<Node::Memory>(parent, from, "Mega CD WRAM");
  debugWRAM->setSize(256_KiB);
  debugWRAM->setRead([&](uint32 address) -> uint8 {
    return wram.read(address >> 1).byte(!address.bit(0));
  });
  debugWRAM->setWrite([&](uint32 address, uint8 data) -> void {
    auto value = wram.read(address >> 1);
    value.byte(!address.bit(0)) = data;
    return wram.write(address >> 1, value);
  });

  debugBRAM = Node::append<Node::Memory>(parent, from, "Mega CD BRAM");
  debugBRAM->setSize(8_KiB);
  debugBRAM->setRead([&](uint32 address) -> uint8 {
    return bram.read(address);
  });
  debugBRAM->setWrite([&](uint32 address, uint8 data) -> void {
    return bram.write(address, data);
  });

  debugInstruction = Node::append<Node::Instruction>(parent, from, "Instruction", "MCD");
  debugInstruction->setAddressBits(24);

  debugInterrupt = Node::append<Node::Notification>(parent, from, "Interrupt", "MCD");

  tray = Node::append<Node::Port>(parent, from, "Disc Tray");
  tray->setFamily("Mega CD");
  tray->setType("Compact Disc");
  tray->setHotSwappable(true);
  tray->setAllocate([&] { return Node::Peripheral::create("Mega CD"); });
  tray->setAttach([&](auto node) { connect(node); });
  tray->setDetach([&](auto node) { disconnect(); });
  tray->scan(from);

  bios.allocate   (128_KiB >> 1);
  pram.allocate   (512_KiB >> 1);
  wram.allocate   (256_KiB >> 1);
  bram.allocate   (  8_KiB >> 0);
  cdc.ram.allocate( 16_KiB >> 1);

  if(expansion.node) {
    if(auto fp = platform->open(expansion.node, "backup.ram", File::Read)) {
      bram.load(fp);
    }
  }

  cdd.load(parent, from);
  pcm.load(parent, from);
}

auto MCD::unload() -> void {
  disconnect();

  if(expansion.node) {
    if(auto fp = platform->open(expansion.node, "backup.ram", File::Write)) {
      bram.save(fp);
    }
  }

  cdd.unload();
  pcm.unload();

  bios.reset();
  pram.reset();
  wram.reset();
  bram.reset();
  cdc.ram.reset();

  node = {};
  debugPRAM = {};
  debugWRAM = {};
  debugBRAM = {};
  debugInstruction = {};
  debugInterrupt = {};
  tray = {};
}

auto MCD::connect(Node::Peripheral with) -> void {
  disconnect();
  if(with) {
    disc = Node::append<Node::Peripheral>(tray, with, "Mega CD");
    disc->setManifest([&] { return information.manifest; });

    information = {};
    if(auto fp = platform->open(disc, "manifest.bml", File::Read, File::Required)) {
      information.manifest = fp->reads();
    }

    auto document = BML::unserialize(information.manifest);
    information.name = document["game/label"].text();

    fd = platform->open(disc, "cd.rom", File::Read, File::Required);
    cdd.insert();
  }
}

auto MCD::disconnect() -> void {
  if(!disc) return;
  cdd.eject();
  disc = {};
  fd = {};
  information = {};
}

auto MCD::main() -> void {
  if(io.halt) return wait(16);

  if(irq.pending) {
    if(1 > r.i && gpu.irq.lower()) {
      if(debugInterrupt->enabled()) debugInterrupt->notify("GPU");
      return interrupt(Vector::Level1, 1);
    }
    if(2 > r.i && external.irq.lower()) {
      if(debugInterrupt->enabled()) debugInterrupt->notify("External");
      return interrupt(Vector::Level2, 2);
    }
    if(3 > r.i && timer.irq.lower()) {
      if(debugInterrupt->enabled()) debugInterrupt->notify("Timer");
      return interrupt(Vector::Level3, 3);
    }
    if(4 > r.i && cdd.irq.lower()) {
      if(debugInterrupt->enabled()) debugInterrupt->notify("CDD");
      return interrupt(Vector::Level4, 4);
    }
    if(5 > r.i && cdc.irq.lower()) {
      if(debugInterrupt->enabled()) debugInterrupt->notify("CDC");
      return interrupt(Vector::Level5, 5);
    }
    if(6 > r.i && irq.subcode.lower()) {
      if(debugInterrupt->enabled()) debugInterrupt->notify("IRQ");
      return interrupt(Vector::Level6, 6);
    }
    if(irq.reset.lower()) {
      if(debugInterrupt->enabled()) debugInterrupt->notify("Reset");
      r.a[7] = read(1, 1, 0) << 16 | read(1, 1, 2) << 0;
      r.pc   = read(1, 1, 4) << 16 | read(1, 1, 6) << 0;
      prefetch();
      prefetch();
      return;
    }
  }

  if(debugInstruction->enabled() && debugInstruction->address(r.pc - 4)) {
    debugInstruction->notify(disassembleInstruction(r.pc - 4), disassembleContext());
  }
  instruction();
}

auto MCD::step(uint clocks) -> void {
  gpu.step(clocks);
  counter.divider += clocks;
  while(counter.divider >= 384) {
    counter.divider -= 384;
    cdc.clock();
    cdd.clock();
    timer.clock();
    pcm.clock();
  }
  counter.dma += clocks;
  while(counter.dma >= 6) {
    counter.dma -= 6;
    cdc.transfer.dma();
  }
  counter.pcm += clocks;
  while(counter.pcm >= frequency() / 44100.0) {
    counter.pcm -= frequency() / 44100.0;
    cdd.sample();
  }

  Thread::step(clocks);
}

auto MCD::idle(uint clocks) -> void {
  step(clocks);
}

auto MCD::wait(uint clocks) -> void {
  step(clocks);
  Thread::synchronize(cpu);
}

auto MCD::power(bool reset) -> void {
  if(auto fp = platform->open(expansion.node, "program.rom", File::Read, File::Required)) {
    for(uint address : range(bios.size())) bios.program(address, fp->readm(2));
  }

  M68K::power();
  Thread::create(12'500'000, {&MCD::main, this});
  counter = {};
  if(!reset) {
    io = {};
    led = {};
    irq = {};
    external = {};
    communication = {};
    cdc.power(reset);
    cdd.power(reset);
    timer.power(reset);
    gpu.power(reset);
    pcm.power(reset);
  }
  irq.reset.enable = 1;
  irq.reset.raise();
  bios.program(0x72 >> 1, 0xffff);
}

}
