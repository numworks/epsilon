PLATFORM ?= simulator
DEBUG ?= 1
DEVELOPMENT ?= 1
ARCH = arm64
ASSERTIONS = 1

include poincare_junior/omgpj/Makefile

poincare_junior_src += $(omgpj_src)

poincare_junior_src += $(addprefix poincare_junior/src/,\
  expression/algebraic.cpp \
  expression/approximation.cpp \
  expression/comparison.cpp \
  expression/integer.cpp \
  expression/polynomial.cpp \
  expression/rational.cpp \
  expression/simplification.cpp \
  expression/set.cpp \
  expression/symbol.cpp \
  expression.cpp \
  memory/cache_pool.cpp \
  memory/cache_reference.cpp \
  memory/edition_pool.cpp \
  memory/edition_reference.cpp \
  memory/exception_checkpoint.cpp \
  memory/node.cpp \
  memory/pattern_matching.cpp \
  memory/pool.cpp \
  n_ary.cpp \
)

tests_src += $(addprefix poincare_junior/test/,\
  calculation.cpp \
  elementary_tree_manipulations.cpp \
  expression_comparison.cpp \
  graph.cpp \
  match.cpp \
  memory_elements.cpp \
  memory_edition.cpp \
  memory_cache.cpp \
  polynomial.cpp \
  set.cpp \
)

tests_src += $(addprefix poincare_junior/omgpj/src/,\
  arithmetic.cpp \
  float.cpp \
  memory.cpp \
  list.cpp \
)

tests_src += $(poincare_junior_src)
