PPU::Registers::Control::operator uint16() const {
  return (
    (bgmode            <<  0)
  | (cgbmode           <<  3)
  | (frame             <<  4)
  | (hblank            <<  5)
  | (objmapping        <<  6)
  | (forceblank        <<  7)
  | (enablebg[0]       <<  8)
  | (enablebg[1]       <<  9)
  | (enablebg[2]       << 10)
  | (enablebg[3]       << 11)
  | (enableobj         << 12)
  | (enablebgwindow[0] << 13)
  | (enablebgwindow[1] << 14)
  | (enableobjwindow   << 15)
  );
}

uint16 PPU::Registers::Control::operator=(uint16 source) {
  bgmode            = source & 0x0007;
  cgbmode           = source & 0x0008;
  frame             = source & 0x0010;
  hblank            = source & 0x0020;
  objmapping        = source & 0x0040;
  forceblank        = source & 0x0080;
  enablebg[0]       = source & 0x0100;
  enablebg[1]       = source & 0x0200;
  enablebg[2]       = source & 0x0400;
  enablebg[3]       = source & 0x0800;
  enableobj         = source & 0x1000;
  enablebgwindow[0] = source & 0x2000;
  enablebgwindow[1] = source & 0x4000;
  enableobjwindow   = source & 0x8000;
  return operator uint16();
}

PPU::Registers::Status::operator uint16() const {
  return (
    (vblank          << 0)
  | (hblank          << 1)
  | (vcoincidence    << 2)
  | (irqvblank       << 3)
  | (irqhblank       << 4)
  | (irqvcoincidence << 5)
  | (vcompare        << 8)
  );
}

uint16 PPU::Registers::Status::operator=(uint16 source) {
  vblank          = source & 0x0001;
  hblank          = source & 0x0002;
  vcoincidence    = source & 0x0004;
  irqvblank       = source & 0x0008;
  irqhblank       = source & 0x0010;
  irqvcoincidence = source & 0x0020;
  vcompare        = source >> 8;
  return operator uint16();
}

PPU::Registers::BackgroundControl::operator uint16() const {
  return (
    (priority           <<  0)
  | (characterbaseblock <<  2)
  | (mosaic             <<  6)
  | (colormode          <<  7)
  | (screenbaseblock    <<  8)
  | (affinewrap         << 13)
  | (screensize         << 14)
  );
}

uint16 PPU::Registers::BackgroundControl::operator=(uint16 source) {
  priority           = source >>  0;
  characterbaseblock = source >>  2;
  mosaic             = source >>  6;
  colormode          = source >>  7;
  screenbaseblock    = source >>  8;
  affinewrap         = source >> 13;
  screensize         = source >> 14;
  return operator uint16();
}

PPU::Registers::WindowFlags::operator uint8() const {
  return (
    (enablebg[0] << 0)
  | (enablebg[1] << 1)
  | (enablebg[2] << 2)
  | (enablebg[3] << 3)
  | (enableobj   << 4)
  | (enablesfx   << 5)
  );
}

uint8 PPU::Registers::WindowFlags::operator=(uint8 source) {
  enablebg[0] = source & 0x01;
  enablebg[1] = source & 0x02;
  enablebg[2] = source & 0x04;
  enablebg[3] = source & 0x08;
  enableobj   = source & 0x10;
  enablesfx   = source & 0x20;
  return operator uint8();
}

PPU::Registers::BlendControl::operator uint16() const {
  return (
    (above[1] <<  0)
  | (above[2] <<  1)
  | (above[3] <<  2)
  | (above[4] <<  3)
  | (above[0] <<  4)
  | (above[5] <<  5)
  | (mode     <<  6)
  | (below[1] <<  8)
  | (below[2] <<  9)
  | (below[3] << 10)
  | (below[4] << 11)
  | (below[0] << 12)
  | (below[5] << 13)
  );
}

uint16 PPU::Registers::BlendControl::operator=(uint16 source) {
  above[1] = source & (1 <<  0);
  above[2] = source & (1 <<  1);
  above[3] = source & (1 <<  2);
  above[4] = source & (1 <<  3);
  above[0] = source & (1 <<  4);
  above[5] = source & (1 <<  5);
  mode     = source >> 6;
  below[1] = source & (1 <<  8);
  below[2] = source & (1 <<  9);
  below[3] = source & (1 << 10);
  below[4] = source & (1 << 11);
  below[0] = source & (1 << 12);
  below[5] = source & (1 << 13);
  return operator uint16();
}
