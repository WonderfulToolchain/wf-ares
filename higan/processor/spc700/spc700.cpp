#include <processor/processor.hpp>
#include "spc700.hpp"

namespace Processor {

#include "algorithms.cpp"
#include "instructions.cpp"
#include "disassembler.cpp"
#include "serialization.cpp"

#define op(id, name, ...)     case id: return op_##name(__VA_ARGS__);
#define ol(id, name, fp)      case id: return op_##name(&SPC700::op_##fp);
#define of(id, name, fp, ...) case id: return op_##name(&SPC700::op_##fp, __VA_ARGS__);

auto SPC700::instruction() -> void {
  switch(opcode = readPC()) {
  op(0x00, nop)
  op(0x01, jst)
  op(0x02, set_bit);
  op(0x03, branch_bit);
  of(0x04, read_dp, or, regs.a)
  of(0x05, read_addr, or, regs.a)
  ol(0x06, read_ix, or);
  ol(0x07, read_idpx, or)
  of(0x08, read_const, or, regs.a)
  ol(0x09, write_dp_dp, or)
  op(0x0a, set_addr_bit)
  ol(0x0b, adjust_dp, asl)
  ol(0x0c, adjust_addr, asl)
  op(0x0d, push, regs.p)
  op(0x0e, test_addr, 1)
  op(0x0f, brk)
  op(0x10, branch, regs.p.n == 0)
  op(0x11, jst)
  op(0x12, set_bit)
  op(0x13, branch_bit);
  of(0x14, read_dpi, or, regs.a, regs.x)
  of(0x15, read_addri, or, regs.x)
  of(0x16, read_addri, or, regs.y)
  ol(0x17, read_idpy, or)
  ol(0x18, write_dp_const, or)
  ol(0x19, write_ix_iy, or)
  op(0x1a, adjust_dpw, -1)
  ol(0x1b, adjust_dpx, asl)
  of(0x1c, adjust, asl, regs.a)
  of(0x1d, adjust, dec, regs.x)
  of(0x1e, read_addr, cmp, regs.x)
  op(0x1f, jmp_iaddrx)
  op(0x20, set_flag, regs.p.p, 0)
  op(0x21, jst)
  op(0x22, set_bit)
  op(0x23, branch_bit)
  of(0x24, read_dp, and, regs.a)
  of(0x25, read_addr, and, regs.a)
  ol(0x26, read_ix, and)
  ol(0x27, read_idpx, and)
  of(0x28, read_const, and, regs.a)
  ol(0x29, write_dp_dp, and)
  op(0x2a, set_addr_bit)
  ol(0x2b, adjust_dp, rol)
  ol(0x2c, adjust_addr, rol)
  op(0x2d, push, regs.a)
  op(0x2e, bne_dp)
  op(0x2f, branch, true)
  op(0x30, branch, regs.p.n == 1)
  op(0x31, jst)
  op(0x32, set_bit)
  op(0x33, branch_bit)
  of(0x34, read_dpi, and, regs.a, regs.x)
  of(0x35, read_addri, and, regs.x)
  of(0x36, read_addri, and, regs.y)
  ol(0x37, read_idpy, and)
  ol(0x38, write_dp_const, and)
  ol(0x39, write_ix_iy, and)
  op(0x3a, adjust_dpw, +1)
  ol(0x3b, adjust_dpx, rol)
  of(0x3c, adjust, rol, regs.a)
  of(0x3d, adjust, inc, regs.x)
  of(0x3e, read_dp, cmp, regs.x)
  op(0x3f, jsr_addr)
  op(0x40, set_flag, regs.p.p, 1)
  op(0x41, jst)
  op(0x42, set_bit)
  op(0x43, branch_bit)
  of(0x44, read_dp, eor, regs.a)
  of(0x45, read_addr, eor, regs.a)
  ol(0x46, read_ix, eor)
  ol(0x47, read_idpx, eor)
  of(0x48, read_const, eor, regs.a)
  ol(0x49, write_dp_dp, eor)
  op(0x4a, set_addr_bit)
  ol(0x4b, adjust_dp, lsr)
  ol(0x4c, adjust_addr, lsr)
  op(0x4d, push, regs.x)
  op(0x4e, test_addr, 0)
  op(0x4f, jsp_dp)
  op(0x50, branch, regs.p.v == 0)
  op(0x51, jst)
  op(0x52, set_bit)
  op(0x53, branch_bit)
  of(0x54, read_dpi, eor, regs.a, regs.x)
  of(0x55, read_addri, eor, regs.x)
  of(0x56, read_addri, eor, regs.y)
  ol(0x57, read_idpy, eor)
  ol(0x58, write_dp_const, eor)
  ol(0x59, write_ix_iy, eor)
  ol(0x5a, read_dpw, cpw)
  ol(0x5b, adjust_dpx, lsr)
  of(0x5c, adjust, lsr, regs.a)
  op(0x5d, transfer, regs.a, regs.x)
  of(0x5e, read_addr, cmp, regs.y)
  op(0x5f, jmp_addr)
  op(0x60, set_flag, regs.p.c, 0)
  op(0x61, jst)
  op(0x62, set_bit)
  op(0x63, branch_bit)
  of(0x64, read_dp, cmp, regs.a)
  of(0x65, read_addr, cmp, regs.a)
  ol(0x66, read_ix, cmp)
  ol(0x67, read_idpx, cmp)
  of(0x68, read_const, cmp, regs.a)
  ol(0x69, write_dp_dp, cmp)
  op(0x6a, set_addr_bit)
  ol(0x6b, adjust_dp, ror)
  ol(0x6c, adjust_addr, ror)
  op(0x6d, push, regs.y)
  op(0x6e, bne_dpdec)
  op(0x6f, rts)
  op(0x70, branch, regs.p.v == 1)
  op(0x71, jst)
  op(0x72, set_bit)
  op(0x73, branch_bit)
  of(0x74, read_dpi, cmp, regs.a, regs.x)
  of(0x75, read_addri, cmp, regs.x)
  of(0x76, read_addri, cmp, regs.y)
  ol(0x77, read_idpy, cmp)
  ol(0x78, write_dp_const, cmp)
  ol(0x79, write_ix_iy, cmp)
  ol(0x7a, read_dpw, adw)
  ol(0x7b, adjust_dpx, ror)
  of(0x7c, adjust, ror, regs.a)
  op(0x7d, transfer, regs.x, regs.a)
  of(0x7e, read_dp, cmp, regs.y)
  op(0x7f, rti)
  op(0x80, set_flag, regs.p.c, 1)
  op(0x81, jst)
  op(0x82, set_bit)
  op(0x83, branch_bit)
  of(0x84, read_dp, adc, regs.a)
  of(0x85, read_addr, adc, regs.a)
  ol(0x86, read_ix, adc)
  ol(0x87, read_idpx, adc)
  of(0x88, read_const, adc, regs.a)
  ol(0x89, write_dp_dp, adc)
  op(0x8a, set_addr_bit)
  ol(0x8b, adjust_dp, dec)
  ol(0x8c, adjust_addr, dec)
  of(0x8d, read_const, ld, regs.y)
  op(0x8e, plp)
  ol(0x8f, write_dp_const, st)
  op(0x90, branch, regs.p.c == 0)
  op(0x91, jst)
  op(0x92, set_bit)
  op(0x93, branch_bit)
  of(0x94, read_dpi, adc, regs.a, regs.x)
  of(0x95, read_addri, adc, regs.x)
  of(0x96, read_addri, adc, regs.y)
  ol(0x97, read_idpy, adc)
  ol(0x98, write_dp_const, adc)
  ol(0x99, write_ix_iy, adc)
  ol(0x9a, read_dpw, sbw)
  ol(0x9b, adjust_dpx, dec)
  of(0x9c, adjust, dec, regs.a)
  op(0x9d, transfer, regs.s, regs.x)
  op(0x9e, div_ya_x)
  op(0x9f, xcn)
  op(0xa0, set_flag, regs.p.i, 1)
  op(0xa1, jst)
  op(0xa2, set_bit)
  op(0xa3, branch_bit)
  of(0xa4, read_dp, sbc, regs.a)
  of(0xa5, read_addr, sbc, regs.a)
  ol(0xa6, read_ix, sbc)
  ol(0xa7, read_idpx, sbc)
  of(0xa8, read_const, sbc, regs.a)
  ol(0xa9, write_dp_dp, sbc)
  op(0xaa, set_addr_bit)
  ol(0xab, adjust_dp, inc)
  ol(0xac, adjust_addr, inc)
  of(0xad, read_const, cmp, regs.y)
  op(0xae, pull, regs.a)
  op(0xaf, sta_ixinc)
  op(0xb0, branch, regs.p.c == 1)
  op(0xb1, jst)
  op(0xb2, set_bit)
  op(0xb3, branch_bit)
  of(0xb4, read_dpi, sbc, regs.a, regs.x)
  of(0xb5, read_addri, sbc, regs.x)
  of(0xb6, read_addri, sbc, regs.y)
  ol(0xb7, read_idpy, sbc)
  ol(0xb8, write_dp_const, sbc)
  ol(0xb9, write_ix_iy, sbc)
  ol(0xba, read_dpw, ldw)
  ol(0xbb, adjust_dpx, inc)
  of(0xbc, adjust, inc, regs.a)
  op(0xbd, transfer, regs.x, regs.s)
  op(0xbe, das)
  op(0xbf, lda_ixinc)
  op(0xc0, set_flag, regs.p.i, 0)
  op(0xc1, jst)
  op(0xc2, set_bit)
  op(0xc3, branch_bit)
  op(0xc4, write_dp, regs.a)
  op(0xc5, write_addr, regs.a)
  op(0xc6, sta_ix)
  op(0xc7, sta_idpx)
  of(0xc8, read_const, cmp, regs.x)
  op(0xc9, write_addr, regs.x)
  op(0xca, set_addr_bit)
  op(0xcb, write_dp, regs.y)
  op(0xcc, write_addr, regs.y)
  of(0xcd, read_const, ld, regs.x)
  op(0xce, pull, regs.x)
  op(0xcf, mul_ya)
  op(0xd0, branch, regs.p.z == 0)
  op(0xd1, jst)
  op(0xd2, set_bit)
  op(0xd3, branch_bit)
  op(0xd4, write_dpi, regs.a, regs.x)
  op(0xd5, write_addri, regs.x)
  op(0xd6, write_addri, regs.y)
  op(0xd7, sta_idpy)
  op(0xd8, write_dp, regs.x)
  op(0xd9, write_dpi, regs.x, regs.y)
  op(0xda, stw_dp)
  op(0xdb, write_dpi, regs.y, regs.x)
  of(0xdc, adjust, dec, regs.y)
  op(0xdd, transfer, regs.y, regs.a)
  op(0xde, bne_dpx)
  op(0xdf, daa)
  op(0xe0, clv)
  op(0xe1, jst)
  op(0xe2, set_bit)
  op(0xe3, branch_bit)
  of(0xe4, read_dp, ld, regs.a)
  of(0xe5, read_addr, ld, regs.a)
  ol(0xe6, read_ix, ld)
  ol(0xe7, read_idpx, ld)
  of(0xe8, read_const, ld, regs.a)
  of(0xe9, read_addr, ld, regs.x)
  op(0xea, set_addr_bit)
  of(0xeb, read_dp, ld, regs.y)
  of(0xec, read_addr, ld, regs.y)
  op(0xed, cmc)
  op(0xee, pull, regs.y)
  op(0xef, wait)
  op(0xf0, branch, regs.p.z == 1)
  op(0xf1, jst)
  op(0xf2, set_bit)
  op(0xf3, branch_bit)
  of(0xf4, read_dpi, ld, regs.a, regs.x)
  of(0xf5, read_addri, ld, regs.x)
  of(0xf6, read_addri, ld, regs.y)
  ol(0xf7, read_idpy, ld)
  of(0xf8, read_dp, ld, regs.x)
  of(0xf9, read_dpi, ld, regs.x, regs.y)
  ol(0xfa, write_dp_dp, st)
  of(0xfb, read_dpi, ld, regs.y, regs.x)
  of(0xfc, adjust, inc, regs.y)
  op(0xfd, transfer, regs.a, regs.y)
  op(0xfe, bne_ydec)
  op(0xff, wait)
  }
}

#undef op
#undef ol
#undef of

}
