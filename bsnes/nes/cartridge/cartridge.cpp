#include <nes/nes.hpp>

namespace Famicom {

#include "chip/chip.cpp"
#include "board/board.cpp"
Cartridge cartridge;

void Cartridge::Main() {
  cartridge.main();
}

void Cartridge::main() {
  board->main();
}

void Cartridge::load(const string &markup, const uint8_t *data, unsigned size) {
  information.markup = markup;

  if((size & 0xff) == 0) {
    sha256 = nall::sha256(data, size);
    board = Board::load(markup, data, size);
  } else {
    sha256 = nall::sha256(data + 16, size - 16);
    board = Board::load(markup, data + 16, size - 16);
  }
  if(board == nullptr) return;

  system.load();
  loaded = true;
}

void Cartridge::unload() {
  if(loaded == false) return;
  loaded = false;
}

unsigned Cartridge::ram_size() {
  return board->memory().size;
}

uint8* Cartridge::ram_data() {
  return board->memory().data;
}

void Cartridge::power() {
  board->power();
}

void Cartridge::reset() {
  create(Cartridge::Main, 21477272);
  board->reset();
}

Cartridge::Cartridge() {
  loaded = false;
}

uint8 Cartridge::prg_read(unsigned addr) {
  return board->prg_read(addr);
}

void Cartridge::prg_write(unsigned addr, uint8 data) {
  return board->prg_write(addr, data);
}

uint8 Cartridge::chr_read(unsigned addr) {
  return board->chr_read(addr);
}

void Cartridge::chr_write(unsigned addr, uint8 data) {
  return board->chr_write(addr, data);
}

void Cartridge::scanline(unsigned y) {
  return board->scanline(y);
}

void Cartridge::serialize(serializer &s) {
  Thread::serialize(s);
  return board->serialize(s);
}

}
