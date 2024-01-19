PLATFORM ?= simulator
DEBUG ?= 1
DEVELOPMENT ?= 1
ARCH = arm64
ASSERTIONCHECKPOINT ?= 0

SFLAGS += -DASSERTIONCHECKPOINT=$(ASSERTIONCHECKPOINT)

include poincare_junior/omgpj/Makefile

poincare_junior_src += $(omgpj_src)

poincare_junior_src += $(addprefix poincare_junior/src/,\
  expression/algebraic.cpp \
  expression/aliases.cpp \
  expression/arithmetic.cpp \
  expression/approximation.cpp \
  expression/approximation_derivative.cpp \
  expression/approximation_integral.cpp \
  expression/approximation_trigonometry.cpp \
  expression/beautification.cpp \
  expression/builtin.cpp \
  expression/comparison.cpp \
  expression/conversion.cpp \
  expression/decimal.cpp \
  expression/dependency.cpp \
  expression/derivation.cpp \
  expression/dimension.cpp \
  expression/integer.cpp \
  expression/list.cpp \
  expression/logarithm.cpp \
  expression/matrix.cpp \
  expression/metric.cpp \
  expression/number.cpp \
  expression/parametric.cpp \
  expression/polynomial.cpp \
  expression/projection.cpp \
  expression/random.cpp \
  expression/rational.cpp \
  expression/set.cpp \
  expression/sign.cpp \
  expression/simplification.cpp \
  expression/solver.cpp \
  expression/symbol.cpp \
  expression/trigonometry.cpp \
  expression/unit.cpp \
  expression/unit_representatives.cpp \
  expression/variables.cpp \
  expression/vector.cpp \
  expression.cpp \
  layout/autocompleted_pair.cpp \
  layout/code_point_layout.cpp \
  layout/cursor_motion.cpp \
  layout/empty_rectangle.cpp \
  layout/grid.cpp \
  layout/layout_cursor.cpp \
  layout/layoutter.cpp \
  layout/parser.cpp \
  layout/parsing/rack_parser.cpp \
  layout/parsing/tokenizer.cpp \
  layout/rack_layout.cpp \
  layout/render.cpp \
  layout.cpp \
  memory/cache_pool.cpp \
  memory/edition_pool.cpp \
  memory/edition_reference.cpp \
  memory/exception_checkpoint.cpp \
  memory/pattern_matching.cpp \
  memory/pool.cpp \
  memory/reference.cpp \
  memory/storage_context.cpp \
  memory/tree.cpp \
  memory/value_block.cpp \
  n_ary.cpp \
  poincare.cpp \
)

tests_src += $(addprefix poincare_junior/test/,\
  approximation.cpp \
  beautification.cpp \
  calculation.cpp \
  comparison.cpp \
  dimension.cpp \
  expression_comparison.cpp \
  layout.cpp \
  graph.cpp \
  helper.cpp \
  integer.cpp \
  k_tree.cpp \
  match.cpp \
  matrix.cpp \
  memory_elements.cpp \
  memory_edition.cpp \
  memory_cache.cpp \
  n_ary.cpp \
  parse.cpp \
  polynomial.cpp \
  projection.cpp \
  rational.cpp \
  set.cpp \
  sign.cpp \
  simplification.cpp \
  solver.cpp \
  storage_context.cpp \
)

tests_src += $(poincare_junior_src)
