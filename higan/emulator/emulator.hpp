#pragma once

#include <libco/libco.h>

#include <nall/platform.hpp>
#include <nall/adaptive-array.hpp>
#include <nall/any.hpp>
#include <nall/array.hpp>
#include <nall/chrono.hpp>
#include <nall/dl.hpp>
#include <nall/endian.hpp>
#include <nall/image.hpp>
#include <nall/literals.hpp>
#include <nall/random.hpp>
#include <nall/serializer.hpp>
#include <nall/set.hpp>
#include <nall/shared-pointer.hpp>
#include <nall/string.hpp>
#include <nall/traits.hpp>
#include <nall/unique-pointer.hpp>
#include <nall/variant.hpp>
#include <nall/vector.hpp>
#include <nall/vfs.hpp>
#include <nall/cd.hpp>
#include <nall/dsp/iir/one-pole.hpp>
#include <nall/dsp/iir/biquad.hpp>
#include <nall/dsp/resampler/cubic.hpp>
#include <nall/hash/crc32.hpp>
#include <nall/hash/sha256.hpp>
using namespace nall;

#include <emulator/types.hpp>
#include <emulator/node/node.hpp>
#include <emulator/platform.hpp>
#include <emulator/interface.hpp>
#include <emulator/memory/readable.hpp>
#include <emulator/memory/writable.hpp>
#include <emulator/resource/resource.hpp>

namespace higan {
  static const string Name    = "higan";
  static const string Version = "106.224";
  static const string Author  = "byuu";
  static const string License = "GPLv3";
  static const string Website = "https://byuu.org";

  //incremented only when serialization format changes
  static const string SerializerVersion = "106.210";

  namespace Constants {
    namespace Colorburst {
      static constexpr double NTSC = 315.0 / 88.0 * 1'000'000.0;
      static constexpr double PAL  = 283.75 * 15'625.0 + 25.0;
    }
  }

  //nall/vfs shorthand constants for open(), load()
  namespace File {
    static const auto Read  = vfs::file::mode::read;
    static const auto Write = vfs::file::mode::write;
    static const auto Optional = false;
    static const auto Required = true;
  }

  namespace Shared {
    using File = shared_pointer<vfs::file>;
  }
}
