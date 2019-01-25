#if defined(CORE_GB)

namespace higan::GameBoy {

struct ID {
  enum : uint {
    System,
    GameBoy,
    SuperGameBoy,
    GameBoyColor,
  };

  struct Port { enum : uint {
    Hardware,
    Cartridge,
  };};

  struct Device { enum : uint {
    Controls,
    MBC5,
    MBC7,
  };};
};

struct AbstractInterface : Interface {
  auto display() -> Display override;

  auto loaded() -> bool override;
  auto hashes() -> vector<string> override;
  auto manifests() -> vector<string> override;
  auto titles() -> vector<string> override;

  auto save() -> void override;
  auto unload() -> void override;

  auto ports() -> vector<Port> override;
  auto devices(uint port) -> vector<Device> override;
  auto inputs(uint device) -> vector<Input> override;

  auto power() -> void override;
  auto run() -> void override;

  auto serialize() -> serializer override;
  auto unserialize(serializer&) -> bool override;

  auto cheats(const vector<string>&) -> void override;

  auto options() -> Settings& override;
};

struct GameBoyInterface : AbstractInterface {
  GameBoyInterface();
  auto information() -> Information override;

  auto color(uint32 color) -> uint64 override;

  auto load() -> bool override;

  auto properties() -> Settings& override;
};

struct GameBoyColorInterface : AbstractInterface {
  GameBoyColorInterface();
  auto information() -> Information override;

  auto color(uint32 color) -> uint64 override;

  auto load() -> bool override;

  auto properties() -> Settings& override;
};

struct SuperGameBoyInterface {
  virtual auto audioSample(const double* samples, uint channels) -> void = 0;
  virtual auto inputPoll(uint port, uint device, uint id) -> int16 = 0;

  virtual auto lcdScanline() -> void = 0;
  virtual auto lcdOutput(uint2 color) -> void = 0;
  virtual auto joypRead() -> uint4 = 0;
  virtual auto joypWrite(bool p15, bool p14) -> void = 0;
};

extern SuperGameBoyInterface* superGameBoy;

#include "options.hpp"
#include "properties.hpp"

}

#endif
