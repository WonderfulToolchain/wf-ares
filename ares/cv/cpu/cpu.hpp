struct CPU : Z80, Z80::Bus, Thread {
  Node::Object node;
  Memory::Writable<uint8> ram;
  Memory::Writable<uint8> expansion;

  struct Debugger {
    //debugger.cpp
    auto load(Node::Object) -> void;
    auto instruction() -> void;
    auto interrupt(string_view) -> void;

    struct Memory {
      Node::Debugger::Memory ram;
      Node::Debugger::Memory expansion;
    } memory;

    struct Tracer {
      Node::Debugger::Tracer::Instruction instruction;
      Node::Debugger::Tracer::Notification interrupt;
    } tracer;
  } debugger;

  auto synchronizing() const -> bool override { return scheduler.synchronizing(); }

  //cpu.cpp
  auto load(Node::Object) -> void;
  auto unload() -> void;

  auto main() -> void;
  auto step(uint clocks) -> void override;

  auto setNMI(bool value) -> void;
  auto setIRQ(bool value) -> void;

  auto power() -> void;

  //memory.cpp
  auto read(uint16 address) -> uint8 override;
  auto write(uint16 address, uint8 data) -> void override;

  auto in(uint16 address) -> uint8 override;
  auto out(uint16 address, uint8 data) -> void override;

  //serialization.cpp
  auto serialize(serializer&) -> void;

private:
  struct State {
    bool nmiLine = 0;
    bool irqLine = 0;
  } state;

  struct IO {
    bool replaceRAM = 0;
    bool replaceBIOS = 0;
  } io;
};

extern CPU cpu;
