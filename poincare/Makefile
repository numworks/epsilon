SFLAGS += -Ipoincare/include

poincare_src += $(addprefix poincare/src/,\
  junior_layout.cpp \
  layout.cpp \
  layout_node.cpp \
)

poincare_src += $(addprefix poincare/src/,\
  aliases_list.cpp \
  beta_function.cpp \
  binomial_distribution.cpp \
  cdf_method.cpp \
  cdf_range_method.cpp \
  checkpoint.cpp \
  chi2_distribution.cpp \
  circuit_breaker_checkpoint.cpp \
  discrete_distribution.cpp \
  distribution.cpp \
  distribution_method.cpp \
  domain.cpp \
  erf_inv.cpp \
  exception_checkpoint.cpp \
  exponential_distribution.cpp \
  fisher_distribution.cpp \
  geometric_distribution.cpp \
  helpers.cpp \
  hypergeometric_distribution.cpp \
  init.cpp \
  inv_method.cpp \
  normal_distribution.cpp \
  pdf_method.cpp \
  poisson_distribution.cpp \
  range.cpp \
  regularized_gamma_function.cpp \
  regularized_incomplete_beta_function.cpp \
  statistics_dataset.cpp\
  student_distribution.cpp \
  uniform_distribution.cpp \
  xnt_helpers.cpp \
)

poincare_src += $(addprefix poincare/src/,\
  absolute_value.cpp \
  addition.cpp \
  approximation_helper.cpp \
  arc_cosecant.cpp \
  arc_cosine.cpp \
  arc_cotangent.cpp \
  arc_secant.cpp \
  arc_sine.cpp \
  arc_tangent.cpp \
  arithmetic.cpp \
  array.cpp \
  based_integer.cpp \
  binomial_coefficient.cpp \
  boolean.cpp \
  ceiling.cpp \
  comparison.cpp \
  complex.cpp \
  complex_argument.cpp \
  complex_cartesian.cpp \
  computation_context.cpp \
  conic.cpp \
  conjugate.cpp \
  constant.cpp \
  context.cpp \
  cosecant.cpp \
  cosine.cpp \
  cotangent.cpp \
  decimal.cpp \
  dependency.cpp \
  derivative.cpp \
  determinant.cpp \
  dimension.cpp \
  distribution_dispatcher.cpp \
  division.cpp \
  division_quotient.cpp \
  division_remainder.cpp \
  empty_expression.cpp \
  evaluation.cpp \
  expression.cpp \
  expression_node.cpp \
  expression_node_properties.cpp \
  factor.cpp \
  factorial.cpp \
  float.cpp \
  float_list.cpp \
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
  junior_expression.cpp \
  least_common_multiple.cpp \
  list.cpp \
  list_access.cpp \
  list_complex.cpp \
  list_maximum.cpp \
  list_mean.cpp \
  list_median.cpp \
  list_minimum.cpp \
  list_product.cpp \
  list_sample_standard_deviation.cpp \
  list_sequence.cpp \
  list_sort.cpp \
  list_standard_deviation.cpp \
  list_sum.cpp \
  list_variance.cpp \
  logarithm.cpp \
  logical_operator.cpp \
  matrix.cpp \
  matrix_complex.cpp \
  matrix_echelon_form.cpp \
  matrix_identity.cpp \
  matrix_inverse.cpp \
  matrix_reduced_row_echelon_form.cpp \
  matrix_row_echelon_form.cpp \
  matrix_trace.cpp \
  matrix_transpose.cpp \
  mixed_fraction.cpp \
  multiplication.cpp \
  n_ary_expression.cpp \
  n_ary_infix_expression.cpp \
  naperian_logarithm.cpp \
  nonreal.cpp \
  nth_root.cpp \
  number.cpp \
  opposite.cpp \
  parametered_expression.cpp \
  parenthesis.cpp \
  percent.cpp \
  permute_coefficient.cpp \
  piecewise_operator.cpp \
  point.cpp \
  point_evaluation.cpp \
  point_of_interest.cpp \
  polynomial.cpp \
  power.cpp \
  preferences.cpp \
  preferences_in_storage.cpp \
  print.cpp \
  print_float.cpp \
  print_int.cpp \
  product.cpp \
  randint.cpp \
  randint_no_repeat.cpp \
  random.cpp \
  rational.cpp \
  real_part.cpp \
  rightwards_arrow_expression.cpp \
  round.cpp \
  secant.cpp \
  sequence.cpp \
  serialization_helper.cpp \
  sign_function.cpp \
  simplification_helper.cpp \
  sine.cpp \
  solver.cpp \
  solver_algorithms.cpp \
  square_root.cpp \
  store.cpp \
  subtraction.cpp \
  sum.cpp \
  sum_and_product.cpp \
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
  variable_context.cpp \
  vector_cross.cpp \
  vector_dot.cpp \
  vector_norm.cpp \
  zoom.cpp \
)

poincare_src += $(addprefix poincare/src/parsing/,\
  parser.cpp \
  helper.cpp \
  tokenizer.cpp \
)

tests_src += $(addprefix poincare/test/,\
  helper.cpp \
  approximation.cpp \
  derivative.cpp \
  parsing.cpp \
  simplification.cpp \
)

poincare_bench_src = $(addprefix poincare/src/,\
  checkpoint_dummy.cpp \
  helpers.cpp \
  init.cpp \
  integer.cpp \
  preferences.cpp \
  preferences_no_storage.cpp \
  print_float.cpp \
  print_int.cpp \
  serialization_helper.cpp \
  tree_handle.cpp \
  tree_node.cpp \
  tree_pool.cpp \
)

ifeq ($(DEBUG),1)
  ifeq ($(PLATFORM),simulator)
    POINCARE_TREE_LOG ?= 1
  endif
endif

ifdef POINCARE_TREE_LOG
SFLAGS += -DPOINCARE_TREE_LOG=$(POINCARE_TREE_LOG)
endif
