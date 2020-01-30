//identifier is static, allowing template<typename T> to access via T::identifier
//identity() is virtual, allowing T* to access via T->identity()

#define DeclareClass(Type, Name) \
  static inline const string identifier = Name; \
  virtual auto identity() const -> string { return Name; } \
  static auto create() -> Node::Object { return new Type; } \
  private: static inline Class::Register<Type> register; public: \

struct Object : shared_pointer_this<Object> {
  DeclareClass(Object, "Object")

  Object(string name = {}) : _name(name) {}
  virtual ~Object() = default;

  inline auto name() const -> string { return _name; }
  inline auto parent() const -> shared_pointer_weak<Object> { return _parent; }

  auto setName(string_view name) -> void { _name = name; }

  auto prepend(Node::Object node) -> Node::Object {
    if(auto found = find(node)) return found;
    _nodes.prepend(node);
    node->_parent = shared();
    PlatformAttach(node);
    return node;
  }

  template<typename T, typename... P>
  auto prepend(P&&... p) -> Node::Object {
    using Type = typename T::type;
    return prepend(shared_pointer<Type>::create(forward<P>(p)...));
  }

  auto append(Node::Object node) -> Node::Object {
    if(auto found = find(node)) return found;
    _nodes.append(node);
    node->_parent = shared();
    PlatformAttach(node);
    return node;
  }

  template<typename T, typename... P>
  auto append(P&&... p) -> Node::Object {
    using Type = typename T::type;
    return append(shared_pointer<Type>::create(forward<P>(p)...));
  }

  auto remove(Node::Object node) -> void {
    if(auto index = _nodes.find(node)) {
      PlatformDetach(node);
      node->reset();
      node->_parent.reset();
      _nodes.remove(*index);
    }
  }

  auto reset() -> void {
    for(auto& node : _nodes) {
      PlatformDetach(node);
      node->reset();
      node->_parent.reset();
    }
    _nodes.reset();
  }

  template<typename T>
  auto cast() -> shared_pointer<typename T::type> {
    if(dynamic_cast<typename T::type*>(this)) return shared();
    return {};
  }

  template<typename T>
  auto is() -> bool {
    return (bool)cast<T>();
  }

  template<typename T>
  auto find() -> vector<shared_pointer<typename T::type>> {
    vector<shared_pointer<typename T::type>> result;
    if(dynamic_cast<typename T::type*>(this)) {
      if(auto instance = shared()) result.append(instance);
    }
    for(auto& node : _nodes) result.append(node->find<T>());
    return result;
  }

  template<typename T>
  auto find(uint index) -> shared_pointer<typename T::type> {
    auto result = find<T>();
    if(index < result.size()) return result[index];
    return {};
  }

  auto find(Node::Object source) -> Node::Object {
    if(!source) return {};
    for(auto& node : _nodes) {
      if(node->identity() == source->identity() && node->_name == source->_name) return node;
    }
    return {};
  }

  template<typename T = Node::Object>
  auto find(string name) -> T {
    using Type = typename T::type;
    for(auto& node : _nodes) {
      if(node->identity() == Type::identifier && node->_name == name) return node;
    }
    return {};
  }

  template<typename T = string>
  auto attribute(const string& name) const -> T {
    if(auto attribute = _attributes.find(name)) {
      if(attribute->value.is<T>()) return attribute->value.get<T>();
    }
    return {};
  }

  template<typename T = string, typename U = string>
  auto setAttribute(const string& name, const U& value = {}) -> void {
    if constexpr(is_same_v<T, string> && !is_same_v<U, string>) return setAttribute(name, string{value});
    if(auto attribute = _attributes.find(name)) {
      if((const T&)value) attribute->value = (const T&)value;
      else _attributes.remove(*attribute);
    } else {
      if((const T&)value) _attributes.insert({name, (const T&)value});
    }
  }

  virtual auto load(Node::Object source) -> bool {
    if(!source || identity() != source->identity() || _name != source->_name) return false;
    _attributes = source->_attributes;
    return true;
  }

  auto save() -> string {
    string markup;
    serialize(markup, {});
    return markup;
  }

  virtual auto serialize(string& output, string depth) -> void {
    output.append(depth, "node: ", identity(), "\n");
    output.append(depth, "  name: ", _name, "\n");
    for(auto& attribute : _attributes) {
      if(!attribute.value.is<string>()) continue;
      output.append(depth, "  attribute\n");
      output.append(depth, "    name: ", attribute.name, "\n");
      output.append(depth, "    value: ", attribute.value.get<string>(), "\n");
    }
    depth.append("  ");
    for(auto& node : _nodes) {
      node->serialize(output, depth);
    }
  }

  virtual auto unserialize(Markup::Node markup) -> void {
    if(!markup) return;
    _name = markup["name"].text();
    _attributes.reset();
    for(auto& attribute : markup.find("attribute")) {
      _attributes.insert({attribute["name"].text(), attribute["value"].text()});
    }
    for(auto& leaf : markup.find("node")) {
      auto node = Class::create(leaf.text());
      append(node);
      node->unserialize(leaf);
    }
  }

  auto begin() { return _nodes.begin(); }
  auto end() { return _nodes.end(); }

protected:
  string _name;
  set<Attribute> _attributes;
  shared_pointer_weak<Object> _parent;
  vector<Node::Object> _nodes;
};
