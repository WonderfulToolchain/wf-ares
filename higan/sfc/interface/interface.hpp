#ifndef SFC_HPP
namespace SuperFamicom {
#endif

struct ID {
  enum : unsigned {
    //cartridges (folders)
    System,
    SuperFamicom,
    SuperGameBoy,
    Satellaview,
    SufamiTurboSlotA,
    SufamiTurboSlotB,

    //memory (files)
    IPLROM,

    ROM,
    RAM,

    EventROM0,
    EventROM1,
    EventROM2,
    EventROM3,
    EventRAM,

    SA1ROM,
    SA1IRAM,
    SA1BWRAM,

    SuperFXROM,
    SuperFXRAM,

    ArmDSPPROM,
    ArmDSPDROM,
    ArmDSPRAM,

    HitachiDSPROM,
    HitachiDSPRAM,
    HitachiDSPDROM,
    HitachiDSPDRAM,

    Nec7725DSPPROM,
    Nec7725DSPDROM,
    Nec7725DSPRAM,

    Nec96050DSPPROM,
    Nec96050DSPDROM,
    Nec96050DSPRAM,

    EpsonRTC,
    SharpRTC,

    SPC7110PROM,
    SPC7110DROM,
    SPC7110RAM,

    SDD1ROM,
    SDD1RAM,

    OBC1RAM,

    SuperGameBoyBootROM,
    SuperGameBoyROM,
    SuperGameBoyRAM,

    BsxFlashROM,
    BsxROM,
    BsxRAM,
    BsxPSRAM,

    SufamiTurboSlotAROM,
    SufamiTurboSlotBROM,
    SufamiTurboSlotARAM,
    SufamiTurboSlotBRAM,

    //controller ports
    Port1 = 1,
    Port2 = 2,
  };
};

struct Interface : Emulator::Interface {
  double videoFrequency();
  double audioFrequency();

  bool loaded();
  string sha256();
  unsigned group(unsigned id);
  void load(unsigned id, const string &manifest);
  void save();
  void load(unsigned id, const stream &stream, const string &markup = "");
  void save(unsigned id, const stream &stream);
  void unload();

  void connect(unsigned port, unsigned device);
  void power();
  void reset();
  void run();

  bool rtc();
  void rtcsync();

  serializer serialize();
  bool unserialize(serializer&);

  void cheatSet(const lstring&);

  void paletteUpdate();

  //debugger functions
  bool tracerEnable(bool);
  void exportMemory();

  Interface();

  file tracer;
  vector<Device> device;
};

extern Interface *interface;

#ifndef SFC_HPP
}
#endif
