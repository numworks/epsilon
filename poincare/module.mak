_sources_poincare_minimal := $(addprefix src/, \
  api.cpp:-nopool \
  init.cpp:-nopool \
  init_no_pool.cpp:+nopool \
  trigonometry.cpp \
  preferences.cpp \
  print_float.cpp \
  sign.cpp \
  old/integer.cpp:-nopool \
  old/pool_handle.cpp:-nopool \
  old/pool_object.cpp:-nopool \
  old/pool.cpp:-nopool \
)

_sources_poincare_checkpoint := $(addprefix src/, \
  $(addsuffix :-nocheckpoint, \
    memory/tree_stack_checkpoint.cpp \
    old/pool_checkpoint.cpp:-nopool \
    old/circuit_breaker_checkpoint.cpp:-nopool \
    old/exception_checkpoint.cpp:-nopool \
  ) \
  old/pool_checkpoint_dummy.cpp:+nocheckpoint \
)

_sources_poincare_storage := $(addprefix src/, \
  old/preferences_in_storage.cpp:-nostorage \
  preferences_no_storage.cpp:+nostorage \
)

_sources_poincare_extended := $(addprefix src/, \
$(addprefix old/, \
  computation_context.cpp:-nopool \
  context.cpp \
  empty_context.cpp \
  float_list.cpp:-nopool \
  pool_variable_context.cpp:-nopool \
  tree_variable_context.cpp \
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
  cas_disabled.cpp:-cas \
  cas_enabled.cpp:+cas \
  continuity.cpp \
  decimal.cpp \
  degree.cpp \
  dependency.cpp \
  derivation.cpp \
  dimension.cpp \
  division.cpp \
  equal_sign.cpp \
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
  set.cpp \
  simplification.cpp \
  symbol.cpp \
  systematic_addition.cpp \
  systematic_operation.cpp \
  systematic_multiplication.cpp \
  systematic_reduction.cpp \
  trigonometry.cpp \
  trigonometry_exact_formulas.cpp \
  undefined.cpp \
  units/unit.cpp \
  units/representatives.cpp \
  variables.cpp \
  vector.cpp \
) \
$(addprefix function_properties/, \
  conic.cpp:-nopool \
  function_type.cpp:-nopool \
  helper.cpp:-nopool \
) \
$(addprefix helpers/, \
  cas.cpp:-nopool \
  expression_equal_sign.cpp:-nopool \
  layout.cpp \
  scatter_plot_iterable.cpp:-nopool \
  store.cpp:-nopool \
  symbol.cpp:-nopool \
  trigonometry.cpp:-nopool \
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
  layout_memoization.cpp:-nopool \
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
$(addprefix pool/, \
  expression.cpp:-nopool \
  layout.cpp:-nopool \
  pool_layout_cursor.cpp:-nopool \
) \
$(addprefix solver/, \
  beta_function.cpp \
  erf_inv.cpp \
  matrix_array.cpp \
  point_of_interest_list.cpp:-nopool \
  regularized_gamma_function.cpp \
  regularized_incomplete_beta_function.cpp \
  roots.cpp \
  solver.cpp \
  solver_algorithms.cpp \
  zoom.cpp \
) \
$(addprefix statistics/, \
  $(addprefix distributions/, \
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
    $(addsuffix :-nopool, \
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
    transformed_regression.cpp \
    trigonometric_regression.cpp \
  )) \
  series.cpp \
  statistics_dataset.cpp \
  statistics_dataset_column.cpp \
) \
  additional_results_helper.cpp:-nopool \
  comparison_operator.cpp \
  print.cpp \
  random.cpp \
  range.cpp \
)

_sources_poincare_test := $(addprefix test/, \
  old/additional_results_helper.cpp \
  old/approximation.cpp \
  old/arithmetic.cpp \
  old/conics.cpp \
  old/dependency.cpp \
  old/derivative.cpp \
  old/distribution.cpp \
  old/erf_inv.cpp \
  old/exam_mode.cpp \
  old/expression_order.cpp \
  old/expression_properties.cpp \
  old/expression_serialization.cpp \
  old/expression_to_layout.cpp \
  old/helper.cpp \
  old/layout.cpp \
  old/layout_serialization.cpp \
  old/layout_to_expression.cpp \
  old/matrix.cpp \
  old/numeric_solver.cpp \
  old/parsing.cpp \
  old/print.cpp \
  old/print_float.cpp \
  old/range.cpp \
  old/rational.cpp \
  old/regularized_function.cpp \
  old/simplification.cpp \
  old/tree/helpers.cpp \
  old/tree/tree_handle.cpp \
  old/zoom.cpp \
  api.cpp \
  approximation.cpp \
  beautification.cpp \
  dimension.cpp \
  equation_solver.cpp \
  float_helper.cpp \
  helper.cpp \
  integer.cpp \
  k_tree.cpp \
  latex_parser.cpp \
  layout.cpp \
  match.cpp \
  matrix.cpp \
  memory_elements.cpp \
  n_ary.cpp \
  order.cpp \
  parse.cpp \
  polynomial.cpp \
  projection.cpp \
  random.cpp \
  rational.cpp \
  roots.cpp \
  set.cpp \
  sign.cpp \
  simplification.cpp \
  tree_stack.cpp \
  trigonometry_exact_formulas.cpp \
$(addprefix helpers/, \
  expression_equal_sign.cpp \
) \
)

$(call create_module,poincare,1, \
  $(_sources_poincare_minimal) \
  $(_sources_poincare_checkpoint) \
  $(_sources_poincare_storage) \
  $(addsuffix :-minimal,$(_sources_poincare_extended)) \
  $(addsuffix :+test,$(_sources_poincare_test)) \
)

$(call assert_defined,KANDINSKY_fonts_dependencies)
$(call all_objects_for,$(SOURCES_poincare)): $(KANDINSKY_fonts_dependencies)

POINCARE_TREE_STACK_VISUALIZATION ?= 0
ifneq ($(POINCARE_TREE_STACK_VISUALIZATION),0)
POINCARE_TREE_LOG := 1
SFLAGS_poincare += -DPOINCARE_TREE_STACK_VISUALIZATION=1
endif

POINCARE_METRICS ?= 0
ifneq ($(POINCARE_METRICS),0)
SFLAGS_poincare += -DPOINCARE_METRICS=1
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

CXXFLAGS += -include $(PATH_poincare)/config.$(POINCARE_variant).h

# TODO: add a compiler family in haussmann
ifeq ($(PLATFORM_TYPE),simulator)
PRIVATE_SFLAGS_poincare += -Wno-tautological-constant-out-of-range-compare
else
PRIVATE_SFLAGS_poincare += -Wno-switch-outside-range
endif
