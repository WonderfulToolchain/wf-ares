struct MasterSystem : Cartridge {
  auto name() -> string override { return "Master System"; }
  auto extensions() -> vector<string> override { return {"ms", "sms"}; }
  auto export(string location) -> vector<u8> override;
  auto heuristics(vector<u8>& data, string location) -> string override;
};

auto MasterSystem::export(string location) -> vector<u8> {
  vector<u8> data;
  append(data, {location, "program.rom"});
  return data;
}

auto MasterSystem::heuristics(vector<u8>& data, string location) -> string {
  string s;
  s += "game\n";
  s +={"  name:  ", Media::name(location), "\n"};
  s +={"  label: ", Media::name(location), "\n"};
  s += "  board\n";
  s += "    memory\n";
  s += "      type: ROM\n";
  s +={"      size: 0x", hex(data.size()), "\n"};
  s += "      content: Program\n";
  s += "    memory\n";
  s += "      type: RAM\n";
  s += "      size: 0x8000\n";
  s += "      content: Save\n";
  return s;
}
