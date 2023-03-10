PLATFORM ?= simulator
DEBUG ?= 1
DEVELOPMENT ?= 1
ARCH = arm64
ASSERTIONS = 1

include poincare_junior/omgpj/Makefile

poincare_junior_src += $(omgpj_src)

poincare_junior_src += $(addprefix poincare_junior/src/,\
  expression/algebraic.cpp \
  expression/aliases.cpp \
  expression/approximation.cpp \
  expression/builtin.cpp \
  expression/comparison.cpp \
  expression/integer.cpp \
  expression/polynomial.cpp \
  expression/rational.cpp \
  expression/simplification.cpp \
  expression/set.cpp \
  expression/symbol.cpp \
  expression.cpp \
  layout/code_point_layout.cpp \
  layout/empty_rectangle.cpp \
  layout/fraction_layout.cpp \
  layout/parenthesis_layout.cpp \
  layout/parser.cpp \
  layout/parsing/rack_parser.cpp \
  layout/parsing/tokenizer.cpp \
  layout/rack_layout.cpp \
  layout/render.cpp \
  layout/vertical_offset_layout.cpp \
  layout.cpp \
  memory/cache_pool.cpp \
  memory/edition_pool.cpp \
  memory/edition_reference.cpp \
  memory/exception_checkpoint.cpp \
  memory/node.cpp \
  memory/pattern_matching.cpp \
  memory/pool.cpp \
  memory/reference.cpp \
  n_ary.cpp \
)

tests_src += $(addprefix poincare_junior/test/,\
  calculation.cpp \
  elementary_tree_manipulations.cpp \
  expression_comparison.cpp \
  layout.cpp \
  graph.cpp \
  helper.cpp \
  integer.cpp \
  k_creator.cpp \
  match.cpp \
  memory_elements.cpp \
  memory_edition.cpp \
  memory_cache.cpp \
  parse.cpp \
  polynomial.cpp \
  set.cpp \
)

tests_src += $(poincare_junior_src)
