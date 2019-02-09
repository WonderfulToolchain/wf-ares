struct SystemCreation : Panel {
  SystemCreation(View*);
  auto show() -> void override;
  auto hide() -> void override;
  auto refresh() -> void;

  auto eventChange() -> void;
  auto eventAccept() -> void;

  ListView systemList{this, Size{~0, ~0}};
  HorizontalLayout controlLayout{this, Size{~0, 0}};
    Label nameLabel{&controlLayout, Size{0, 0}};
    LineEdit nameValue{&controlLayout, Size{~0, 0}};
    Button createButton{&controlLayout, Size{80, 0}};
};
