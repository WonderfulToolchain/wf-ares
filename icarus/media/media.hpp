struct Media {
  virtual ~Media() {}
  virtual auto construct() -> void;
  virtual auto read(string filename) -> vector<uint8_t>;
  virtual auto type() -> string = 0;
  virtual auto name() -> string = 0;
  virtual auto metadata(string location) -> string = 0;
  virtual auto import(string filename) -> string = 0;

  Markup::Node database;
  string pathname;
};
