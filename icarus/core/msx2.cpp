auto Icarus::msx2Manifest(string location) -> string {
  vector<uint8_t> buffer;
  concatenate(buffer, {location, "program.rom"});
  return msx2Manifest(buffer, location);
}

auto Icarus::msx2Manifest(vector<uint8_t>& buffer, string location) -> string {
  if(settings.useDatabase) {
    auto digest = Hash::SHA256(buffer).digest();
    for(auto game : Database::MSX2.find("game")) {
      if(game["sha256"].text() == digest) return BML::serialize(game);
    }
  }

  if(settings.useHeuristics) {
    Heuristics::MSX2 game{buffer, location};
    if(auto manifest = game.manifest()) return manifest;
  }

  return {};
}

auto Icarus::msx2Import(vector<uint8_t>& buffer, string location) -> string {
  auto name = Location::prefix(location);
  auto source = Location::path(location);
  string target{settings.msx2, name, "/"};

  auto manifest = msx2Manifest(buffer, location);
  if(!manifest) return failure("failed to parse ROM image");

  if(!create(target)) return failure("library path unwritable");
  if(exists({source, name, ".sav"}) && !exists({target, "save.ram"})) {
    copy({source, name, ".sav"}, {target, "save.ram"});
  }

  if(settings.createManifests) write({target, "manifest.bml"}, manifest);
  write({target, "program.rom"}, buffer);
  return success(target);
}
