struct Writable {
  explicit operator bool() const {
    return size > 0;
  }

  auto reset() -> void {
    memory::free<u8, 64_KiB>(data);
    data = nullptr;
    size = 0;
    maskByte = 0;
    maskHalf = 0;
    maskWord = 0;
    maskDual = 0;
  }

  auto allocate(u32 capacity, u32 fillWith = ~0) -> void {
    reset();
    size = capacity & ~15;
    u32 mask = bit::round(size) - 1;
    maskByte = mask & ~0;
    maskHalf = mask & ~1;
    maskWord = mask & ~3;
    maskDual = mask & ~7;
    data = memory::alocate<u8, 64_KiB>(mask + 1);
    fill(fillWith);
  }

  auto fill(u32 value = 0) -> void {
    for(u32 address = 0; address < size; address += 4) {
      *(u32*)&data[address & maskWord] = bswap32(value);
    }
  }

  auto load(VFS::File fp) -> void {
    if(!size) allocate(fp->size());
    for(u32 address = 0; address < min(size, fp->size()); address += 4) {
      *(u32*)&data[address & maskWord] = bswap32(fp->readm(4L));
    }
  }

  auto save(VFS::File fp) -> void {
    for(u32 address = 0; address < min(size, fp->size()); address += 4) {
      fp->writem(bswap32(*(u32*)&data[address & maskWord]), 4L);
    }
  }

  //N64 CPU requires aligned memory accesses
  template<u32 Size>
  auto read(u32 address) -> u64 {
    if constexpr(Size == Byte) return        (*(u8* )&data[address & maskByte]);
    if constexpr(Size == Half) return bswap16(*(u16*)&data[address & maskHalf]);
    if constexpr(Size == Word) return bswap32(*(u32*)&data[address & maskWord]);
    if constexpr(Size == Dual) return bswap64(*(u64*)&data[address & maskDual]);
    unreachable;
  }
  template<u32 Size>
  auto write(u32 address, u64 value) -> void {
    if constexpr(Size == Byte) *(u8* )&data[address & maskByte] =        (value);
    if constexpr(Size == Half) *(u16*)&data[address & maskHalf] = bswap16(value);
    if constexpr(Size == Word) *(u32*)&data[address & maskWord] = bswap32(value);
    if constexpr(Size == Dual) *(u64*)&data[address & maskDual] = bswap64(value);
  }

  //N64 RSP allows unaligned memory accesses in certain cases
  template<u32 Size>
  auto readUnaligned(u32 address) -> u64 {
    static_assert(Size != Byte);
    if constexpr(Size == Half) return bswap16(*(u16*)&data[address & maskByte]);
    if constexpr(Size == Word) return bswap32(*(u32*)&data[address & maskByte]);
    if constexpr(Size == Dual) return bswap64(*(u64*)&data[address & maskByte]);
    unreachable;
  }

  template<u32 Size>
  auto writeUnaligned(u32 address, u64 value) -> void {
    static_assert(Size != Byte);
    if constexpr(Size == Half) *(u16*)&data[address & maskByte] = bswap16(value);
    if constexpr(Size == Word) *(u32*)&data[address & maskByte] = bswap32(value);
    if constexpr(Size == Dual) *(u64*)&data[address & maskByte] = bswap64(value);
  }

  auto serialize(serializer& s) -> void {
    s(array_span<u8>{data, size});
  }

//private:
  u8* data = nullptr;
  u32 size = 0;
  u32 maskByte = 0;
  u32 maskHalf = 0;
  u32 maskWord = 0;
  u32 maskDual = 0;
};
