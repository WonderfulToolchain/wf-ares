#include "../byuu.hpp"
#include "video.cpp"
#include "audio.cpp"
#include "input.cpp"
#include "hotkeys.cpp"
#include "drivers.cpp"

Settings settings;
namespace Instances { Instance<SettingsWindow> settingsWindow; }
SettingsWindow& settingsWindow = Instances::settingsWindow();
VideoSettings& videoSettings = settingsWindow.videoSettings;
AudioSettings& audioSettings = settingsWindow.audioSettings;
InputSettings& inputSettings = settingsWindow.inputSettings;
HotkeySettings& hotkeySettings = settingsWindow.hotkeySettings;
DriverSettings& driverSettings = settingsWindow.driverSettings;

auto Settings::load() -> void {
  Markup::Node::operator=(BML::unserialize(string::read(locate("settings.bml")), " "));
  process(true);
  save();
}

auto Settings::save() -> void {
  process(false);
  file::write(locate("settings.bml"), BML::serialize(*this, " "));
}

auto Settings::process(bool load) -> void {
  if(load) {
    //initialize non-static default settings
    video.driver = ruby::Video::optimalDriver();
    audio.driver = ruby::Audio::optimalDriver();
    input.driver = ruby::Input::optimalDriver();
  }

  #define bind(type, path, name) \
    if(load) { \
      if(auto node = operator[](path)) name = node.type(); \
    } else { \
      operator()(path).setValue(name); \
    } \

  bind(string,  "Video/Driver", video.driver);
  bind(string,  "Video/Monitor", video.monitor);
  bind(string,  "Video/Format", video.format);
  bind(boolean, "Video/Exclusive", video.exclusive);
  bind(boolean, "Video/Blocking", video.blocking);
  bind(boolean, "Video/Flush", video.flush);
  bind(string,  "Video/Shader", video.shader);
  bind(natural, "Video/Multiplier", video.multiplier);
  bind(string,  "Video/Output", video.output);
  bind(boolean, "Video/AspectCorrection", video.aspectCorrection);
  bind(boolean, "Video/AdaptiveSizing", video.adaptiveSizing);
  bind(boolean, "Video/AutoCentering", video.autoCentering);
  bind(real,    "Video/Luminance", video.luminance);
  bind(real,    "Video/Saturation", video.saturation);
  bind(real,    "Video/Gamma", video.gamma);

  bind(string,  "Audio/Driver", audio.driver);
  bind(string,  "Audio/Device", audio.device);
  bind(natural, "Audio/Frequency", audio.frequency);
  bind(natural, "Audio/Latency", audio.latency);
  bind(boolean, "Audio/Exclusive", audio.exclusive);
  bind(boolean, "Audio/Blocking", audio.blocking);
  bind(boolean, "Audio/Dynamic", audio.dynamic);
  bind(boolean, "Audio/Mute", audio.mute);
  bind(real,    "Audio/Volume", audio.volume);
  bind(real,    "Audio/Balance", audio.balance);

  bind(string,  "Input/Driver", input.driver);
  bind(string,  "Input/Defocus", input.defocus);

  bind(boolean, "General/ShowStatusBar", general.showStatusBar);

  for(auto& mapping : virtualPad.mappings) {
    string name = {"VirtualPad/", mapping->name};
    bind(string, name, mapping->assignment);
  }

  for(auto& mapping : inputManager.hotkeys) {
    string name = {"Hotkey/", string{mapping.name}.replace(" ", "")};
    bind(string, name, mapping.assignment);
  }

  for(auto& emulator : emulators) {
    string name = {string{emulator->name}.replace(" ", ""), "/Game/Path"};
    bind(string, name, emulator->settings.gamePath);
  }

  #undef bind
}

//

SettingsWindow::SettingsWindow() {
  layout.setPadding(5);

  panelList.append(ListViewItem().setText("Video").setIcon(Icon::Device::Display));
  panelList.append(ListViewItem().setText("Audio").setIcon(Icon::Device::Speaker));
  panelList.append(ListViewItem().setText("Input").setIcon(Icon::Device::Joypad));
  panelList.append(ListViewItem().setText("Hotkeys").setIcon(Icon::Device::Keyboard));
  panelList.append(ListViewItem().setText("Drivers").setIcon(Icon::Place::Settings));
  panelList.onChange([&] { eventChange(); });

  panelContainer.append(videoSettings, Size{~0, ~0});
  panelContainer.append(audioSettings, Size{~0, ~0});
  panelContainer.append(inputSettings, Size{~0, ~0});
  panelContainer.append(hotkeySettings, Size{~0, ~0});
  panelContainer.append(driverSettings, Size{~0, ~0});

  videoSettings.construct();
  audioSettings.construct();
  inputSettings.construct();
  hotkeySettings.construct();
  driverSettings.construct();

  statusBar.setFont(Font().setBold());

  setDismissable();
  setTitle("Configuration");
  setSize({720, 400});
  setAlignment({0.0, 1.0});
}

auto SettingsWindow::show(const string& panel) -> void {
  for(auto item : panelList.items()) {
    if(item.text() == panel) {
      item.setSelected();
      eventChange();
      break;
    }
  }
  setVisible();
  setFocused();
}

auto SettingsWindow::eventChange() -> void {
  videoSettings.setVisible(false);
  audioSettings.setVisible(false);
  inputSettings.setVisible(false);
  hotkeySettings.setVisible(false);
  driverSettings.setVisible(false);

  if(auto item = panelList.selected()) {
    if(item.text() == "Video"  ) videoSettings.setVisible();
    if(item.text() == "Audio"  ) audioSettings.setVisible();
    if(item.text() == "Input"  ) inputSettings.setVisible();
    if(item.text() == "Hotkeys") hotkeySettings.setVisible();
    if(item.text() == "Drivers") driverSettings.setVisible();
  }

  panelContainer.resize();
}
