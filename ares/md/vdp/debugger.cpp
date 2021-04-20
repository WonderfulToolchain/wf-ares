auto VDP::Debugger::load(Node::Object parent) -> void {
  memory.vram = parent->append<Node::Debugger::Memory>("VDP VRAM");
  memory.vram->setSize(32_KiB << 1);
  memory.vram->setRead([&](u32 address) -> u8 {
    return self.vram.memory[n15(address >> 1)].byte(!(address & 1));
  });
  memory.vram->setWrite([&](u32 address, u8 data) -> void {
    self.vram.memory[n15(address >> 1)].byte(!(address & 1)) = data;
  });

  memory.vsram = parent->append<Node::Debugger::Memory>("VDP VSRAM");
  memory.vsram->setSize(40 << 1);
  memory.vsram->setRead([&](u32 address) -> u8 {
    if(address >= 40 << 1) return 0x00;
    return self.vsram.memory[address >> 1].byte(!(address & 1));
  });
  memory.vsram->setWrite([&](u32 address, u8 data) -> void {
    if(address >= 40 << 1) return;
    self.vsram.memory[address >> 1].byte(!(address & 1)) = data;
  });

  memory.cram = parent->append<Node::Debugger::Memory>("VDP CRAM");
  memory.cram->setSize(64 << 1);
  memory.cram->setRead([&](u32 address) -> u8 {
    return self.cram.memory[n6(address >> 1)].byte(!(address & 1));
  });
  memory.cram->setWrite([&](u32 address, u8 data) -> void {
    self.cram.memory[n6(address >> 1)].byte(!(address & 1)) = data;
  });

  tracer.interrupt = parent->append<Node::Debugger::Tracer::Notification>("Interrupt", "VDP");
  tracer.dma = parent->append<Node::Debugger::Tracer::Notification>("DMA", "VDP");
  tracer.io = parent->append<Node::Debugger::Tracer::Notification>("I/O", "VDP");
}

auto VDP::Debugger::unload() -> void {
  memory.vram.reset();
  memory.vsram.reset();
  memory.cram.reset();
}

auto VDP::Debugger::interrupt(string_view type) -> void {
  if(tracer.interrupt->enabled()) {
    string message = {type, " SR=", cpu.r.i, " @ ", vdp.vcounter(), ",", vdp.hclock()};
    tracer.interrupt->notify(message);
  }
}

auto VDP::Debugger::dma(string_view line) -> void {
  if(tracer.dma->enabled()) {
    tracer.dma->notify(line);
  }
}

auto VDP::Debugger::io(n5 register, n8 data) -> void {
  if(tracer.io->enabled()) {
    static const string name[32] = {
      /* $00 */ "mode register 1",
      /* $01 */ "mode register 2",
      /* $02 */ "layer A name table location",
      /* $03 */ "window name table location",
      /* $04 */ "layer B name table location",
      /* $05 */ "sprite attribute table location",
      /* $06 */ "sprite pattern base address",
      /* $07 */ "background color",
      /* $08 */ "",
      /* $09 */ "",
      /* $0a */ "horizontal interrupt counter",
      /* $0b */ "mode register 3",
      /* $0c */ "mode register 4",
      /* $0d */ "horizontal scroll data location",
      /* $0e */ "nametable pattern base address",
      /* $0f */ "data port auto-increment value",
      /* $10 */ "layer size",
      /* $11 */ "window plane horizontal position",
      /* $12 */ "window plane vertical position",
      /* $13 */ "DMA length (lo)",
      /* $14 */ "DMA length (hi)",
      /* $15 */ "DMA source (lo)",
      /* $16 */ "DMA source (md)",
      /* $17 */ "DMA source (hi)",
      /* $18 */ "",
      /* $19 */ "",
      /* $1a */ "",
      /* $1b */ "",
      /* $1c */ "",
      /* $1d */ "",
      /* $1e */ "",
      /* $1f */ "",
    };
    tracer.io->notify({
      "$", hex(register, 2L), " = #$", hex(data, 2L),
      " @ ", pad(vdp.vcounter(), 3L), ",", pad(vdp.hcounter(), 4L),
      "  ", name[register]
    });
  }
}
