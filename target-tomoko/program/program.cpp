#include "../tomoko.hpp"
#include <fc/interface/interface.hpp>
#include <sfc/interface/interface.hpp>
#include <gb/interface/interface.hpp>
#include <gba/interface/interface.hpp>
#include "interface.cpp"
#include "media.cpp"
Program* program = nullptr;

Program::Program() {
  program = this;
  directory::create({configpath(), "tomoko/"});
  Application::onMain({&Program::main, this});

  emulators.append(new Famicom::Interface);
  emulators.append(new SuperFamicom::Interface);
  emulators.append(new GameBoy::Interface);
  emulators.append(new GameBoyAdvance::Interface);
  for(auto& emulator : emulators) emulator->bind = this;

  new InputManager;
  new LibraryManager;
  new SettingsManager;
  new Presentation;

  presentation->setVisible();

  video.driver("XShm");
  video.set(Video::Handle, presentation->viewport.handle());
  video.set(Video::Synchronize, false);
  if(!video.init()) { video.driver("None"); video.init(); }

  audio.driver("OpenAL");
  audio.set(Audio::Handle, presentation->viewport.handle());
  audio.set(Audio::Synchronize, false);
  audio.set(Audio::Frequency, 96000u);
  audio.set(Audio::Latency, 80u);
  if(!audio.init()) { audio.driver("None"); audio.init(); }

  input.driver("Xlib");
  input.set(Input::Handle, presentation->viewport.handle());
  if(!input.init()) { input.driver("None"); input.init(); }

  dsp.setPrecision(16);
  dsp.setBalance(0.0);
  dsp.setVolume(1.0);
  dsp.setFrequency(32040);
  dsp.setResampler(DSP::ResampleEngine::Sinc);
  dsp.setResamplerFrequency(96000);

  drawSplashScreen();
}

auto Program::emulator() -> Emulator::Interface& {
  if(activeEmulator == nullptr) throw;
  return *activeEmulator;
}

auto Program::main() -> void {
  inputManager->poll();

  if(activeEmulator == nullptr || emulator().loaded() == false) {
    audio.clear();
    usleep(20 * 1000);
    return;
  }

  emulator().run();
}

auto Program::quit() -> void {
  unloadMedia();
  inputManager->quit();
  exit(0);
}

auto Program::setEmulator(Emulator::Interface* emulator) -> void {
  activeEmulator = emulator;
}

auto Program::drawSplashScreen() -> void {
  image emblem{string{userpath(), ".local/share/icons/tomoko.png"}};
  emblem.alphaBlend(0x000000);
  emblem.scale(128, 128);

  uint32* output;
  unsigned length;
  if(video.lock(output, length, 512, 480)) {
    for(auto y : range(480)) {
      uint32* dp = output + y * (length >> 2);
      for(auto x : range(640)) *dp++ = 0xff000000;
    }
    unsigned z = 0;
    for(auto y : range(emblem.height)) {
      uint32* dp = output + (480 - emblem.height + y - 8) * (length >> 2) + (512 - emblem.width - 8);
      for(auto x : range(emblem.width)) {
        *dp++ = emblem.read(emblem.data + z);
        z += 4;
      }
    }
    video.unlock();
    video.refresh();
  }
}
