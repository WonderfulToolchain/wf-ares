struct PCEngine : Cartridge {
  auto name() -> string override { return "PC Engine"; }
  auto export(string location) -> vector<uint8_t> override;
  auto heuristics(vector<uint8_t>& data, string location) -> string override;
};

auto PCEngine::export(string location) -> vector<uint8_t> {
  vector<uint8_t> data;
  append(data, {location, "program.rom"});
  return data;
}

auto PCEngine::heuristics(vector<uint8_t>& data, string location) -> string {
  if((data.size() & 0x1fff) == 512) {
    //remove header if present
    memory::move(&data[0], &data[512], data.size() - 512);
    data.resize(data.size() - 512);
  }

  string s;
  s += "game\n";
  s +={"  name:  ", Location::prefix(location), "\n"};
  s +={"  label: ", Location::prefix(location), "\n"};
  s += "  board\n";
  s += "    memory\n";
  s += "      type: ROM\n";
  s +={"      size: 0x", hex(data.size()), "\n"};
  s += "      content: Program\n";
  return s;
}
