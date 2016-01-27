//20000-fffff
auto Cartridge::romRead(uint addr) -> uint8 {
  switch(addr >> 16) {
  case 2:  addr = r.bank_rom0 << 16 | (uint16)addr; break;  //20000-2ffff
  case 3:  addr = r.bank_rom1 << 16 | (uint16)addr; break;  //30000-3ffff
  default: addr = r.bank_rom2 << 16 | (uint16)addr; break;  //40000-fffff
  }
  if(!rom.data || addr >= rom.size) return 0x00;
  return rom.data[addr];
}

auto Cartridge::romWrite(uint addr, uint8 data) -> void {
}

//10000-1ffff
auto Cartridge::ramRead(uint addr) -> uint8 {
  addr = r.bank_sram << 16 | (uint16)addr;
  if(!ram.data || addr >= ram.size) return 0x00;
  return ram.data[addr];
}

auto Cartridge::ramWrite(uint addr, uint8 data) -> void {
  addr = r.bank_sram << 16 | (uint16)addr;
  if(!ram.data || addr >= ram.size) return;
  ram.data[addr] = data;
}
