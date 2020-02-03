#include "../byuu.hpp"
#include "platform.cpp"
#include "load.cpp"
#include "states.cpp"
#include "status.cpp"
#include "drivers.cpp"

Program program;

auto Program::create() -> void {
  higan::platform = this;

  videoDriverUpdate();
  audioDriverUpdate();
  inputDriverUpdate();

  driverSettings.videoRefresh();
  driverSettings.audioRefresh();
  driverSettings.inputRefresh();
}

auto Program::main() -> void {
  if(Application::modal()) return;  //modal loop calls usleep() internally
  updateMessage();
  inputManager.poll();
  inputManager.pollHotkeys();
  if(!emulator) return (void)usleep(20 * 1000);
  emulator->interface->run();
}

auto Program::quit() -> void {
  unloadGame();
  presentation.setVisible(false);  //makes quitting the emulator feel more responsive
  Application::processEvents();
  Application::quit();

  ruby::video.reset();
  ruby::audio.reset();
  ruby::input.reset();
}
