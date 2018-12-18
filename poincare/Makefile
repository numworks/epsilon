SFLAGS += -Ipoincare/include

#include poincare/src/simplify/Makefile
#include poincare/src/simplification/Makefile
objs += $(addprefix poincare/src/,\
  binomial_coefficient_layout.o\
  bracket_layout.o\
  bracket_pair_layout.o\
  ceiling_layout.o\
  char_layout.o\
  condensed_sum_layout.o\
  conjugate_layout.o\
  empty_layout.o\
  fraction_layout.o\
  grid_layout.o\
  horizontal_layout.o\
  integral_layout.o\
  layout_cursor.o\
  layout.o\
  layout_node.o\
  left_parenthesis_layout.o\
  left_square_bracket_layout.o\
  matrix_layout.o\
  nth_root_layout.o\
  parenthesis_layout.o\
  product_layout.o\
  right_parenthesis_layout.o\
  right_square_bracket_layout.o\
  sequence_layout.o\
  sum_layout.o\
  vertical_offset_layout.o\
)

objs += $(addprefix poincare/src/,\
  init.o\
  exception_checkpoint.o\
  helpers.o\
)

objs += $(addprefix poincare/src/,\
  absolute_value.o\
  addition.o\
  approximation_helper.o\
  arc_cosine.o\
  arc_sine.o\
  arc_tangent.o\
  arithmetic.o\
  binomial_coefficient.o\
  ceiling.o\
  complex.o\
  complex_argument.o\
  confidence_interval.o\
  conjugate.o\
  constant.o\
  cosine.o\
  decimal.o\
  derivative.o\
  determinant.o\
  division.o\
  division_quotient.o\
  division_remainder.o\
  empty_expression.o\
  equal.o\
  evaluation.o\
  expression.o\
  expression_node.o\
  factor.o\
  factorial.o\
  float.o\
  floor.o\
  frac_part.o\
  function.o\
  great_common_divisor.o\
  hyperbolic_arc_cosine.o\
  hyperbolic_arc_sine.o\
  hyperbolic_arc_tangent.o\
  hyperbolic_cosine.o\
  hyperbolic_sine.o\
  hyperbolic_tangent.o\
  hyperbolic_trigonometric_function.o\
  imaginary_part.o\
  infinity.o\
  integer.o\
  integral.o\
  layout_helper.o\
  least_common_multiple.o\
  logarithm.o\
  matrix.o\
  matrix_complex.o\
  matrix_dimension.o\
  matrix_inverse.o\
  matrix_trace.o\
  matrix_transpose.o\
  multiplication.o\
  n_ary_expression_node.o\
  naperian_logarithm.o\
  nth_root.o\
  number.o\
  opposite.o\
  parametered_expression_helper.o\
  parenthesis.o\
  permute_coefficient.o\
  power.o\
  prediction_interval.o\
  preferences.o\
  print_float.o\
  product.o\
  randint.o\
  random.o\
  rational.o\
  real_part.o\
  round.o\
  sequence.o\
  serialization_helper.o\
  simplification_helper.o\
  sine.o\
  square_root.o\
  store.o\
  subtraction.o\
  sum.o\
  symbol.o\
  symbol_abstract.o\
  tangent.o\
  tree_handle.o\
  tree_node.o\
  tree_pool.o\
  trigonometry.o\
  undefined.o\
  variable_context.o\
)

objs += $(addprefix poincare/src/parsing/,\
  parser.o\
  tokenizer.o\
)

tests += $(addprefix poincare/test/,\
  tree/tree_handle.cpp\
  tree/helpers.cpp\
  addition.cpp\
  arithmetic.cpp\
  binomial_coefficient_layout.cpp\
  complex_to_expression.cpp\
  convert_expression_to_text.cpp\
  decimal.cpp\
  division.cpp\
  expression.cpp\
  factorial.cpp\
  float.cpp\
  fraction_layout.cpp\
  function.cpp\
  helper.cpp\
  helpers.cpp\
  infinity.cpp\
  integer.cpp\
  layouts.cpp\
  logarithm.cpp\
  matrix.cpp\
  multiplication.cpp\
  nth_root_layout.cpp\
  number.cpp\
  parentheses_layout.cpp\
  parser.cpp\
  power.cpp\
  properties.cpp\
  rational.cpp\
  simplify_mix.cpp\
  store.cpp\
  subtraction.cpp\
  symbol.cpp\
  trigo.cpp\
  user_variable.cpp\
  vertical_offset_layout.cpp\
)

test_objs += $(addprefix poincare/test/, tree/helpers.o)
test_objs += $(addprefix apps/shared/, global_context.o)

#  simplify_utils.cpp\

ifdef POINCARE_TESTS_PRINT_EXPRESSIONS
tests += poincare/src/expression_debug.o
objs += poincare/src/expression_debug.o
SFLAGS += -DPOINCARE_TESTS_PRINT_EXPRESSIONS=1
endif

ifdef POINCARE_TREE_LOG
SFLAGS += -DPOINCARE_TREE_LOG=1
endif
