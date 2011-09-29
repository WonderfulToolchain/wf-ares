//NES-NROM-128
//NES-NROM-256

struct NROM : Board {

struct Settings {
  bool mirror;  //0 = horizontal, 1 = vertical
} settings;

uint8 prg_read(unsigned addr) {
  if(addr & 0x8000) return Board::prg_read(addr);
  return cpu.mdr();
}

void prg_write(unsigned addr, uint8 data) {
}

uint8 chr_read(unsigned addr) {
  if(addr & 0x2000) {
    if(settings.mirror == 0) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
    return ppu.ciram_read(addr & 0x07ff);
  }
  return Board::chr_read(addr);
}

void chr_write(unsigned addr, uint8 data) {
  if(addr & 0x2000) {
    if(settings.mirror == 0) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
    return ppu.ciram_write(addr & 0x07ff, data);
  }
  return Board::chr_write(addr, data);
}

NROM(BML::Node &board, const uint8_t *data, unsigned size) : Board(board, data, size) {
  settings.mirror = board["mirror"].value() == "vertical" ? 1 : 0;
}

};
