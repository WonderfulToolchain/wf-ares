auto X24C01::serialize(serializer& s) -> void {
  s(memory);
  s(clock.lo);
  s(clock.hi);
  s(clock.fall);
  s(clock.rise);
  s(clock.line);
  s(data.lo);
  s(data.hi);
  s(data.fall);
  s(data.rise);
  s(data.line);
  s((uint&)mode);
  s(counter);
  s(address);
  s(input);
  s(output);
  s(line);
}
