PLATFORM ?= simulator
DEBUG ?= 1
DEVELOPMENT ?= 1
ARCH = arm64

include poincare_junior/omgpj/Makefile

poincare_junior_src += $(omgpj_src)

poincare_junior_src += $(addprefix poincare_junior/src/,\
  expression/advanced_simplification.cpp \
  expression/algebraic.cpp \
  expression/aliases.cpp \
  expression/arithmetic.cpp \
  expression/approximation.cpp \
  expression/approximation_derivative.cpp \
  expression/approximation_integral.cpp \
  expression/approximation_power.cpp \
  expression/approximation_trigonometry.cpp \
  expression/beautification.cpp \
  expression/binary.cpp \
  expression/builtin.cpp \
  expression/comparison.cpp \
  expression/constant.cpp \
  expression/conversion.cpp \
  expression/decimal.cpp \
  expression/dependency.cpp \
  expression/derivation.cpp \
  expression/dimension.cpp \
  expression/float.cpp \
  expression/integer.cpp \
  expression/integer_serialization.cpp \
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
  layout/app_helpers.cpp \
  layout/autocompleted_pair.cpp \
  layout/code_point_layout.cpp \
  layout/cursor_motion.cpp \
  layout/empty_rectangle.cpp \
  layout/grid.cpp \
  layout/input_beautification.cpp \
  layout/layout_cursor.cpp \
  layout/layout_selection.cpp \
  layout/layoutter.cpp \
  layout/multiplication_symbol.cpp \
  layout/parser.cpp \
  layout/parsing/helper.cpp \
  layout/parsing/rack_parser.cpp \
  layout/parsing/tokenizer.cpp \
  layout/rack_from_text.cpp \
  layout/rack_layout.cpp \
  layout/rack_layout_decoder.cpp \
  layout/render.cpp \
  layout/serialize.cpp \
  layout/xnt.cpp \
  memory/edition_pool.cpp \
  memory/edition_reference.cpp \
  memory/exception_checkpoint.cpp \
  memory/pattern_matching.cpp \
  memory/storage_context.cpp \
  memory/tree.cpp \
  memory/value_block.cpp \
  n_ary.cpp \
  poincare.cpp \
)

poincare_junior_src += $(addprefix poincare_junior/src/numeric/,\
  beta_function.cpp \
  erf_inv.cpp \
  range.cpp \
  regularized_gamma_function.cpp \
  regularized_incomplete_beta_function.cpp \
  solver_algorithms.cpp \
)

poincare_junior_src += $(addprefix poincare_junior/src/probability/,\
  binomial_distribution.cpp \
  cdf_method.cpp \
  cdf_range_method.cpp \
  chi2_distribution.cpp \
  discrete_distribution.cpp \
  distribution.cpp \
  distribution_method.cpp \
  domain.cpp \
  exponential_distribution.cpp \
  fisher_distribution.cpp \
  geometric_distribution.cpp \
  hypergeometric_distribution.cpp \
  inv_method.cpp \
  normal_distribution.cpp \
  pdf_method.cpp \
  poisson_distribution.cpp \
  student_distribution.cpp \
  uniform_distribution.cpp \
)

tests_src += $(addprefix poincare_junior/test/,\
  approximation.cpp \
  beautification.cpp \
  comparison.cpp \
  dimension.cpp \
  expression_comparison.cpp \
  layout.cpp \
  helper.cpp \
  integer.cpp \
  k_tree.cpp \
  match.cpp \
  matrix.cpp \
  memory_elements.cpp \
  memory_edition.cpp \
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

ifeq ($(DEBUG),1)
  ifeq ($(PLATFORM),simulator)
    POINCARE_TREE_LOG ?= 1
  endif
endif

ifdef POINCARE_TREE_LOG
SFLAGS += -DPOINCARE_TREE_LOG=$(POINCARE_TREE_LOG)
endif
