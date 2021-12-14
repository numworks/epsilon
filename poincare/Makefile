SFLAGS += -Ipoincare/include

poincare_src += $(addprefix poincare/src/,\
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
  kmat.cpp \
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
  vector_layout.cpp \
  vertical_offset_layout.cpp \
)

poincare_src += $(addprefix poincare/src/,\
  init.cpp \
  beta_function.cpp \
  binomial_distribution.cpp \
  erf_inv.cpp \
  exception_checkpoint.cpp \
  helpers.cpp \
  normal_distribution.cpp \
  regularized_incomplete_beta_function.cpp \
)

poincare_src += $(addprefix poincare/src/,\
  absolute_value.cpp \
  addition.cpp \
  approximation_helper.cpp \
  arc_cosine.cpp \
  arc_sine.cpp \
  arc_tangent.cpp \
  arithmetic.cpp \
  based_integer.cpp \
  binom_cdf.cpp \
  binomial_coefficient.cpp \
  binomial_distribution_function.cpp \
  binom_pdf.cpp \
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
  inv_binom.cpp \
  inv_norm.cpp \
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
  matrix_echelon_form.cpp \
  matrix_row_echelon_form.cpp \
  matrix_reduced_row_echelon_form.cpp \
  multiplication.cpp \
  n_ary_expression.cpp \
  n_ary_infix_expression.cpp \
  naperian_logarithm.cpp \
  norm_cdf.cpp \
  norm_cdf2.cpp \
  norm_pdf.cpp \
  normal_distribution_function.cpp \
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
  print_int.cpp \
  product.cpp \
  randint.cpp \
  random.cpp \
  rational.cpp \
  real_part.cpp \
  rightwards_arrow_expression.cpp \
  round.cpp \
  sequence.cpp \
  serialization_helper.cpp \
  sign_function.cpp \
  sine.cpp \
  solver.cpp \
  square_root.cpp \
  store.cpp \
  sum_and_product.cpp \
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
  unit.cpp \
  unit_convert.cpp \
  unreal.cpp \
  variable_context.cpp \
  vector_cross.cpp \
  vector_dot.cpp \
  vector_norm.cpp \
  zoom.cpp \
)

poincare_src += $(addprefix poincare/src/parsing/,\
  parser.cpp \
  tokenizer.cpp \
)

tests_src += $(addprefix poincare/test/,\
  tree/tree_handle.cpp\
  tree/helpers.cpp\
  approximation.cpp\
  arithmetic.cpp\
  context.cpp\
  erf_inv.cpp \
  derivative.cpp\
  expression.cpp\
  expression_order.cpp\
  expression_properties.cpp\
  expression_serialization.cpp\
  expression_to_layout.cpp\
  function_solver.cpp\
  helper.cpp\
  helpers.cpp\
  infinity.cpp \
  integer.cpp\
  layout.cpp\
  layout_cursor.cpp\
  layout_serialization.cpp\
  layout_to_expression.cpp\
  parsing.cpp\
  print_float.cpp\
  print_int.cpp\
  rational.cpp\
  regularized_incomplete_beta_function.cpp \
  simplification.cpp\
  zoom.cpp\
)

ifeq ($(DEBUG),1)
POINCARE_TREE_LOG ?= 1
endif

ifdef POINCARE_TREE_LOG
SFLAGS += -DPOINCARE_TREE_LOG=$(POINCARE_TREE_LOG)
endif
