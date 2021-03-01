#include <cv/cv.hpp>

namespace ares::ColecoVision {

Cartridge& cartridge = cartridgeSlot.cartridge;
#include "slot.cpp"
#include "serialization.cpp"

auto Cartridge::allocate(Node::Port parent) -> Node::Peripheral {
  return node = parent->append<Node::Peripheral>(system.name());
}

auto Cartridge::connect() -> void {
  if(!node->setPak(pak = platform->pak(node))) return;

  information = {};
  information.name = pak->attribute("title");
  information.region = pak->attribute("region");

  if(auto fp = pak->read("program.rom")) {
    rom.allocate(fp->size());
    rom.load(fp);
  }

  power();
}

auto Cartridge::disconnect() -> void {
  if(!node) return;
  rom.reset();
  pak.reset();
  node.reset();
}

auto Cartridge::save() -> void {
}

auto Cartridge::power() -> void {
}

}
