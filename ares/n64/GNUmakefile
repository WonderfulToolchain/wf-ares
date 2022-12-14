ares.components += sm5k

ares.objects += ares-n64-memory
ares.objects += ares-n64-system
ares.objects += ares-n64-cartridge
ares.objects += ares-n64-controller
ares.objects += ares-n64-dd
ares.objects += ares-n64-mi
ares.objects += ares-n64-vi
ares.objects += ares-n64-ai
ares.objects += ares-n64-pi
ares.objects += ares-n64-pif
ares.objects += ares-n64-ri
ares.objects += ares-n64-si
ares.objects += ares-n64-rdram
ares.objects += ares-n64-cpu
ares.objects += ares-n64-rsp
ares.objects += ares-n64-rdp

$(object.path)/ares-n64-memory.o:     $(ares.path)/n64/memory/memory.cpp
$(object.path)/ares-n64-system.o:     $(ares.path)/n64/system/system.cpp
$(object.path)/ares-n64-cartridge.o:  $(ares.path)/n64/cartridge/cartridge.cpp
$(object.path)/ares-n64-controller.o: $(ares.path)/n64/controller/controller.cpp
$(object.path)/ares-n64-dd.o:         $(ares.path)/n64/dd/dd.cpp
$(object.path)/ares-n64-sp.o:         $(ares.path)/n64/sp/sp.cpp
$(object.path)/ares-n64-dp.o:         $(ares.path)/n64/dp/dp.cpp
$(object.path)/ares-n64-mi.o:         $(ares.path)/n64/mi/mi.cpp
$(object.path)/ares-n64-vi.o:         $(ares.path)/n64/vi/vi.cpp
$(object.path)/ares-n64-ai.o:         $(ares.path)/n64/ai/ai.cpp
$(object.path)/ares-n64-pi.o:         $(ares.path)/n64/pi/pi.cpp
$(object.path)/ares-n64-pif.o:        $(ares.path)/n64/pif/pif.cpp
$(object.path)/ares-n64-ri.o:         $(ares.path)/n64/ri/ri.cpp
$(object.path)/ares-n64-si.o:         $(ares.path)/n64/si/si.cpp
$(object.path)/ares-n64-rdram.o:      $(ares.path)/n64/rdram/rdram.cpp
$(object.path)/ares-n64-cpu.o:        $(ares.path)/n64/cpu/cpu.cpp
$(object.path)/ares-n64-rsp.o:        $(ares.path)/n64/rsp/rsp.cpp
$(object.path)/ares-n64-rdp.o:        $(ares.path)/n64/rdp/rdp.cpp

ifeq ($(vulkan),true)
  ifeq ($(filter shortest-stem,${.FEATURES}),)
    $(error Compiling Ares N64 Vulkan backend requires GNU Make 3.82 or later)
  endif
  ifeq ($(platform),macos)
    molten = $(ares.path)/../thirdparty/MoltenVK/libMoltenVK.dylib
    ifeq ($(wildcard $(molten)),)  
      molten = $(shell brew --prefix molten-vk)/lib/libMoltenVK.dylib
      ifeq ($(wildcard $(molten)),)
        $(error Compiling Ares N64 Vulkan backend requires MoltenVK. Install it via Homebrew, compile it using thirdparty/MoltenVK/build-moltenvk.sh, or disable with vulkan=false")
      endif
    endif
    ares.dylibs += $(molten)
  endif
  ares.objects += ares-n64-vulkan
  $(object.path)/ares-n64-vulkan.o: $(ares.path)/n64/vulkan/vulkan.cpp
  PARALLEL_RDP_IMPLEMENTATION := $(ares.path)/n64/vulkan/parallel-rdp
  include $(PARALLEL_RDP_IMPLEMENTATION)/config.mk
  ares.objects += $(PARALLEL_RDP_SOURCES_CXX:$(PARALLEL_RDP_IMPLEMENTATION)/%.cpp=ares-n64-parallel-rdp/%)
  ares.objects += $(PARALLEL_RDP_SOURCES_C:$(PARALLEL_RDP_IMPLEMENTATION)/%.c=ares-n64-parallel-rdp/%)
  flags += $(PARALLEL_RDP_INCLUDE_DIRS) $(PARALLEL_RDP_CXXFLAGS) $(PARALLEL_RDP_CFLAGS)
  $(object.path)/ares-n64-parallel-rdp/%.o: $(PARALLEL_RDP_IMPLEMENTATION)/%.cpp
	$(info Compiling $(subst ../,,$<) ...)
	$(call mkdir,$(dir $@))
	@$(call compile)
  $(object.path)/ares-n64-parallel-rdp/%.o: $(PARALLEL_RDP_IMPLEMENTATION)/%.c
	$(info Compiling $(subst ../,,$<) ...)
	$(call mkdir,$(dir $@))
	@$(call compile)
endif

ifeq ($(mame.rdp),true)
  mame.path := $(thirdparty.path)/mame

  ares.objects += mame-emu-emucore
  ares.objects += mame-emu-video-rgbutil
  ares.objects += mame-lib-util-delegate
  ares.objects += mame-lib-util-strformat
  ares.objects += mame-mame-video-n64
  ares.objects += mame-mame-video-pin64
  ares.objects += mame-mame-video-rdpblend
  ares.objects += mame-mame-video-rdptpipe
  ares.objects += mame-osd-osdcore
  ares.objects += mame-osd-osdsync

  $(object.path)/mame-emu-emucore.o:          $(mame.path)/emu/emucore.cpp
  $(object.path)/mame-emu-video-rgbutil.o:    $(mame.path)/emu/video/rgbutil.cpp
  $(object.path)/mame-lib-util-delegate.o:    $(mame.path)/lib/util/delegate.cpp
  $(object.path)/mame-lib-util-strformat.o:   $(mame.path)/lib/util/strformat.cpp
  $(object.path)/mame-mame-video-n64.o:       $(mame.path)/mame/video/n64.cpp
  $(object.path)/mame-mame-video-pin64.o:     $(mame.path)/mame/video/pin64.cpp
  $(object.path)/mame-mame-video-rdpblend.o:  $(mame.path)/mame/video/rdpblend.cpp
  $(object.path)/mame-mame-video-rdptpipe.o:  $(mame.path)/mame/video/rdptpipe.cpp
  $(object.path)/mame-osd-osdcore.o:          $(mame.path)/osd/osdcore.cpp
  $(object.path)/mame-osd-osdsync.o:          $(mame.path)/osd/osdsync.cpp

  flags += -I$(mame.path)/devices -I$(mame.path)/emu -I$(mame.path)/lib/util -I$(mame.path)/mame -I$(mame.path)/osd
  flags += -DMAME_RDP -DLSB_FIRST -DPTR64
  ifeq ($(platform),windows)
    flags += -DOSD_WINDOWS=1
  else ifeq ($(platform),macos)
    flags += -DSDLMAME_MACOSX
  else ifeq ($(platform),linux)
    flags += -DSDLMAME_LINUX
  else ifeq ($(platform),bsd)
    flags += -DSDLMAME_BSD
  endif
endif
