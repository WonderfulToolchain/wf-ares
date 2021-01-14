auto GPU::Blitter::queue() -> void {
  self.refreshed = true;

  //if the display is disabled, output a black screen image
  if(blank = self.io.displayDisable) {
    self.screen->frame();
    return;
  }

  depth = self.io.colorDepth;

  width  = self.display.width;
  height = self.display.height;
  if(self.overscan->value() == 0) {
    if(height == 240) height = 224;
    if(height == 480) height = 448;
  }

  int offsetX1 = 0;
  if(self.display.width == 256) offsetX1 = 0x228;
  if(self.display.width == 320) offsetX1 = 0x260;
  if(self.display.width == 368) offsetX1 = 0x260;  //untested
  if(self.display.width == 512) offsetX1 = 0x260;
  if(self.display.width == 640) offsetX1 = 0x260;

  int offsetY1 = 0;
  if(self.display.height == 240) offsetY1 = 0x10;
  if(self.display.height == 480) offsetY1 = 0x10;

  //target coordinates
  tx = max(((int)self.io.displayRangeX1 - offsetX1) / (int)self.display.dotclock, 0);
  ty = max(((int)self.io.displayRangeY1 - offsetY1) << self.display.interlace, 0);
  tw = max(((int)self.io.displayRangeX2 - (int)self.io.displayRangeX1) / (int)self.display.dotclock & ~3, 0);
  th = max(((int)self.io.displayRangeY2 - (int)self.io.displayRangeY1) << self.display.interlace, 0);

  //ensure coordinates cannot go out of bounds of the output region
  if(tx > 1023) tx = 1023;
  if(ty >  511) ty =  511;
  if(tx + tw > 1023) tw = 1023 - tx;
  if(ty + th >  511) th =  511 - ty;

  //overscan offset may send ty out of bounds, but the rendering below will skip drawing these lines
  if(self.overscan->value() == 0) {
    if(self.display.height == 240) ty -=  8;
    if(self.display.height == 480) ty -= 16;
  }

  if(tx != self.display.previous.x
  || ty != self.display.previous.y
  || tw != self.display.previous.width
  || th != self.display.previous.height
  ) {
    //the video output area has changed: clear the output buffer completely.
    //this ensures that none of the previous frames are still drawn to the screen.
    //todo
  }
  self.display.previous.x = tx;
  self.display.previous.y = ty;
  self.display.previous.width  = tw;
  self.display.previous.height = th;

  //source coordinates
  sx = self.io.displayStartX;
  sy = self.io.displayStartY;

  self.screen->setViewport(0, 0, width, height);
  self.screen->frame();
}

auto GPU::Blitter::refresh() -> void {
  if(blank) return;

  self.vram.mutex.lock();
  auto output = self.screen->pixels(1).data();

  //15bpp
  if(depth == 0) {
    for(uint y : range(th)) {
      if(y + ty < 0 || y + ty >= 512) continue;
      u32 source = (y + sy) * 1024 * 2 + sx * 2;
      u32 target = (y + ty) *  640 * 1 + tx * 1;
      for(uint x : range(tw)) {
        u16 data = self.vram.readHalf(source);
        output[target++] = 1 << 24 | data & 0x7fff;
        source += 2;
      }
    }
  }

  //24bpp
  if(depth == 1) {
    for(uint y : range(th)) {
      if(y + ty < 0 || y + ty >= 512) continue;
      u32 source = (y + sy) * 1024 * 2 + sx * 2;
      u32 target = (y + ty) *  640 * 1 + tx * 1;
      for(uint x : range(tw)) {
        u32 data = self.vram.readWordUnaligned(source);
        output[target++] = data & 0xffffff;
        source += 3;
      }
    }
  }

  self.vram.mutex.unlock();
}

auto GPU::Blitter::power() -> void {
}
