struct MasterSystem : System {
  auto name() -> string override { return "Master System"; }
  auto load(string location) -> bool override;
  auto save(string location) -> bool override;
};

auto MasterSystem::load(string location) -> bool {
  this->location = locate();
  pak = new vfs::directory;
  return true;
}

auto MasterSystem::save(string location) -> bool {
  return true;
}
