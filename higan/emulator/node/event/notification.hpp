struct Notification : Event {
  DeclareClass(Notification, "Notification")

  Notification(string name = {}, string component = {}) : Event(name, component) {
  }

  auto notify(string_view message = {}) -> void {
    if(message) {
      PlatformLog({_component, " [", name, ": ", message, "]\n"});
    } else {
      PlatformLog({_component, " [", name, "]\n"});
    }
  }

protected:
};
