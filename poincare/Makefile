SFLAGS += -Ipoincare/include

#include poincare/src/simplify/Makefile
#include poincare/src/simplification/Makefile
objs += $(addprefix poincare/src/,\
  binomial_coefficient_layout_node.o\
  bracket_layout_node.o\
  bracket_pair_layout_node.o\
  ceiling_layout_node.o\
  char_layout_node.o\
  condensed_sum_layout_node.o\
  conjugate_layout_node.o\
  empty_layout_node.o\
  floor_layout_node.o\
  fraction_layout_node.o\
  grid_layout_node.o\
  horizontal_layout_node.o\
  integral_layout_node.o\
  layout_cursor.o\
  layout_node.o\
  layout_reference.o\
  left_parenthesis_layout_node.o\
  left_square_bracket_layout_node.o\
  matrix_layout_node.o\
  nth_root_layout_node.o\
  parenthesis_layout_node.o\
  product_layout_node.o\
  right_parenthesis_layout_node.o\
  right_square_bracket_layout_node.o\
  sequence_layout_node.o\
  sum_layout_node.o\
  vertical_offset_layout_node.o\
)

objs += $(addprefix poincare/src/,\
  init.o\
  exception_checkpoint.o\
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
  expression_lexer.o\
  expression_node.o\
  expression_parser.o\
  factor.o\
  factorial.o\
  float.o\
  floor.o\
  frac_part.o\
  global_context.o\
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
  opposite.o\
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
  parenthesis.o\
  permute_coefficient.o\
  power.o\
  print_float.o\
  prediction_interval.o\
  preferences.o\
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
  tangent.o\
  tree_node.o\
  tree_pool.o\
  tree_by_reference.o\
  trigonometry.o\
  undefined.o\
  variable_context.o\
)

objsExpected += $(addprefix poincare/src/,\
  binomial_coefficient_layout_node.o\
  bracket_layout_node.o\
  bracket_pair_layout_node.o\
  char_layout_node.o\
  condensed_sum_layout_node.o\
  conjugate_layout_node.o\
  empty_layout_node.o\
  fraction_layout_node.o\
  grid_layout_node.o\
  horizontal_layout_node.o\
  integral_layout_node.o\
  layout_cursor.o\
  layout_node.o\
  layout_reference.o\
  left_parenthesis_layout_node.o\
  left_square_bracket_layout_node.o\
  matrix_layout_node.o\
  nth_root_layout_node.o\
  parenthesis_layout_node.o\
  product_layout_node.o\
  right_parenthesis_layout_node.o\
  right_square_bracket_layout_node.o\
  sequence_layout_node.o\
  sum_layout_node.o\
  vertical_offset_layout_node.o\
)

objsExpected += $(addprefix poincare/src/,\
  absolute_value.o\
  addition.o\
  approximation_helper.o\
  arc_cosine.o\
  arc_sine.o\
  arc_tangent.o\
  arithmetic.o\
  binomial_coefficient.o\
  bounded_static_hierarchy.o\
  ceiling.o\
  complex_argument.o\
  confidence_interval.o\
  conjugate.o\
  cosine.o\
  decimal.o\
  derivative.o\
  determinant.o\
  division_quotient.o\
  division_remainder.o\
  division.o\
  dynamic_hierarchy.o\
  empty_expression.o\
  equal.o\
  evaluation.o\
  expression_lexer.o\
  expression_parser.o\
  expression.o\
  factor.o\
  factorial.o\
  floor.o\
  frac_part.o\
  global_context.o\
  great_common_divisor.o\
  hyperbolic_arc_cosine.o\
  hyperbolic_arc_sine.o\
  hyperbolic_arc_tangent.o\
  hyperbolic_cosine.o\
  hyperbolic_sine.o\
  hyperbolic_tangent.o\
  imaginary_part.o\
  integer.o\
  integral.o\
  layout_helper.o\
  list_data.o\
  least_common_multiple.o\
  logarithm.o\
  matrix_data.o\
  matrix_dimension.o\
  matrix_inverse.o\
  matrix_trace.o\
  matrix_transpose.o\
  matrix.o\
  multiplication.o\
  naperian_logarithm.o\
  nth_root.o\
  opposite.o\
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
  simplification_helper.o\
  sine.o\
  square_root.o\
  static_hierarchy.o\
  store.o\
  sum.o\
  symbol.o\
  tangent.o\
  trigonometry.o\
  undefined.o\
  variable_context.o\
)

tests += $(addprefix poincare/test/,\
  tree/tree_by_reference.cpp\
  addition.cpp\
  arithmetic.cpp\
  binomial_coefficient_layout.cpp\
  complex_to_expression.cpp\
  convert_expression_to_text.cpp\
  division.cpp\
  expression.cpp\
  factorial.cpp\
  fraction_layout.cpp\
  function.cpp\
  helper.cpp\
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
  vertical_offset_layout.cpp\
)

testsExpected += $(addprefix poincare/test/,\
  addition.cpp\
  arithmetic.cpp\
  binomial_coefficient_layout.cpp\
  complex_to_expression.cpp\
  convert_expression_to_text.cpp\
  division.cpp\
  factorial.cpp\
  fraction_layout.cpp\
  function.cpp\
  helper.cpp\
  integer.cpp\
  logarithm.cpp\
  matrix.cpp\
  multiplication.cpp\
  nth_root_layout.cpp\
  parser.cpp\
  power.cpp\
  properties.cpp\
  rational.cpp\
  simplify_mix.cpp\
  store.cpp\
  subtraction.cpp\
  symbol.cpp\
  trigo.cpp\
  vertical_offset_layout.cpp\
)
#  simplify_utils.cpp\

ifdef POINCARE_TESTS_PRINT_EXPRESSIONS
tests += poincare/src/expression_debug.o
objs += poincare/src/expression_debug.o
SFLAGS += -DPOINCARE_TESTS_PRINT_EXPRESSIONS=1
endif

ifdef POINCARE_TREE_LOG
SFLAGS += -DPOINCARE_TREE_LOG=1
endif

# Even though flex and bison will generate both implementation and headers at
# once, we don't declare it in the Makefile. If we did, "make -jN" with N>1 may
# call bison or flex twice.

lexer_files = $(addprefix poincare/src/, expression_lexer.cpp expression_lexer.hpp)
poincare/src/expression_lexer.hpp: poincare/src/expression_lexer.cpp
poincare/src/expression_lexer.cpp: poincare/src/expression_lexer.l
	@echo "FLEX    $(lexer_files)"
	$(Q) flex -P poincare_expression_yy --header-file=poincare/src/expression_lexer.hpp -o poincare/src/expression_lexer.cpp $<

poincare/src/expression_lexer.o: CXXFLAGS += -Wno-deprecated-register -Wno-unused-value -Wno-unused-function

parser_files = $(addprefix poincare/src/, expression_parser.cpp expression_parser.hpp)
poincare/src/expression_parser.hpp: poincare/src/expression_parser.cpp
poincare/src/expression_parser.cpp: poincare/src/expression_parser.y
	@echo "BISON   $(parser_files)"
	$(Q) bison -d -p poincare_expression_yy -o poincare/src/expression_parser.cpp $<

poincare/src/expression.cpp: $(lexer_files) $(parser_files)

products += $(lexer_files) $(parser_files)
