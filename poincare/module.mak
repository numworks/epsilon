_sources_poincare_minimal := $(addprefix src/, \
  api.cpp \
  cas_disabled.cpp:-cas \
  cas_enabled.cpp:+cas \
  init.cpp \
  new_trigonometry.cpp \
  preferences.cpp \
  print_float.cpp \
  sign.cpp \
  old/helpers.cpp \
  old/integer.cpp \
  old/serialization_helper.cpp \
  old/pool_handle.cpp \
  old/pool_object.cpp \
  old/pool.cpp \
)

_sources_poincare_checkpoint := $(addprefix src/, \
  $(addsuffix :-nocheckpoint, \
    memory/tree_stack_checkpoint.cpp \
    old/pool_checkpoint.cpp \
    old/circuit_breaker_checkpoint.cpp \
    old/exception_checkpoint.cpp \
  ) \
  old/pool_checkpoint_dummy.cpp:+nocheckpoint \
)

_sources_poincare_storage := $(addprefix src/, \
  old/preferences_in_storage.cpp:-nostorage \
  preferences_no_storage.cpp:+nostorage \
)

_sources_poincare_extended := $(addprefix src/, \
$(addprefix old/, \
  absolute_value.cpp \
  addition.cpp \
  aliases_list.cpp \
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
  empty_context.cpp \
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
  integral.cpp \
  junior_expression.cpp \
  junior_layout.cpp \
  layout.cpp \
  layout_node.cpp \
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
  polynomial.cpp \
  power.cpp \
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
  trigonometry.cpp \
  trigonometry_cheat_table.cpp \
  undefined.cpp \
  unit.cpp \
  unit_convert.cpp \
  variable_context.cpp \
  vector_cross.cpp \
  vector_dot.cpp \
  vector_norm.cpp \
  xnt_helpers.cpp \
) \
$(addprefix expression/, \
  advanced_operation.cpp \
  advanced_reduction.cpp \
  algebraic.cpp \
  aliases.cpp \
  arithmetic.cpp \
  approximation.cpp \
  approximation_derivative.cpp \
  approximation_helpers.cpp \
  approximation_integral.cpp \
  approximation_matrix.cpp \
  approximation_prepare.cpp \
  approximation_power.cpp \
  approximation_trigonometry.cpp \
  beautification.cpp \
  binary.cpp \
  builtin.cpp \
  continuity.cpp \
  conversion.cpp \
  decimal.cpp \
  degree.cpp \
  dependency.cpp \
  derivation.cpp \
  dimension.cpp \
  division.cpp \
  equation_solver.cpp \
  float_helper.cpp \
  infinity.cpp \
  integer.cpp \
  integer_serialization.cpp \
  list.cpp \
  logarithm.cpp \
  matrix.cpp \
  metric.cpp \
  number.cpp \
  order.cpp \
  parametric.cpp \
  physical_constant.cpp \
  polynomial.cpp \
  projection.cpp \
  random.cpp \
  rational.cpp \
  sequence.cpp \
  sequence_cache.cpp \
  set.cpp \
  simplification.cpp \
  symbol.cpp \
  systematic_addition.cpp \
  systematic_operation.cpp \
  systematic_multiplication.cpp \
  systematic_reduction.cpp \
  trigonometry.cpp \
  undefined.cpp \
  unit.cpp \
  unit_representatives.cpp \
  variables.cpp \
  vector.cpp \
) \
$(addprefix function_properties/, \
  conic.cpp \
  function_type.cpp \
  helper.cpp \
) \
$(addprefix helpers/, \
  expression_equal_sign.cpp \
  layout.cpp \
) \
$(addprefix layout/, \
  autocompleted_pair.cpp \
  code_point_layout.cpp \
  cursor_motion.cpp \
  empty_rectangle.cpp \
  grid.cpp \
  input_beautification.cpp \
  layout_cursor.cpp \
  layout_selection.cpp \
  layout_span.cpp \
  layout_span_decoder.cpp \
  layouter.cpp \
  multiplication_symbol.cpp \
  parser.cpp \
  parsing/helper.cpp \
  parsing/latex_parser.cpp \
  parsing/rack_parser.cpp \
  parsing/tokenizer.cpp \
  rack_from_text.cpp \
  rack_layout.cpp \
  rack_layout_decoder.cpp \
  render.cpp \
  serialize.cpp \
  xnt.cpp \
) \
$(addprefix memory/, \
  block_stack.cpp \
  tree_stack.cpp \
  n_ary.cpp \
  pattern_matching.cpp \
  tree.cpp \
  tree_ref.cpp \
  value_block.cpp \
  visualization.cpp \
) \
$(addprefix numeric/, \
  beta_function.cpp \
  erf_inv.cpp \
  helpers.cpp \
  point_of_interest.cpp \
  regularized_gamma_function.cpp \
  regularized_incomplete_beta_function.cpp \
  roots.cpp \
  solver.cpp \
  solver_algorithms.cpp \
  statistics_dataset.cpp \
  statistics_dataset_column.cpp \
  zoom.cpp \
) \
$(addprefix probability/, \
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
) \
$(addprefix regression/, \
  affine_regression.cpp \
  cubic_regression.cpp \
  exponential_regression.cpp \
  linear_regression.cpp \
  logarithmic_regression.cpp \
  logistic_regression.cpp \
  median_regression.cpp \
  power_regression.cpp \
  proportional_regression.cpp \
  quadratic_regression.cpp \
  quartic_regression.cpp \
  regression.cpp \
  regression_switch.cpp \
  series.cpp \
  transformed_regression.cpp \
  trigonometric_regression.cpp \
) \
  additional_results_helper.cpp \
  print.cpp \
  range.cpp \
)

_sources_poincare_test := $(addprefix test/, \
  old/additional_results_helper.cpp \
  old/approximation.cpp \
  old/arithmetic.cpp \
  old/conics.cpp \
  old/derivative.cpp \
  old/distribution.cpp \
  old/erf_inv.cpp \
  old/expression_properties.cpp \
  old/expression_to_layout.cpp \
  old/helper.cpp \
  old/layout.cpp \
  old/layout_cursor.cpp \
  old/layout_serialization.cpp \
  old/layout_to_expression.cpp \
  old/numeric_solver.cpp \
  old/parsing.cpp \
  old/print.cpp \
  old/print_float.cpp \
  old/range.cpp \
  old/regularized_function.cpp \
  old/simplification.cpp \
  old/zoom.cpp \
  adaptive_strategy.cpp \
  api.cpp \
  approximation.cpp \
  beautification.cpp \
  comparison.cpp \
  dimension.cpp \
  equation_solver.cpp \
  expression_comparison.cpp \
  float_helper.cpp \
  layout.cpp \
  helper.cpp \
  integer.cpp \
  k_tree.cpp \
  latex_parser.cpp \
  match.cpp \
  matrix.cpp \
  memory_elements.cpp \
  n_ary.cpp \
  parse.cpp \
  polynomial.cpp \
  projection.cpp \
  rational.cpp \
  roots.cpp \
  set.cpp \
  sign.cpp \
  simplification.cpp \
  tree_stack.cpp \
$(addprefix helpers/, \
  expression_equal_sign.cpp \
) \
)

_sources_poincare_js_bridge := $(patsubst %,src/js_bridge/%:+js_bridge, \
  computation_context.cpp \
  expression.cpp \
  system_expression.cpp \
  system_function.cpp \
  user_expression.cpp \
  utils.cpp \
  regression.cpp \
  solver.cpp \
  statistics.cpp \
)

$(call create_module,poincare,1, \
  $(_sources_poincare_minimal) \
  $(_sources_poincare_checkpoint) \
  $(_sources_poincare_storage) \
  $(_sources_poincare_js_bridge) \
  $(addsuffix :-minimal,$(_sources_poincare_extended)) \
  $(addsuffix :+test,$(_sources_poincare_test)) \
)

$(call assert_defined,KANDINSKY_fonts_dependencies)
$(call all_objects_for,$(SOURCES_poincare)): $(KANDINSKY_fonts_dependencies)

POINCARE_POOL_VISUALIZATION ?= 0
ifneq ($(POINCARE_POOL_VISUALIZATION),0)
POINCARE_TREE_LOG := 1
SFLAGS_poincare += -DPOINCARE_POOL_VISUALIZATION=1
endif

POINCARE_TREE_LOG ?= 0
ifeq ($(PLATFORM_TYPE),simulator)
ifneq ($(DEBUG),0)
POINCARE_TREE_LOG := 1
endif
endif

ifneq ($(POINCARE_TREE_LOG),0)
SFLAGS_poincare += -DPOINCARE_TREE_LOG=1
endif
