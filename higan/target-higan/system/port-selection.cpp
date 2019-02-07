PortSelectionPanel::PortSelectionPanel(HorizontalLayout* parent, Size size) : VerticalLayout(parent, size) {
  nodeList.setBackgroundColor(Theme::BackgroundColor);
  nodeList.setForegroundColor(Theme::ForegroundColor);
  nodeList.onChange([&] {
    eventChange();
  });
  nodeList.onContext([&] {
    if(auto item = nodeList.selected(); item.property("type") == "object") {
      contextMenu.setVisible();
    }
  });
  nodeList.onActivate([&] {
    eventAccept();
  });
  nameLabel.setText("Name:");
  nameValue.setBackgroundColor(Theme::BackgroundColor);
  nameValue.setForegroundColor(Theme::ForegroundColor);
  nameValue.onActivate([&] {
    eventAccept();
  });
  acceptButton.setText("Connect").onActivate([&] {
    eventAccept();
  });

  renameAction.setIcon(Icon::Application::TextEditor).setText("Rename ...").onActivate([&] {
    if(auto item = nodeList.selected(); item.property("type") == "object") {
      auto name = item.cell(0).text();
      if(auto rename = NameDialog()
      .setAlignment(systemManager)
      .rename(item.cell(0).text()
      )) {
        if(rename != name) {
          string source = item.property("location");
          string target = {Location::dir(source), rename};
          if(directory::exists(target)) return (void)MessageDialog()
          .setTitle("Error")
          .setText("A directory by the chosen name already exists.")
          .setAlignment(systemManager)
          .error();

          //todo: handle case where this renames a currently selected port node in the tree
          directory::rename(source, target);
          refresh();
        }
      }
    }
  });

  removeAction.setIcon(Icon::Action::Remove).setText("Delete ...").onActivate([&] {
    if(auto item = nodeList.selected(); item.property("type") == "object") {
      if(MessageDialog()
      .setTitle("Warning")
      .setText("Are you sure you want to delete this entry?\n"
               "All data it contains will be permanently lost!")
      .setAlignment(systemManager)
      .question() == "No") return;

      //todo: handle case where this removes a currently selected port node in the tree
      directory::remove(item.property("location"));
      refresh();
    }
  });
}

auto PortSelectionPanel::show(higan::Node::Port port) -> void {
  this->port = port;
  root = interface->root();
  refresh();
  setVisible();
}

auto PortSelectionPanel::hide() -> void {
  setVisible(false);
  nodeList.reset();
  port = {};
  root = {};
}

auto PortSelectionPanel::refresh() -> void {
  nodeList.reset();
  nodeList.append(TableViewColumn().setExpandable());

  { TableViewItem item{&nodeList};
    item.setProperty("type", "nothing");
    TableViewCell cell{&item};
    cell.setIcon(Icon::Action::Remove).setText("Nothing");
  }

  if(string location = {emulator.system.templates, port->type, "/"}) {
    for(auto& name : directory::folders(location)) {
      TableViewItem item{&nodeList};
      item.setProperty("type", "template");
      item.setProperty("location", {location, name});
      TableViewCell cell{&item};
      cell.setIcon(Icon::Emblem::FolderTemplate).setText(name.trimRight("/", 1L));
    }
  }

  auto location = port->property("location");
  if(!location) location = {emulator.system.data, port->type, "/"};
  if(location) {
    for(auto& name : directory::folders(location)) {
      TableViewItem item{&nodeList};
      item.setProperty("type", "object");
      item.setProperty("location", {location, name});
      TableViewCell cell{&item};
      cell.setIcon(Icon::Emblem::Folder).setText(name.trimRight("/", 1L));
    }
  }

  nodeList.doChange();
}

auto PortSelectionPanel::eventChange() -> void {
  if(auto item = nodeList.selected()) {
    bool isTemplate = item.property("type") == "template";
    nameLabel.setVisible(isTemplate);
    nameValue.setText(item.cell(0).text()).setVisible(isTemplate);
    acceptButton.setEnabled(true);
  } else {
    nameLabel.setVisible(false);
    nameValue.setText().setVisible(false);
    acceptButton.setEnabled(false);
  }
}

auto PortSelectionPanel::eventAccept() -> void {
  if(auto item = nodeList.selected()) {
    if(!port->hotSwappable && emulator.system.power) {
      auto response = MessageDialog()
      .setTitle("Warning")
      .setText("The peripheral currently connected to this port isn't hot swappable.\n"
               "Removing it may crash the emulated system.\n"
               "What would you like to do?")
      .setAlignment(systemManager)
      .question({"Force", "Power Off", "Cancel"});
      if(response == "Cancel") return;
      if(response == "Power Off") emulator.power(false);
    }

    auto name = item.cell(0).text();
    auto label = nameValue.text().strip();

    if(item.property("type") == "nothing") {
      port->disconnect();
      systemManager.refresh();
    }

    if(item.property("type") == "template" && label) {
      auto source = item.property("location");
      auto target = string{emulator.system.data, port->type, "/", label, "/"};
      if(emulator.connected(target)) return (void)MessageDialog()
      .setText("This peripheral is already connected to a port.")
      .setAlignment(systemManager)
      .information();

      if(directory::exists(target)) {
        if(MessageDialog()
        .setTitle("Warning")
        .setText("A folder by this name already exists.\n"
                 "Do you want to erase it and create a new folder?\n"
                 "If you choose yes, all contents of the original folder will be lost!")
        .setAlignment(systemManager)
        .question() == "No") return;
        directory::remove(target);
      }

      if(directory::copy(source, target)) {
        file::write({target, "identity.bml"}, string{
          "system: ", root->name, "\n",
          "  type: ", port->type, "\n",
          "  name: ", name, "\n"
        });
        port->connect(name, [&](auto node) {
          node->setProperty("location", target);
        });
        systemManager.refresh();
      }
    }

    if(item.property("type") == "object") {
      auto location = item.property("location");
      if(emulator.connected(location)) return (void)MessageDialog()
      .setText("This peripheral is already connected to a port.")
      .setAlignment(systemManager)
      .information();

      if(auto document = BML::unserialize(file::read({location, "identity.bml"}))) {
        if(auto node = document["system/name"]) name = node.text();
      }
      port->connect(name, [&](auto node) {
        node->setProperty("location", location);
      });
      systemManager.refresh();
    }
  }

  hide();
}
