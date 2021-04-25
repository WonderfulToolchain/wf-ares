auto VDP::Sprite::write(n16 address, n16 data) -> void {
  auto baseAddress = nametableAddress;
  if(vdp.h40()) baseAddress &= ~0x1ff;

  address -= baseAddress;
  if(address >= 320) return;

  auto& object = cache[address >> 2];
  switch(address & 3) {
  case 0: object.y      = data.bit( 0,10); break;
  case 1: object.link   = data.bit( 0, 6);
          object.height = data.bit( 8, 9);
          object.width  = data.bit(10,11); break;
  }
}

//called before mapping fetches
auto VDP::Sprite::begin() -> void {
  for(auto& mapping : mappings) mapping.valid = 0;
  mappingCount = 0;
}

//called before pattern fetches
auto VDP::Sprite::end() -> void {
  for(auto& pixel : pixels) pixel.color = 0;
  visibleLink  = 0;
  visibleCount = 0;
  visibleStop  = 0;
  patternIndex = 0;
  patternSlice = 0;
  patternCount = 0;
  patternStop  = 0;
}

//called 16 (H32) or 20 (H40) times
auto VDP::Sprite::mappingFetch(u32) -> void {
  if(!vdp.displayEnable()) {
    return vdp.fifo.slot();
  }

  if(test.disablePhase2) return;

  //mapping fetches are delayed when less than 16/20 objects are visible
  if(visibleCount++ < objectLimit()) return;

  auto interlace = vdp.io.interlaceMode == 3;
  auto y = 129 + (i9)vdp.vcounter();
  if(interlace) y = y << 1 | vdp.field();

  auto id = visible[mappingCount];
  auto& object = cache[id];
  auto height = 1 + object.height << 3 + interlace;

  auto baseAddress = nametableAddress;
  if(vdp.h40()) baseAddress &= ~0x1ff;

  auto& mapping = mappings[mappingCount++];
  auto address = baseAddress + id * 4 + 2;
  n16 d2 = vdp.vram.read(address++);
  n16 d3 = vdp.vram.read(address++);

  mapping.valid    = 1;
  mapping.width    = object.width;
  mapping.height   = object.height;
  mapping.address  = d2.bit(0,10) << 4 + interlace;
  mapping.hflip    = d2.bit(11);
  mapping.palette  = d2.bit(13,14);
  mapping.priority = d2.bit(15);
  mapping.x        = d3.bit(0,8);

  y = y - object.y;
  if(d2.bit(12)) y = (height - 1) - y;
  mapping.address += (y >> 3 + interlace) << 4 + interlace;
  mapping.address += (y & 7 + interlace * 8) << 1;
}

//called 32 (H32) or 40 (H40) times
auto VDP::Sprite::patternFetch(u32) -> void {
  if(!vdp.displayEnable()) {
    return vdp.fifo.slot();
  }

  if(test.disablePhase3) patternStop = 1;

  auto interlace = vdp.io.interlaceMode == 3;
  auto y = 129 + (i9)vdp.vcounter();
  if(interlace) y = y << 1 | vdp.field();

  if(!patternStop && mappings[patternIndex].valid) {
    auto& object = mappings[patternIndex];
    if(patternX && !object.x) patternStop = 1;
    patternX = object.x;
  } else {
    patternX = 0;
  }

  if(!patternStop && mappings[patternIndex].valid) {
    auto& object = mappings[patternIndex];
    auto width  = 1 + object.width  << 3;
    auto height = 1 + object.height << 3 + interlace;

    u32 x = patternSlice * 8;
    if(object.hflip) x = (width - 1) - x;

    u32 tileX = x >> 3;
    u32 tileNumber = tileX * (height >> 3 + interlace);
    n15 tileAddress = object.address + (tileNumber << 4 + interlace);

    u16 hi = vdp.vram.read(generatorAddress | tileAddress++);
    u16 lo = vdp.vram.read(generatorAddress | tileAddress++);
    u32 data = hi << 16 | lo << 0;
    if(object.hflip) data = hflip(data);
    for(auto index : range(8)) {
      n9 x = object.x + patternSlice * 8 + index - 128;
      n6 color = data >> 28;
      data <<= 4;
      if(!color) continue;
      if(pixels[x].color) {
        collision = 1;
      } else {
        color |= object.palette << 4;
        pixels[x] = {color, object.priority};
      }
    }

    if(++patternSlice >= 1 + object.width) {
      patternSlice = 0;
      if(++patternIndex >= objectLimit()) patternStop = 1;
    }
    if(++patternCount >= tileLimit()) patternStop = 1;
    if(patternX && !object.x) patternStop = 1;
    patternX = object.x;
  } else {
    patternX = 0;
  }

  y = 129 + (i9)vdp.vcounter();
  if(interlace) y = y << 1 | vdp.field();

  if(test.disablePhase1) visibleStop = 1;

  for(auto index : range(2)) {
    if(visibleStop) break;
    auto id = visibleLink;
    auto& object = cache[id];
    visibleLink = object.link;
    if(!visibleLink) visibleStop = 1;

    auto height = 1 + object.height << 3 + interlace;
    if(y <  object.y) continue;
    if(y >= object.y + height) continue;

    visible[visibleCount++] = id;
    if(visibleCount >= objectLimit()) visibleStop = 1;
  }
}

auto VDP::Sprite::pixel(u32 pixelIndex) -> Pixel {
  return pixels[pixelIndex];
}

auto VDP::Sprite::power(bool reset) -> void {
  generatorAddress = 0;
  nametableAddress = 0;
  collision = 0;
  overflow = 0;
  for(auto& pixel : pixels) pixel = {};
  for(auto& cache : this->cache) cache = {};
  for(auto& mapping : mappings) mapping = {};
  mappingCount = 0;
  patternX = 0;
  patternIndex = 0;
  patternSlice = 0;
  patternCount = 0;
  patternStop = 0;
  for(auto& visible : this->visible) visible = {};
  visibleLink = 0;
  visibleCount = 0;
  visibleStop = 0;
  test = {};
}
