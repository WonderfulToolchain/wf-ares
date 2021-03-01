#include <md/md.hpp>

namespace ares::MegaDrive {

Expansion& expansion = expansionPort.expansion;
#include "port.cpp"
#include "serialization.cpp"

auto Expansion::allocate(Node::Port parent) -> Node::Peripheral {
  return node = parent->append<Node::Peripheral>("Mega Drive");
}

auto Expansion::connect() -> void {
  if(!node->setPak(pak = platform->pak(node))) return;

  information = {};
  if(auto fp = pak->read("manifest.bml")) {
    information.manifest = fp->reads();
  }
  auto document = BML::unserialize(information.manifest);
  information.name = document["game/label"].string();
  information.regions = document["game/region"].string().split(",").strip();

  mcd.load(node);

  power();
}

auto Expansion::disconnect() -> void {
  if(!node) return;
  pak.reset();
  node.reset();
}

auto Expansion::save() -> void {
  if(!node) return;
}

auto Expansion::power() -> void {
}

/* the only existing expansion port device is the Mega CD, which is hard-coded below for now */

auto Expansion::read(n1 upper, n1 lower, n22 address, n16 data) -> n16 {
  return mcd.external_read(upper, lower, address, data);
}

auto Expansion::write(n1 upper, n1 lower, n22 address, n16 data) -> void {
  return mcd.external_write(upper, lower, address, data);
}

auto Expansion::readIO(n1 upper, n1 lower, n24 address, n16 data) -> n16 {
  if(!node) return data;
  return mcd.external_readIO(upper, lower, address, data);
}

auto Expansion::writeIO(n1 upper, n1 lower, n24 address, n16 data) -> void {
  if(!node) return;
  return mcd.external_writeIO(upper, lower, address, data);
}

}
