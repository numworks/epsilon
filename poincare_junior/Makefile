PLATFORM ?= simulator
DEBUG ?= 1
DEVELOPMENT ?= 1
ARCH = arm64

include build/defaults.mak
include build/platform.$(PLATFORM).mak
include build/toolchain.$(TOOLCHAIN).mak

poincare_junior_src += $(addprefix poincare_junior/,\
  cached_tree.o \
  exception_checkpoint.o \
  helpers.o \
  handle.o \
  tree_block.o \
  tree_cache.o \
  tree_pool.o \
  tree_sandbox.o \
  test/elementary_tree_manipulations.o \
  test/main.o \
  test/sandbox_overflow.o \
)

junior.elf: $(poincare_junior_src)
	$(LD) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f poincare_junior/*/*.o
	rm -f poincare_junior/*/*.d
	rm -f poincare_junior/*.o
	rm -f poincare_junior/*.d
	rm -f junior.elf

