Controls controls;

auto Controls::load(Node::Object parent, Node::Object from) -> void {
  up     = Node::append<Node::Button>(parent, from, "Up");
  down   = Node::append<Node::Button>(parent, from, "Down");
  left   = Node::append<Node::Button>(parent, from, "Left");
  right  = Node::append<Node::Button>(parent, from, "Right");
  a      = Node::append<Node::Button>(parent, from, "A");
  b      = Node::append<Node::Button>(parent, from, "B");
  option = Node::append<Node::Button>(parent, from, "Option");
  power  = Node::append<Node::Button>(parent, from, "Power");
}

auto Controls::poll() -> void {
  platform->input(up);
  platform->input(down);
  platform->input(left);
  platform->input(right);
  platform->input(a);
  platform->input(b);
  platform->input(option);

  if(!(up->value & down->value)) {
    yHold = 0, upLatch = up->value, downLatch = down->value;
  } else if(!yHold) {
    yHold = 1, swap(upLatch, downLatch);
  }

  if(!(left->value & right->value)) {
    xHold = 0, leftLatch = left->value, rightLatch = right->value;
  } else if(!xHold) {
    xHold = 1, swap(leftLatch, rightLatch);
  }
}
