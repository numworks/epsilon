PLATFORM ?= simulator
DEBUG ?= 1
DEVELOPMENT ?= 1
ARCH = arm64

include build/defaults.mak
include build/platform.$(PLATFORM).mak
include build/toolchain.$(TOOLCHAIN).mak

poincare_junior_src += $(addprefix poincare_junior/,\
  interfaces/addition_interface.o \
  interfaces/constant_interface.o \
  interfaces/division_interface.o \
  interfaces/expression_interface.o \
  interfaces/integer_interface.o \
  interfaces/interface.o \
  interfaces/multiplication_interface.o \
  interfaces/n_ary_interface.o \
  interfaces/power_interface.o \
  interfaces/subtraction_interface.o \
  test/calculation.o \
  test/constexpr.o \
  test/edition_pool_overflow.o \
  test/elementary_tree_manipulations.o \
  test/graph.o \
  test/main.o \
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

