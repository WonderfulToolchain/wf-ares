#include <ms/ms.hpp>

namespace higan::MasterSystem {

System system;
Scheduler scheduler;
Cheat cheat;
#include "serialization.cpp"

auto System::run() -> void {
  if(scheduler.enter() == Scheduler::Event::Frame) {
    cpu.pollPause();
    vdp.refresh();
  }
}

auto System::runToSave() -> void {
  scheduler.synchronize(cpu);
  scheduler.synchronize(vdp);
  scheduler.synchronize(psg);
}

auto System::load(Interface* interface, Model model) -> bool {
  this->interface = interface;
  information = {};
  information.model = model;

  auto document = BML::unserialize(interface->properties().serialize());

  if(MasterSystem::Model::ColecoVision()) {
    if(auto fp = platform->open(ID::System, "bios.rom", File::Read, File::Required)) {
      fp->read(bios, 0x2000);
    } else return false;
  }

  if(!cartridge.load()) return false;

  if(cartridge.region() == "NTSC") {
    information.region = Region::NTSC;
    information.colorburst = Constants::Colorburst::NTSC;
  }
  if(cartridge.region() == "PAL") {
    information.region = Region::PAL;
    information.colorburst = Constants::Colorburst::PAL * 4.0 / 5.0;
  }

  serializeInit();
  return information.loaded = true;
}

auto System::save() -> void {
  cartridge.save();
}

auto System::unload() -> void {
  if(!MasterSystem::Model::GameGear()) {
    cpu.peripherals.reset();
    controllerPort1.unload();
    controllerPort2.unload();
  }
  cartridge.unload();
}

auto System::power() -> void {
  video.reset(interface);
  video.setPalette();
  audio.reset(interface);

  scheduler.reset();
  cartridge.power();
  cpu.power();
  vdp.power();
  psg.power();
  scheduler.primary(cpu);

  if(!MasterSystem::Model::GameGear()) {
    controllerPort1.power(ID::Port::Controller1);
    controllerPort2.power(ID::Port::Controller2);

    controllerPort1.connect(option.port.controller1.device());
    controllerPort2.connect(option.port.controller2.device());
  }
}

}
