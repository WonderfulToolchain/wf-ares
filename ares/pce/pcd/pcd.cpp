#include <pce/pce.hpp>

namespace ares::PCEngine {

PCD pcd;
#include "io.cpp"
#include "serialization.cpp"

auto PCD::load(Node::Object parent) -> void {
  node = parent->append<Node::Component>("PC Engine CD");

  tray = node->append<Node::Port>("Disc Tray");
  tray->setFamily("PC Engine CD");
  tray->setType("Compact Disc");
  tray->setHotSwappable(true);
  tray->setAllocate([&] { return Node::Peripheral::create("PC Engine CD"); });
  tray->setAttach([&](auto node) { connect(node); });
  tray->setDetach([&](auto node) { disconnect(); });

  wram.allocate(64_KiB);
  bram.allocate(2_KiB);
}

auto PCD::save() -> void {
}

auto PCD::unload() -> void {
  wram.reset();
  bram.reset();
  disconnect();
  node = {};
  tray = {};
}

auto PCD::connect(Node::Peripheral with) -> void {
  disconnect();
  if(with) {
    disc = tray->append<Node::Peripheral>("PC Engine CD");
    disc->setManifest([&] { return information.manifest; });

    information = {};
    if(auto fp = platform->open(disc, "manifest.bml", File::Read, File::Required)) {
      information.manifest = fp->reads();
    }

    auto document = BML::unserialize(information.manifest);
    information.name = document["game/label"].string();

    fd = platform->open(disc, "cd.rom", File::Read, File::Required);
  }
}

auto PCD::disconnect() -> void {
  disc = {};
  fd = {};
}

auto PCD::main() -> void {
  step(1000);
}

auto PCD::step(uint clocks) -> void {
  Thread::step(clocks);
  Thread::synchronize(cpu);
}

auto PCD::power() -> void {
  Thread::create(1'000, {&PCD::main, this});
}

}
