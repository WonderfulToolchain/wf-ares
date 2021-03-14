auto SH2::serialize(serializer& s) -> void {
  s(branch.pc);
  s(branch.state);
  s(R);
  s(SR.T);
  s(SR.S);
  s(SR.I);
  s(SR.Q);
  s(SR.M);
  s(GBR);
  s(VBR);
  s(MACH);
  s(MACL);
  s(PR);
  s(PC);
}
