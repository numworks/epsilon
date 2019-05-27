SFLAGS += -Ipoincare/include

src += $(addprefix poincare/src/,\
  binomial_coefficient_layout.cpp \
  bracket_layout.cpp \
  bracket_pair_layout.cpp \
  code_point_layout.cpp\
  condensed_sum_layout.cpp \
  conjugate_layout.cpp \
  empty_layout.cpp \
  fraction_layout.cpp \
  grid_layout.cpp \
  horizontal_layout.cpp \
  integral_layout.cpp \
  layout_cursor.cpp \
  layout.cpp \
  layout_node.cpp \
  left_parenthesis_layout.cpp \
  left_square_bracket_layout.cpp \
  matrix_layout.cpp \
  nth_root_layout.cpp \
  parenthesis_layout.cpp \
  product_layout.cpp \
  right_parenthesis_layout.cpp \
  right_square_bracket_layout.cpp \
  sequence_layout.cpp \
  sum_layout.cpp \
  vertical_offset_layout.cpp \
)

src += $(addprefix poincare/src/,\
  init.cpp \
  exception_checkpoint.cpp \
  helpers.cpp \
)

src += $(addprefix poincare/src/,\
  absolute_value.cpp \
  addition.cpp \
  approximation_helper.cpp \
  arc_cosine.cpp \
  arc_sine.cpp \
  arc_tangent.cpp \
  arithmetic.cpp \
  binomial_coefficient.cpp \
  ceiling.cpp \
  complex.cpp \
  complex_argument.cpp \
  complex_cartesian.cpp \
  confidence_interval.cpp \
  conjugate.cpp \
  constant.cpp \
  cosine.cpp \
  decimal.cpp \
  derivative.cpp \
  determinant.cpp \
  division.cpp \
  division_quotient.cpp \
  division_remainder.cpp \
  empty_expression.cpp \
  equal.cpp \
  evaluation.cpp \
  expression.cpp \
  expression_node.cpp \
  factor.cpp \
  factorial.cpp \
  float.cpp \
  floor.cpp \
  frac_part.cpp \
  function.cpp \
  great_common_divisor.cpp \
  hyperbolic_arc_cosine.cpp \
  hyperbolic_arc_sine.cpp \
  hyperbolic_arc_tangent.cpp \
  hyperbolic_cosine.cpp \
  hyperbolic_sine.cpp \
  hyperbolic_tangent.cpp \
  hyperbolic_trigonometric_function.cpp \
  imaginary_part.cpp \
  infinity.cpp \
  integer.cpp \
  integral.cpp \
  layout_helper.cpp \
  least_common_multiple.cpp \
  logarithm.cpp \
  matrix.cpp \
  matrix_complex.cpp \
  matrix_dimension.cpp \
  matrix_identity.cpp \
  matrix_inverse.cpp \
  matrix_trace.cpp \
  matrix_transpose.cpp \
  multiplication.cpp \
  n_ary_expression_node.cpp \
  naperian_logarithm.cpp \
  nth_root.cpp \
  number.cpp \
  opposite.cpp \
  parametered_expression.cpp \
  parenthesis.cpp \
  permute_coefficient.cpp \
  power.cpp \
  prediction_interval.cpp \
  preferences.cpp \
  print_float.cpp \
  product.cpp \
  randint.cpp \
  random.cpp \
  rational.cpp \
  real_part.cpp \
  round.cpp \
  sequence.cpp \
  serialization_helper.cpp \
  sign_function.cpp \
  simplification_helper.cpp \
  sine.cpp \
  square_root.cpp \
  store.cpp \
  subtraction.cpp \
  sum.cpp \
  symbol.cpp \
  symbol_abstract.cpp \
  tangent.cpp \
  tree_handle.cpp \
  tree_node.cpp \
  tree_pool.cpp \
  trigonometry.cpp \
  trigonometry_cheat_table.cpp \
  undefined.cpp \
  unreal.cpp \
  variable_context.cpp \
)

src += $(addprefix poincare/src/parsing/,\
  parser.cpp \
  tokenizer.cpp \
)

tests += $(addprefix poincare/test/,\
  tree/tree_handle.cpp\
  tree/helpers.cpp\
  addition.cpp\
  arithmetic.cpp\
  binomial_coefficient_layout.cpp\
  complex.cpp\
  complex_to_expression.cpp\
  convert_expression_to_text.cpp\
  decimal.cpp\
  division.cpp\
  expression.cpp\
  expression_order.cpp\
  factorial.cpp\
  float.cpp\
  fraction_layout.cpp\
  function.cpp\
  helper.cpp\
  helpers.cpp\
  infinity.cpp\
  integer.cpp\
  layouts.cpp\
  layout_cursor.cpp\
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
  random.cpp\
  simplify.cpp\
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
src += poincare/src/expression_debug.cpp
SFLAGS += -DPOINCARE_TESTS_PRINT_EXPRESSIONS=1
endif

ifdef POINCARE_TREE_LOG
SFLAGS += -DPOINCARE_TREE_LOG=1
endif
