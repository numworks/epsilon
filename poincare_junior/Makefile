PLATFORM ?= simulator
DEBUG ?= 1
DEVELOPMENT ?= 1
ARCH = arm64
ASSERTIONS = 1

include poincare_junior/omgpj/Makefile

poincare_junior_src += $(omgpj_src)

poincare_junior_src += $(addprefix poincare_junior/src/,\
  expression/algebraic.o \
  expression/approximation.o \
  expression/comparison.o \
  expression/integer.o \
  expression/polynomial.o \
  expression/rational.o \
  expression/simplification.o \
  expression/set.o \
  expression/symbol.o \
  expression.o \
  memory/cache_pool.o \
  memory/cache_reference.o \
  memory/edition_pool.o \
  memory/edition_reference.o \
  memory/exception_checkpoint.o \
  memory/node.o \
  memory/pool.o \
  n_ary.o \
)

poincare_junior_src += $(addprefix poincare_junior/test/,\
  calculation.o \
  elementary_tree_manipulations.o \
  expression_comparison.o \
  graph.o \
  main.o \
  memory_elements.o \
  memory_edition.o \
  memory_cache.o \
  polynomial.o \
  set.o \
)
poincare_junior_src += ion/src/simulator/shared/crc32.o
poincare_junior_src += ion/src/shared/crc32_eat_byte.o

tests_src += $(addprefix poincare_junior/src/,\
  expression/approximation.cpp \
  expression/integer.cpp \
  expression/rational.cpp \
  expression/simplification.cpp \
  expression.cpp \
  memory/cache_pool.cpp \
  memory/cache_reference.cpp \
  memory/edition_pool.cpp \
  memory/edition_reference.cpp \
  memory/exception_checkpoint.cpp \
  memory/node.cpp \
  memory/pool.cpp \
  n_ary.cpp \
)

tests_src += $(addprefix poincare_junior/test/,\
  junior.cpp \
)

tests_src += $(addprefix poincare_junior/omgpj/src/,\
  arithmetic.cpp \
  float.cpp \
  memory.cpp \
  list.cpp \
)
# tests_src += $(addprefix poincare_junior/test/,\
  # calculation.cpp \
  # constexpr.cpp \
  # edition_pool_overflow.cpp \
  # elementary_tree_manipulations.cpp \
  # expression_simplification.cpp \
  # graph.cpp \
# )

#SFLAGS += -DASSERTIONS=$(ASSERTIONS) -DPOINCARE_MEMORY_TREE_LOG=1 -Iion/include/ -Ikandinsky/include

