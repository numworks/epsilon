SFLAGS += -Ipoincare/include

#include poincare/src/simplify/Makefile
#include poincare/src/simplification/Makefile

objs += $(addprefix poincare/src/,\
  absolute_value.o\
  addition.o\
  approximation_engine.o\
  arc_cosine.o\
  arc_sine.o\
  arc_tangent.o\
  arithmetic.o\
  binomial_coefficient.o\
  bounded_static_hierarchy.o\
  ceiling.o\
  complex.o\
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
  layout_engine.o\
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
  simplification_engine.o\
  sine.o\
  square_root.o\
  static_hierarchy.o\
  store.o\
  subtraction.o\
  sum.o\
  symbol.o\
  tangent.o\
  trigonometry.o\
  undefined.o\
  variable_context.o\
)

objs += $(addprefix poincare/src/layout/,\
  baseline_relative_layout.o\
  bracket_layout.o\
  condensed_sum_layout.o\
  conjugate_layout.o\
  expression_layout.o\
  fraction_layout.o\
  grid_layout.o\
  horizontal_layout.o\
  integral_layout.o\
  nth_root_layout.o\
  parenthesis_layout.o\
  product_layout.o\
  sequence_layout.o\
  string_layout.o\
  sum_layout.o\
)

tests += $(addprefix poincare/test/,\
  addition.cpp\
  arithmetic.cpp\
  complex.cpp\
  convert_expression_to_text.cpp\
  division.cpp\
  factorial.cpp\
  function.cpp\
  helper.cpp\
  integer.cpp\
  logarithm.cpp\
  matrix.cpp\
  multiplication.cpp\
  parser.cpp\
  power.cpp\
  properties.cpp\
  rational.cpp\
  simplify_mix.cpp\
  subtraction.cpp\
  symbol.cpp\
  store.cpp\
  trigo.cpp\
)
#  simplify_utils.cpp\

ifdef POINCARE_TESTS_PRINT_EXPRESSIONS
tests += poincare/src/expression_debug.o
objs += poincare/src/expression_debug.o
SFLAGS += -DPOINCARE_TESTS_PRINT_EXPRESSIONS=1
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
