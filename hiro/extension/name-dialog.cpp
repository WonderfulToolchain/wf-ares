#if defined(Hiro_NameDialog)

NameDialog::NameDialog() {
  layout.setPadding(5);
  typeIcon.setCollapsible();
  nameValue.onActivate([&] { acceptButton.doActivate(); });
  acceptButton.onActivate([&] {
    response = nameValue.text();
    window.doClose();
  });
  cancelButton.setText("Cancel").onActivate([&] { window.doClose(); });

  window.onClose([&] {
    window.setModal(false);
    window.setVisible(false);
  });

  window.setDismissable();
}

auto NameDialog::create() -> string {
  return show("Create");
}

auto NameDialog::rename(string name) -> string {
  return show("Rename", name);
}

auto NameDialog::setAlignment(Alignment alignment) -> type& {
  state.alignment = alignment;
  state.relativeTo = {};
  return *this;
}

auto NameDialog::setAlignment(sWindow relativeTo, Alignment alignment) -> type& {
  state.alignment = alignment;
  state.relativeTo = relativeTo;
  return *this;
}

auto NameDialog::setIcon(const image& icon) -> type& {
  state.icon = icon;
  return *this;
}

auto NameDialog::setText(const string& text) -> type& {
  state.text = text;
  return *this;
}

auto NameDialog::setTitle(const string& title) -> type& {
  state.title = title;
  return *this;
}

auto NameDialog::show(string mode, string name) -> string {
  response = {};
  setTitle(state.title);
  if(!state.title && mode == "Create") setTitle("Create");
  if(!state.title && mode == "Rename") setTitle({"Rename ", name});
  textLabel.setText(state.text ? state.text : "Enter a name:");
  if(state.icon) {
    //todo: enforce 16x16 size?
    typeIcon.setIcon(state.icon);
  } else {
    typeIcon.setVisible(false);
  }
  nameValue.setText(name);
  acceptButton.setText(mode);
  window.setTitle(state.title);
  window.setSize({400, layout.minimumSize().height()});
  window.setAlignment(state.relativeTo, state.alignment);
  window.setVisible();
  nameValue.setFocused();
  window.setModal();
  return response;
}

#endif
