auto PPU::latchCounters() -> void {
  cpu.synchronize(ppu);
  io.hcounter = hdot();
  io.vcounter = vcounter();
  latch.counters = 1;
}

auto PPU::addressVRAM() const -> uint16 {
  uint16 address = io.vramAddress;
  switch(io.vramMapping) {
  case 0: return address;
  case 1: return address.range( 8,15) <<  8 | address.range(0,4) << 3 | address.range(5,7);
  case 2: return address.range( 9,15) <<  9 | address.range(0,5) << 3 | address.range(6,8);
  case 3: return address.range(10,15) << 10 | address.range(0,6) << 3 | address.range(7,9);
  }
  unreachable;
}

auto PPU::readVRAM() -> uint16 {
  if(!io.displayDisable && vcounter() < vdisp()) return 0x0000;
  auto address = addressVRAM();
  return vram[address];
}

auto PPU::writeVRAM(uint1 byte, uint8 data) -> void {
  if(!io.displayDisable && vcounter() < vdisp()) return;
  auto address = addressVRAM();
  if(byte == 0) vram[address].range(0, 7) = data;
  if(byte == 1) vram[address].range(8,15) = data;
}

auto PPU::readOAM(uint10 address) -> uint8 {
  if(!io.displayDisable && vcounter() < vdisp()) {
    if(address.field(9) == 0) return obj.oam.read(0x000 | latch.oamAddress << 2 | address & 1);
    if(address.field(9) == 1) return obj.oam.read(0x200 | latch.oamAddress >> 2);
  }
  return obj.oam.read(address);
}

auto PPU::writeOAM(uint10 address, uint8 data) -> void {
  if(!io.displayDisable && vcounter() < vdisp()) {
    if(address.field(9) == 0) obj.oam.write(0x000 | latch.oamAddress << 2 | address & 1, data);
    if(address.field(9) == 1) obj.oam.write(0x200 | latch.oamAddress >> 2, data);
    return;
  }
  obj.oam.write(address, data);
}

auto PPU::readCGRAM(uint1 byte, uint8 address) -> uint8 {
  if(!io.displayDisable
  && vcounter() > 0 && vcounter() < vdisp()
  && hcounter() >= 88 && hcounter() < 1096
  ) address = latch.cgramAddress;
  return screen.cgram[address] >> byte * 8;
}

auto PPU::writeCGRAM(uint8 address, uint15 data) -> void {
  if(!io.displayDisable
  && vcounter() > 0 && vcounter() < vdisp()
  && hcounter() >= 88 && hcounter() < 1096
  ) address = latch.cgramAddress;
  screen.cgram[address] = data;
}

auto PPU::readIO(uint24 address, uint8 data) -> uint8 {
  cpu.synchronize(ppu);

  switch((uint16)address) {

  case 0x2104: case 0x2105: case 0x2106: case 0x2108:
  case 0x2109: case 0x210a: case 0x2114: case 0x2115:
  case 0x2116: case 0x2118: case 0x2119: case 0x211a:
  case 0x2124: case 0x2125: case 0x2126: case 0x2128:
  case 0x2129: case 0x212a: {
    return ppu1.mdr;
  }

  //MPYL
  case 0x2134: {
    uint24 result = (int16)io.m7a * (int8)(io.m7b >> 8);
    return ppu1.mdr = result >>  0;
  }

  //MPYM
  case 0x2135: {
    uint24 result = (int16)io.m7a * (int8)(io.m7b >> 8);
    return ppu1.mdr = result >>  8;
  }

  //MPYH
  case 0x2136: {
    uint24 result = (int16)io.m7a * (int8)(io.m7b >> 8);
    return ppu1.mdr = result >> 16;
  }

  //SLHV
  case 0x2137: {
    if(cpu.pio().field(7)) latchCounters();
    return data;  //CPU MDR
  }

  //OAMDATAREAD
  case 0x2138: {
    ppu1.mdr = readOAM(io.oamAddress++);
    obj.setFirstSprite();
    return ppu1.mdr;
  }

  //VMDATALREAD
  case 0x2139: {
    ppu1.mdr = latch.vram >> 0;
    if(io.vramIncrementMode == 0) {
      latch.vram = readVRAM();
      io.vramAddress += io.vramIncrementSize;
    }
    return ppu1.mdr;
  }

  //VMDATAHREAD
  case 0x213a: {
    ppu1.mdr = latch.vram >> 8;
    if(io.vramIncrementMode == 1) {
      latch.vram = readVRAM();
      io.vramAddress += io.vramIncrementSize;
    }
    return ppu1.mdr;
  }

  //CGDATAREAD
  case 0x213b: {
    if(io.cgramAddressLatch++ == 0) {
      ppu2.mdr.range(0,7) = readCGRAM(0, io.cgramAddress);
    } else {
      ppu2.mdr.range(0,6) = readCGRAM(1, io.cgramAddress++);
    }
    return ppu2.mdr;
  }

  //OPHCT
  case 0x213c: {
    if(latch.hcounter++ == 0) {
      ppu2.mdr.range(0,7) = io.hcounter.range(0,7);
    } else {
      ppu2.mdr.field(0  ) = io.hcounter.field(8  );
    }
    return ppu2.mdr;
  }

  //OPVCT
  case 0x213d: {
    if(latch.vcounter++ == 0) {
      ppu2.mdr.range(0,7) = io.vcounter.range(0,7);
    } else {
      ppu2.mdr.field(0  ) = io.vcounter.field(8  );
    }
    return ppu2.mdr;
  }

  //STAT77
  case 0x213e: {
    ppu1.mdr.range(0,3) = ppu1.version;
    ppu1.mdr.field(  5) = 0;
    ppu1.mdr.field(  6) = obj.io.rangeOver;
    ppu1.mdr.field(  7) = obj.io.timeOver;
    return ppu1.mdr;
  }

  //STAT78
  case 0x213f: {
    latch.hcounter = 0;
    latch.vcounter = 0;
    ppu2.mdr.range(0,3) = ppu2.version;
    ppu2.mdr.field(  4) = Region::PAL();  //0 = NTSC, 1 = PAL
    if(!cpu.pio().field(7)) {
      ppu2.mdr.field(6) = 1;
    } else {
      ppu2.mdr.field(6) = latch.counters;
      latch.counters = 0;
    }
    ppu2.mdr.field(7) = field();
    return ppu2.mdr;
  }

  }

  return data;
}

auto PPU::writeIO(uint24 address, uint8 data) -> void {
  cpu.synchronize(ppu);

  switch((uint16)address) {

  //INIDISP
  case 0x2100: {
    if(io.displayDisable && vcounter() == vdisp()) obj.addressReset();
    io.displayBrightness = data.range(0,3);
    io.displayDisable    = data.field(7);
    return;
  }

  //OBSEL
  case 0x2101: {
    obj.io.tiledataAddress = data.range(0,2) << 13;
    obj.io.nameselect      = data.range(3,4);
    obj.io.baseSize        = data.range(5,7);
    return;
  }

  //OAMADDL
  case 0x2102: {
    io.oamBaseAddress.range(1,8) = data.range(0,7);
    obj.addressReset();
    return;
  }

  //OAMADDH
  case 0x2103: {
    io.oamBaseAddress.field(9) = data.field(0);
    io.oamPriority = data.field(7);
    obj.addressReset();
    return;
  }

  //OAMDATA
  case 0x2104: {
    uint1 latchBit = io.oamAddress.field(0);
    uint10 address = io.oamAddress++;
    if(latchBit == 0) latch.oam = data;
    if(address.field(9)) {
      writeOAM(address, data);
    } else if(latchBit == 1) {
      writeOAM((address & ~1) + 0, latch.oam);
      writeOAM((address & ~1) + 1, data);
    }
    obj.setFirstSprite();
    return;
  }

  //BGMODE
  case 0x2105: {
    io.bgMode       = data.range(0,2);
    io.bgPriority   = data.field(  3);
    bg1.io.tileSize = data.field(  4);
    bg2.io.tileSize = data.field(  5);
    bg3.io.tileSize = data.field(  6);
    bg4.io.tileSize = data.field(  7);
    updateVideoMode();
    return;
  }

  //MOSAIC
  case 0x2106: {
    bg1.mosaic.enable = data.field(0);
    bg2.mosaic.enable = data.field(1);
    bg3.mosaic.enable = data.field(2);
    bg4.mosaic.enable = data.field(3);
    Background::Mosaic::size = data.range(4,7);
    return;
  }

  //BG1SC
  case 0x2107: {
    bg1.io.screenSize    = data.range(0,1);
    bg1.io.screenAddress = data.range(2,7) << 10;
    return;
  }

  //BG2SC
  case 0x2108: {
    bg2.io.screenSize    = data.range(0,1);
    bg2.io.screenAddress = data.range(2,7) << 10;
    return;
  }

  //BG3SC
  case 0x2109: {
    bg3.io.screenSize    = data.range(0,1);
    bg3.io.screenAddress = data.range(2,7) << 10;
    return;
  }

  //BG4SC
  case 0x210a: {
    bg4.io.screenSize    = data.range(0,1);
    bg4.io.screenAddress = data.range(2,7) << 10;
    return;
  }

  //BG12NBA
  case 0x210b: {
    bg1.io.tiledataAddress = data.range(0,3) << 12;
    bg2.io.tiledataAddress = data.range(4,7) << 12;
    return;
  }

  //BG34NBA
  case 0x210c: {
    bg3.io.tiledataAddress = data.range(0,3) << 12;
    bg4.io.tiledataAddress = data.range(4,7) << 12;
    return;
  }

  //BG1HOFS
  case 0x210d: {
    io.hoffsetMode7 = data << 8 | latch.mode7;
    latch.mode7 = data;

    bg1.io.hoffset = data << 8 | latch.bgofsPPU1 & ~7 | latch.bgofsPPU2 & 7;
    latch.bgofsPPU1 = data;
    latch.bgofsPPU2 = data;
    return;
  }

  //BG1VOFS
  case 0x210e: {
    io.voffsetMode7 = data << 8 | latch.mode7;
    latch.mode7 = data;

    bg1.io.voffset = data << 8 | latch.bgofsPPU1;
    latch.bgofsPPU1 = data;
    return;
  }

  //BG2HOFS
  case 0x210f: {
    bg2.io.hoffset = data << 8 | latch.bgofsPPU1 & ~7 | latch.bgofsPPU2 & 7;
    latch.bgofsPPU1 = data;
    latch.bgofsPPU2 = data;
    return;
  }

  //BG2VOFS
  case 0x2110: {
    bg2.io.voffset = data << 8 | latch.bgofsPPU1;
    latch.bgofsPPU1 = data;
    return;
  }

  //BG3HOFS
  case 0x2111: {
    bg3.io.hoffset = data << 8 | latch.bgofsPPU1 & ~7 | latch.bgofsPPU2 & 7;
    latch.bgofsPPU1 = data;
    latch.bgofsPPU2 = data;
    return;
  }

  //BG3VOFS
  case 0x2112: {
    bg3.io.voffset = data << 8 | latch.bgofsPPU1;
    latch.bgofsPPU1 = data;
    return;
  }

  //BG4HOFS
  case 0x2113: {
    bg4.io.hoffset = data << 8 | latch.bgofsPPU1 & ~7 | latch.bgofsPPU2 & 7;
    latch.bgofsPPU1 = data;
    latch.bgofsPPU2 = data;
    return;
  }

  //BG4VOFS
  case 0x2114: {
    bg4.io.voffset = data << 8 | latch.bgofsPPU1;
    latch.bgofsPPU1 = data;
    return;
  }

  //VMAIN
  case 0x2115: {
    static const uint size[4] = {1, 32, 128, 128};
    io.vramIncrementSize = size[data.range(0,1)];
    io.vramMapping       = data.range(2,3);
    io.vramIncrementMode = data.field(  7);
    return;
  }

  //VMADDL
  case 0x2116: {
    io.vramAddress.range(0,7) = data;
    latch.vram = readVRAM();
    return;
  }

  //VMADDH
  case 0x2117: {
    io.vramAddress.range(8,15) = data;
    latch.vram = readVRAM();
    return;
  }

  //VMDATAL
  case 0x2118: {
    writeVRAM(0, data);
    if(io.vramIncrementMode == 0) io.vramAddress += io.vramIncrementSize;
    return;
  }

  //VMDATAH
  case 0x2119: {
    writeVRAM(1, data);
    if(io.vramIncrementMode == 1) io.vramAddress += io.vramIncrementSize;
    return;
  }

  //M7SEL
  case 0x211a: {
    io.hflipMode7  = data.field(  0);
    io.vflipMode7  = data.field(  1);
    io.repeatMode7 = data.range(6,7);
    return;
  }

  //M7A
  case 0x211b: {
    io.m7a = data << 8 | latch.mode7;
    latch.mode7 = data;
    return;
  }

  //M7B
  case 0x211c: {
    io.m7b = data << 8 | latch.mode7;
    latch.mode7 = data;
    return;
  }

  //M7C
  case 0x211d: {
    io.m7c = data << 8 | latch.mode7;
    latch.mode7 = data;
    return;
  }

  //M7D
  case 0x211e: {
    io.m7d = data << 8 | latch.mode7;
    latch.mode7 = data;
    return;
  }

  //M7X
  case 0x211f: {
    io.m7x = data << 8 | latch.mode7;
    latch.mode7 = data;
    return;
  }

  //M7Y
  case 0x2120: {
    io.m7y = data << 8 | latch.mode7;
    latch.mode7 = data;
    return;
  }

  //CGADD
  case 0x2121: {
    io.cgramAddress = data;
    io.cgramAddressLatch = 0;
    return;
  }

  //CGDATA
  case 0x2122: {
    if(io.cgramAddressLatch++ == 0) {
      latch.cgram = data;
    } else {
      writeCGRAM(io.cgramAddress++, data.range(0,6) << 8 | latch.cgram);
    }
    return;
  }

  //W12SEL
  case 0x2123: {
    window.io.bg1.oneInvert = data.field(0);
    window.io.bg1.oneEnable = data.field(1);
    window.io.bg1.twoInvert = data.field(2);
    window.io.bg1.twoEnable = data.field(3);
    window.io.bg2.oneInvert = data.field(4);
    window.io.bg2.oneEnable = data.field(5);
    window.io.bg2.twoInvert = data.field(6);
    window.io.bg2.twoEnable = data.field(7);
    return;
  }

  //W34SEL
  case 0x2124: {
    window.io.bg3.oneInvert = data.field(0);
    window.io.bg3.oneEnable = data.field(1);
    window.io.bg3.twoInvert = data.field(2);
    window.io.bg3.twoEnable = data.field(3);
    window.io.bg4.oneInvert = data.field(4);
    window.io.bg4.oneEnable = data.field(5);
    window.io.bg4.twoInvert = data.field(6);
    window.io.bg4.twoEnable = data.field(7);
    return;
  }

  //WOBJSEL
  case 0x2125: {
    window.io.obj.oneInvert = data.field(0);
    window.io.obj.oneEnable = data.field(1);
    window.io.obj.twoInvert = data.field(2);
    window.io.obj.twoEnable = data.field(3);
    window.io.col.oneInvert = data.field(4);
    window.io.col.oneEnable = data.field(5);
    window.io.col.twoInvert = data.field(6);
    window.io.col.twoEnable = data.field(7);
    return;
  }

  //WH0
  case 0x2126: {
    window.io.oneLeft = data;
    return;
  }

  //WH1
  case 0x2127: {
    window.io.oneRight = data;
    return;
  }

  //WH2
  case 0x2128: {
    window.io.twoLeft = data;
    return;
  }

  //WH3
  case 0x2129: {
    window.io.twoRight = data;
    return;
  }

  //WBGLOG
  case 0x212a: {
    window.io.bg1.mask = data.range(0,1);
    window.io.bg2.mask = data.range(2,3);
    window.io.bg3.mask = data.range(4,5);
    window.io.bg4.mask = data.range(6,7);
    return;
  }

  //WOBJLOG
  case 0x212b: {
    window.io.obj.mask = data.range(0,1);
    window.io.col.mask = data.range(2,3);
    return;
  }

  //TM
  case 0x212c: {
    bg1.io.aboveEnable = data.field(0);
    bg2.io.aboveEnable = data.field(1);
    bg3.io.aboveEnable = data.field(2);
    bg4.io.aboveEnable = data.field(3);
    obj.io.aboveEnable = data.field(4);
    return;
  }

  //TS
  case 0x212d: {
    bg1.io.belowEnable = data.field(0);
    bg2.io.belowEnable = data.field(1);
    bg3.io.belowEnable = data.field(2);
    bg4.io.belowEnable = data.field(3);
    obj.io.belowEnable = data.field(4);
    return;
  }

  //TMW
  case 0x212e: {
    window.io.bg1.aboveEnable = data.field(0);
    window.io.bg2.aboveEnable = data.field(1);
    window.io.bg3.aboveEnable = data.field(2);
    window.io.bg4.aboveEnable = data.field(3);
    window.io.obj.aboveEnable = data.field(4);
    return;
  }

  //TSW
  case 0x212f: {
    window.io.bg1.belowEnable = data.field(0);
    window.io.bg2.belowEnable = data.field(1);
    window.io.bg3.belowEnable = data.field(2);
    window.io.bg4.belowEnable = data.field(3);
    window.io.obj.belowEnable = data.field(4);
    return;
  }

  //CGWSEL
  case 0x2130: {
    screen.io.directColor   = data.field(  0);
    screen.io.blendMode     = data.field(  1);
    window.io.col.belowMask = data.range(4,5);
    window.io.col.aboveMask = data.range(6,7);
    return;
  }

  //CGADDSUB
  case 0x2131: {
    screen.io.bg1.colorEnable  = data.field(0);
    screen.io.bg2.colorEnable  = data.field(1);
    screen.io.bg3.colorEnable  = data.field(2);
    screen.io.bg4.colorEnable  = data.field(3);
    screen.io.obj.colorEnable  = data.field(4);
    screen.io.back.colorEnable = data.field(5);
    screen.io.colorHalve       = data.field(6);
    screen.io.colorMode        = data.field(7);
    return;
  }

  //COLDATA
  case 0x2132: {
    if(data.field(5)) screen.io.colorRed   = data.range(0,4);
    if(data.field(6)) screen.io.colorGreen = data.range(0,4);
    if(data.field(7)) screen.io.colorBlue  = data.range(0,4);
    return;
  }

  //SETINI
  case 0x2133: {
    io.interlace     = data.field(0);
    obj.io.interlace = data.field(1);
    io.overscan      = data.field(2);
    io.pseudoHires   = data.field(3);
    io.extbg         = data.field(6);
    updateVideoMode();
    return;
  }

  }
}

auto PPU::updateVideoMode() -> void {
  self.vdisp = !io.overscan ? 225 : 240;

  switch(io.bgMode) {
  case 0:
    bg1.io.mode = Background::Mode::BPP2;
    bg2.io.mode = Background::Mode::BPP2;
    bg3.io.mode = Background::Mode::BPP2;
    bg4.io.mode = Background::Mode::BPP2;
    memory::assign(bg1.io.priority, 8, 11);
    memory::assign(bg2.io.priority, 7, 10);
    memory::assign(bg3.io.priority, 2,  5);
    memory::assign(bg4.io.priority, 1,  4);
    memory::assign(obj.io.priority, 3,  6, 9, 12);
    break;

  case 1:
    bg1.io.mode = Background::Mode::BPP4;
    bg2.io.mode = Background::Mode::BPP4;
    bg3.io.mode = Background::Mode::BPP2;
    bg4.io.mode = Background::Mode::Inactive;
    if(io.bgPriority) {
      memory::assign(bg1.io.priority, 5,  8);
      memory::assign(bg2.io.priority, 4,  7);
      memory::assign(bg3.io.priority, 1, 10);
      memory::assign(obj.io.priority, 2,  3, 6,  9);
    } else {
      memory::assign(bg1.io.priority, 6,  9);
      memory::assign(bg2.io.priority, 5,  8);
      memory::assign(bg3.io.priority, 1,  3);
      memory::assign(obj.io.priority, 2,  4, 7, 10);
    }
    break;

  case 2:
    bg1.io.mode = Background::Mode::BPP4;
    bg2.io.mode = Background::Mode::BPP4;
    bg3.io.mode = Background::Mode::Inactive;
    bg4.io.mode = Background::Mode::Inactive;
    memory::assign(bg1.io.priority, 3, 7);
    memory::assign(bg2.io.priority, 1, 5);
    memory::assign(obj.io.priority, 2, 4, 6, 8);
    break;

  case 3:
    bg1.io.mode = Background::Mode::BPP8;
    bg2.io.mode = Background::Mode::BPP4;
    bg3.io.mode = Background::Mode::Inactive;
    bg4.io.mode = Background::Mode::Inactive;
    memory::assign(bg1.io.priority, 3, 7);
    memory::assign(bg2.io.priority, 1, 5);
    memory::assign(obj.io.priority, 2, 4, 6, 8);
    break;

  case 4:
    bg1.io.mode = Background::Mode::BPP8;
    bg2.io.mode = Background::Mode::BPP2;
    bg3.io.mode = Background::Mode::Inactive;
    bg4.io.mode = Background::Mode::Inactive;
    memory::assign(bg1.io.priority, 3, 7);
    memory::assign(bg2.io.priority, 1, 5);
    memory::assign(obj.io.priority, 2, 4, 6, 8);
    break;

  case 5:
    bg1.io.mode = Background::Mode::BPP4;
    bg2.io.mode = Background::Mode::BPP2;
    bg3.io.mode = Background::Mode::Inactive;
    bg4.io.mode = Background::Mode::Inactive;
    memory::assign(bg1.io.priority, 3, 7);
    memory::assign(bg2.io.priority, 1, 5);
    memory::assign(obj.io.priority, 2, 4, 6, 8);
    break;

  case 6:
    bg1.io.mode = Background::Mode::BPP4;
    bg2.io.mode = Background::Mode::Inactive;
    bg3.io.mode = Background::Mode::Inactive;
    bg4.io.mode = Background::Mode::Inactive;
    memory::assign(bg1.io.priority, 2, 5);
    memory::assign(obj.io.priority, 1, 3, 4, 6);
    break;

  case 7:
    if(!io.extbg) {
      bg1.io.mode = Background::Mode::Mode7;
      bg2.io.mode = Background::Mode::Inactive;
      bg3.io.mode = Background::Mode::Inactive;
      bg4.io.mode = Background::Mode::Inactive;
      memory::assign(bg1.io.priority, 2);
      memory::assign(obj.io.priority, 1, 3, 4, 5);
    } else {
      bg1.io.mode = Background::Mode::Mode7;
      bg2.io.mode = Background::Mode::Mode7;
      bg3.io.mode = Background::Mode::Inactive;
      bg4.io.mode = Background::Mode::Inactive;
      memory::assign(bg1.io.priority, 3);
      memory::assign(bg2.io.priority, 1, 5);
      memory::assign(obj.io.priority, 2, 4, 6, 7);
    }
    break;
  }
}
