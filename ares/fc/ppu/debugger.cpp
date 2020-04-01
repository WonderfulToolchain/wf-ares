auto PPU::Debugger::load(Node::Object parent, Node::Object from) -> void {
  memory.ciram = Node::append<Node::Memory>(parent, from, "PPU CIRAM");
  memory.ciram->setSize(ppu.ciram.size());
  memory.ciram->setRead([&](uint32 address) -> uint8 {
    return ppu.ciram[address];
  });
  memory.ciram->setWrite([&](uint32 address, uint8 data) -> void {
    ppu.ciram[address] = data;
  });

  memory.cgram = Node::append<Node::Memory>(parent, from, "PPU CGRAM");
  memory.cgram->setSize(ppu.cgram.size());
  memory.cgram->setRead([&](uint32 address) -> uint8 {
    return ppu.cgram[address];
  });
  memory.cgram->setWrite([&](uint32 address, uint8 data) -> void {
    ppu.cgram[address] = data;
  });

  memory.oam = Node::append<Node::Memory>(parent, from, "PPU OAM");
  memory.oam->setSize(ppu.oam.size());
  memory.oam->setRead([&](uint32 address) -> uint8 {
    return ppu.oam[address];
  });
  memory.oam->setWrite([&](uint32 address, uint8 data) -> void {
    ppu.oam[address] = data;
  });
}
