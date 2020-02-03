auto Program::attach(higan::Node::Object node) -> void {
  if(auto screen = node->cast<higan::Node::Screen>()) {
    screens = emulator->root->find<higan::Node::Screen>();
  }

  if(auto stream = node->cast<higan::Node::Stream>()) {
    streams = emulator->root->find<higan::Node::Stream>();
    stream->setResamplerFrequency(48000.0);
  }
}

auto Program::detach(higan::Node::Object node) -> void {
}

auto Program::open(higan::Node::Object node, string name, vfs::file::mode mode, bool required) -> shared_pointer<vfs::file> {
  auto location = node->attribute("location");

  if(name == "manifest.bml") {
    if(auto manifest = execute("icarus", "--system", node->name(), "--manifest", location).output) {
      emulator->game.manifest = manifest;
      auto document = BML::unserialize(manifest);
      return vfs::memory::file::open(manifest.data<uint8_t>(), manifest.size());
    }
  }

  return emulator->open(node, name, mode, required);
}

auto Program::event(higan::Event event) -> void {
}

auto Program::log(string_view message) -> void {
}

auto Program::video(higan::Node::Screen node, const uint32_t* data, uint pitch, uint width, uint height) -> void {
  if(!screens) return;

  uint videoWidth = node->width() * node->scaleX();
  uint videoHeight = node->height() * node->scaleY();
  if(settings.video.aspectCorrection) videoWidth = videoWidth * node->aspectX() / node->aspectY();
  if(node->rotation() == 90 || node->rotation() == 270) swap(videoWidth, videoHeight);

  auto [viewportWidth, viewportHeight] = ruby::video.size();
  uint multiplierX = viewportWidth / videoHeight;
  uint multiplierY = viewportHeight / videoHeight;
  uint multiplier = min(multiplierX, multiplierY);

  uint outputWidth = videoWidth * multiplier;
  uint outputHeight = videoHeight * multiplier;

  if(multiplier == 0 || settings.video.output == "Scale") {
    float multiplierX = (float)viewportWidth / (float)videoWidth;
    float multiplierY = (float)viewportHeight / (float)videoHeight;
    float multiplier = min(multiplierX, multiplierY);

    outputWidth = videoWidth * multiplier;
    outputHeight = videoHeight * multiplier;
  }

  if(settings.video.output == "Stretch") {
    outputWidth = viewportWidth;
    outputHeight = viewportHeight;
  }

  pitch >>= 2;
  if(auto [output, length] = ruby::video.acquire(width, height); output) {
    length >>= 2;
    for(auto y : range(height)) {
      memory::copy<uint32_t>(output + y * length, data + y * pitch, width);
    }
    ruby::video.release();
    ruby::video.output(outputWidth, outputHeight);
  }

  static uint64_t frameCounter = 0, previous, current;
  frameCounter++;

  current = chrono::timestamp();
  if(current != previous) {
    previous = current;
    message.text = {"FPS: ", frameCounter};
    frameCounter = 0;
  }
}

auto Program::audio(higan::Node::Stream node) -> void {
  if(!streams) return;

  //process all pending frames (there may be more than one waiting)
  while(true) {
    //only process a frame if all streams have at least one pending frame
    for(auto& stream : streams) {
      if(!stream->pending()) return;
    }

    //mix all frames together
    double samples[2] = {0.0, 0.0};
    for(auto& stream : streams) {
      double buffer[2];
      uint channels = stream->read(buffer);
      if(channels == 1) {
        //monaural -> stereo mixing
        samples[0] += buffer[0];
        samples[1] += buffer[0];
      } else {
        samples[0] += buffer[0];
        samples[1] += buffer[1];
      }
    }

    //apply volume, balance, and clamping to the output frame
    //TODO
    if(settings.audio.mute) {
      samples[0] = 0.0;
      samples[1] = 0.0;
    }

    //send frame to the audio output device
    ruby::audio.output(samples);
  }
}

auto Program::input(higan::Node::Input node) -> void {
  emulator->input(node);
}
