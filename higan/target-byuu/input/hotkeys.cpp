auto InputManager::createHotkeys() -> void {
  static bool fastForwardVideoBlocking;
  static bool fastForwardAudioBlocking;
  static bool fastForwardAudioDynamic;

  hotkeys.append(InputHotkey("Toggle Fullscreen").onPress([&] {
    program.videoFullscreenToggle();
  }));

  hotkeys.append(InputHotkey("Fast Forward").onPress([&] {
    if(!emulator) return;
    fastForwardVideoBlocking = ruby::video.blocking();
    fastForwardAudioBlocking = ruby::audio.blocking();
    fastForwardAudioDynamic  = ruby::audio.dynamic();
    ruby::video.setBlocking(false);
    ruby::audio.setBlocking(false);
    ruby::audio.setDynamic(false);
  }).onRelease([&] {
    if(!emulator) return;
    ruby::video.setBlocking(fastForwardVideoBlocking);
    ruby::audio.setBlocking(fastForwardAudioBlocking);
    ruby::audio.setDynamic(fastForwardAudioDynamic);
  }));

  hotkeys.append(InputHotkey("Save State").onPress([&] {
    if(!emulator) return;
    program.stateSave(program.state.slot);
  }));

  hotkeys.append(InputHotkey("Load State").onPress([&] {
    if(!emulator) return;
    program.stateLoad(program.state.slot);
  }));

  hotkeys.append(InputHotkey("Decrement State Slot").onPress([&] {
    if(!emulator) return;
    if(program.state.slot == 1) program.state.slot = 9;
    else program.state.slot--;
    program.showMessage({"Selected state slot ", program.state.slot});
  }));

  hotkeys.append(InputHotkey("Increment State Slot").onPress([&] {
    if(!emulator) return;
    if(program.state.slot == 9) program.state.slot = 1;
    else program.state.slot++;
    program.showMessage({"Selected state slot ", program.state.slot});
  }));

  hotkeys.append(InputHotkey("Pause Emulation").onPress([&] {
    if(!emulator) return;
    program.pause(!program.paused);
  }));

  hotkeys.append(InputHotkey("Quit Emulator").onPress([&] {
    program.quit();
  }));
}

auto InputManager::pollHotkeys() -> void {
  if(Application::modal() || !presentation.focused()) return;

  for(auto& hotkey : hotkeys) {
    auto state = hotkey.value();
    if(hotkey.state == 0 && state == 1 && hotkey.press) hotkey.press();
    if(hotkey.state == 1 && state == 0 && hotkey.release) hotkey.release();
    hotkey.state = state;
  }
}
