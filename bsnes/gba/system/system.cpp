#include <gba/gba.hpp>

namespace GBA {

#include "bios.cpp"
#include "serialization.cpp"
BIOS bios;
System system;

void System::init() {
}

void System::term() {
}

void System::power() {
  bus.power();
  cpu.power();
  ppu.power();
  apu.power();
  cartridge.power();
  scheduler.power();
}

void System::load() {
  serialize_init();
}

void System::run() {
  scheduler.enter();
}

void System::runtosave() {
  scheduler.sync = Scheduler::SynchronizeMode::CPU;
  runthreadtosave();

  scheduler.sync = Scheduler::SynchronizeMode::All;
  scheduler.active = ppu.thread;
  runthreadtosave();

  scheduler.sync = Scheduler::SynchronizeMode::All;
  scheduler.active = apu.thread;
  runthreadtosave();

  scheduler.sync = Scheduler::SynchronizeMode::None;
}

void System::runthreadtosave() {
  while(true) {
    scheduler.enter();
    if(scheduler.exit_reason() == Scheduler::ExitReason::SynchronizeEvent) break;
    if(scheduler.exit_reason() == Scheduler::ExitReason::FrameEvent);
  }
}

}
