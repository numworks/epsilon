PLATFORM ?= simulator
DEBUG ?= 1
DEVELOPMENT ?= 1
ARCH = arm64
ASSERTIONS = 1

include build/defaults.mak
include build/platform.$(PLATFORM).mak
include build/toolchain.$(TOOLCHAIN).mak

include poincare_junior/utils/Makefile

poincare_junior_src += $(utils_src)

poincare_junior_src += $(addprefix poincare_junior/src/,\
  expression/approximation.o \
  expression/integer.o \
  expression/rational.o \
  expression/simplification.o \
  expression.o \
  memory/cache_pool.o \
  memory/cache_reference.o \
  memory/edition_pool.o \
  memory/edition_reference.o \
  memory/exception_checkpoint.o \
  memory/node.o \
  memory/pool.o \
)

poincare_junior_src += $(addprefix poincare_junior/test/,\
  calculation.o \
  constexpr.o \
  edition_pool_overflow.o \
  elementary_tree_manipulations.o \
  graph.o \
  main.o \
  node_iterator.o \
)
poincare_junior_src += ion/src/simulator/shared/crc32.o
poincare_junior_src += ion/src/shared/crc32_eat_byte.o

SFLAGS += -DASSERTIONS=$(ASSERTIONS) -Iion/include/ -Ikandinsky/include

junior.elf: $(poincare_junior_src)
	$(LD) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f poincare_junior/*/*/*/*/*.o
	rm -f poincare_junior/*/*/*/*/*.d
	rm -f poincare_junior/*/*/*/*.o
	rm -f poincare_junior/*/*/*/*.d
	rm -f poincare_junior/*/*/*.o
	rm -f poincare_junior/*/*/*.d
	rm -f poincare_junior/*/*.o
	rm -f poincare_junior/*/*.d
	rm -f poincare_junior/*.o
	rm -f poincare_junior/*.d
	rm -f junior.elf

