#include <md/md.hpp>

namespace ares::MegaDrive {

Cartridge& cartridge = cartridgeSlot.cartridge;
#include "board/board.cpp"
#include "slot.cpp"
#include "serialization.cpp"

auto Cartridge::allocate(Node::Port parent) -> Node::Peripheral {
  return node = parent->append<Node::Peripheral>("Mega Drive");
}

auto Cartridge::connect() -> void {
  if(!node->setPak(pak = platform->pak(node))) return;

  information = {};
  if(auto fp = pak->read("manifest.bml")) {
    information.manifest = fp->reads();
  }
  auto document = BML::unserialize(information.manifest);
  information.name = document["game/label"].string();
  information.regions = document["game/region"].string().split(",").strip();
  information.bootable = (bool)document["game/bootable"];

  if(document["game/board/memory(type=ROM,content=Patch)"]) {
    board = new Board::LockOn(*this);
  } else if(document["game/board/slot(type=Mega Drive)"]) {
    board = new Board::GameGenie(*this);
  } else if(document["game/board/memory(type=ROM,content=Program)/size"].natural() > 0x200000) {
    board = new Board::Banked(*this);
  } else {
    board = new Board::Linear(*this);
  }
  board->pak = pak;
  board->load();
  power();
}

auto Cartridge::disconnect() -> void {
  if(!node) return;
  board->pak.reset();
  board.reset();
  pak.reset();
  node.reset();
  information = {};
}

auto Cartridge::save() -> void {
  if(!node) return;
  board->save();
}

auto Cartridge::power() -> void {
  board->power();
}

auto Cartridge::read(n1 upper, n1 lower, n22 address, n16 data) -> n16 {
  return board->read(upper, lower, address, data);
}

auto Cartridge::write(n1 upper, n1 lower, n22 address, n16 data) -> void {
  return board->write(upper, lower, address, data);
}

auto Cartridge::readIO(n1 upper, n1 lower, n24 address, n16 data) -> n16 {
  return board->readIO(upper, lower, address, data);
}

auto Cartridge::writeIO(n1 upper, n1 lower, n24 address, n16 data) -> void {
  return board->writeIO(upper, lower, address, data);
}

}
