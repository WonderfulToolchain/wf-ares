#pragma once
//started: 2020-04-28

#include <ares/ares.hpp>
#include <nmmintrin.h>
using s128 = __m128i;

namespace ares::Nintendo64 {
  struct Accuracy {
    //enable all accuracy flags
    static constexpr bool Reference = 0;

    //emulate the CPU translation lookaside buffer
    static constexpr bool TLB = 0 | Reference;

    //exceptions when the CPU accesses unaligned memory addresses
    static constexpr bool AddressErrors = 0 | Reference;
  };

  struct Thread {
    auto reset() -> void {
      clock = 0;
    }

    auto serialize(serializer& s) -> void {
      s.integer(clock);
    }

    i64 clock;
  };

  template<uint lo, uint hi, typename T>
  struct bitref {
    static constexpr T mask = ~0ull >> 64 - (hi - lo + 1) << lo;
    bitref(T& data) : data(data) {}
    operator T() const { return (data & mask) >> lo; }
    auto& operator=(T value) { return data = data & ~mask | value << lo & mask, *this; }
    auto& operator=(const bitref& value) { return operator=((T)value); }
    T& data;
  };

  template<uint lo, typename T> auto bit(T& value) { return bitref<lo, lo, T>(value); }
  template<uint lo, uint hi, typename T> auto bit(T& value) { return bitref<lo, hi, T>(value); }

  #include <n64/memory/memory.hpp>
  #include <n64/system/system.hpp>
  #include <n64/cartridge/cartridge.hpp>
  #include <n64/controller/controller.hpp>
  #include <n64/mi/mi.hpp>
  #include <n64/vi/vi.hpp>
  #include <n64/ai/ai.hpp>
  #include <n64/pi/pi.hpp>
  #include <n64/ri/ri.hpp>
  #include <n64/si/si.hpp>
  #include <n64/rdram/rdram.hpp>
  #include <n64/cpu/cpu.hpp>
  #include <n64/rdp/rdp.hpp>
  #include <n64/rsp/rsp.hpp>
  #include <n64/memory/bus.hpp>
}

#include <n64/interface/interface.hpp>
