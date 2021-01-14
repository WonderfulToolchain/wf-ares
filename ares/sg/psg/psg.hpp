struct PSG : SN76489, Thread {
  Node::Object node;
  Node::Audio::Stream stream;

  //psg.cpp
  auto load(Node::Object) -> void;
  auto unload() -> void;

  auto main() -> void;
  auto step(uint clocks) -> void;
  auto power() -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

private:
  double volume[16];
};

extern PSG psg;
