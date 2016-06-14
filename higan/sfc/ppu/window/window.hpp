struct Window {
  auto scanline() -> void;
  auto run() -> void;
  auto test(bool oneEnable, bool one, bool twoEnable, bool two, uint mask) -> bool;
  auto reset() -> void;

  auto serialize(serializer&) -> void;

  struct Registers {
    struct Layer {
      bool oneEnable;
      bool oneInvert;
      bool twoEnable;
      bool twoInvert;
      uint2 mask;
      bool aboveEnable;
      bool belowEnable;
    } bg1, bg2, bg3, bg4, oam;

    struct Color {
      bool oneEnable;
      bool oneInvert;
      bool twoEnable;
      bool twoInvert;
      uint2 mask;
      uint2 aboveMask;
      uint2 belowMask;
    } col;

    uint8 oneLeft;
    uint8 oneRight;
    uint8 twoLeft;
    uint8 twoRight;
  } r;

  struct Output {
    struct Pixel {
      bool colorEnable;
    } above, below;
  } output;

  struct {
    uint x;
  };

  friend class PPU;
};
