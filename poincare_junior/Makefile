PLATFORM ?= simulator
DEBUG ?= 1
DEVELOPMENT ?= 1
ARCH = arm64
ASSERTIONS = 1

include build/defaults.mak
include build/platform.$(PLATFORM).mak
include build/toolchain.$(TOOLCHAIN).mak

poincare_junior_src += $(addprefix poincare_junior/,\
  interfaces/addition.o \
  interfaces/constant.o \
  interfaces/division.o \
  interfaces/expression.o \
  interfaces/integer.o \
  interfaces/interface.o \
  interfaces/multiplication.o \
  interfaces/n_ary.o \
  interfaces/power.o \
  interfaces/subtraction.o \
  test/calculation.o \
  test/constexpr.o \
  test/edition_pool_overflow.o \
  test/elementary_tree_manipulations.o \
  test/graph.o \
  test/main.o \
  test/node_iterator.o \
  cache_pool.o \
  cache_reference.o \
  edition_pool.o \
  edition_reference.o \
  exception_checkpoint.o \
  expression.o \
  node.o \
  pool.o \
  utils.o \
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
	rm -f poincare_junior/*/*.o
	rm -f poincare_junior/*/*.d
	rm -f poincare_junior/*.o
	rm -f poincare_junior/*.d
	rm -f junior.elf

