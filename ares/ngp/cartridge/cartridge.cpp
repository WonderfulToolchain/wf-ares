#include <ngp/ngp.hpp>

namespace ares::NeoGeoPocket {

Cartridge& cartridge = cartridgeSlot.cartridge;
#include "slot.cpp"
#include "flash.cpp"
#include "serialization.cpp"

auto Cartridge::allocate(Node::Port parent) -> Node::Peripheral {
  return node = parent->append<Node::Peripheral>(system.name());
}

auto Cartridge::connect() -> void {
  if(!node->setPak(pak = platform->pak(node))) return;

  information = {};
  if(auto fp = pak->read("manifest.bml")) {
    information.manifest = fp->reads();
  }
  auto document = BML::unserialize(information.manifest);
  information.name = document["game/label"].string();

  flash[0].reset(0);
  flash[1].reset(1);

  if(auto memory = document["game/board/memory(type=Flash,content=Program)"]) {
    auto size = memory["size"].natural();
    flash[0].allocate(min(16_Mibit, size));
    flash[1].allocate(size >= 16_Mibit ? size - 16_Mibit : 0);
    if(auto fp = pak->read("program.flash")) {
      flash[0].load(fp);
      flash[1].load(fp);
    };
  }

  power();
}

auto Cartridge::disconnect() -> void {
  if(!node) return;
  flash[0].reset(0);
  flash[1].reset(1);
  pak.reset();
  node.reset();
}

auto Cartridge::save() -> void {
  if(!node) return;
  auto document = BML::unserialize(information.manifest);

  if(auto memory = document["game/board/memory(type=Flash,content=Program)"]) {
    if(flash[0].modified || flash[1].modified) {
      if(auto fp = pak->write("program.flash")) {
        flash[0].save(fp);
        flash[1].save(fp);
      }
    }
  }
}

auto Cartridge::power() -> void {
  flash[0].power();
  flash[1].power();
}

auto Cartridge::read(n1 chip, n21 address) -> n8 {
  if(!flash[chip]) return 0x00;
  return flash[chip].read(address);
}

auto Cartridge::write(n1 chip, n21 address, n8 data) -> void {
  if(!flash[chip]) return;
  return flash[chip].write(address, data);
}

}
