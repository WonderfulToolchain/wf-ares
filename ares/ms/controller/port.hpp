struct ControllerPort {
  Node::Port port;

  //port.cpp
  ControllerPort(string name);
  auto load(Node::Object) -> void;
  auto unload() -> void;
  auto connected() const -> bool;
  auto connect(Node::Peripheral) -> void;
  auto disconnect() -> void;

  auto read() -> uint8 { if(device) return device->read(); return 0xff; }
  auto write(uint8 data) -> void { if(device) return device->write(data); }

  auto serialize(serializer&) -> void;

  const string name;
  unique_pointer<Controller> device;
};

extern ControllerPort controllerPort1;
extern ControllerPort controllerPort2;
