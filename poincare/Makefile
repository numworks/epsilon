SFLAGS += -Ipoincare/include

include poincare/src/simplify/Makefile

objs += $(addprefix poincare/src/,\
  absolute_value.o\
  addition.o\
  binary_operation.o\
  cosine.o\
  derivative.o\
  exponential.o\
  expression.o\
  expression_lexer.o\
  expression_parser.o\
  float.o\
  fraction.o\
  function.o\
  global_context.o\
  integer.o\
  integral.o\
  list_data.o\
  leaf_expression.o\
  logarithm.o\
  matrix.o\
  matrix_data.o\
  multiplication.o\
  n_context.o\
  nth_root.o\
  opposite.o\
  parenthesis.o\
  power.o\
  sine.o\
  subtraction.o\
  sum.o\
  symbol.o\
  tangent.o\
  x_context.o\
)
objs += $(addprefix poincare/src/layout/,\
  absolute_value_layout.o\
  exponent_layout.o\
  expression_layout.o\
  fraction_layout.o\
  horizontal_layout.o\
  integral_layout.o\
  matrix_layout.o\
  nth_root_layout.o\
  parenthesis_layout.o\
  product_layout.o\
  string_layout.o\
  sum_layout.o\
  subscript_layout.o\
  symbol_layout.o\
)

tests += $(addprefix poincare/test/,\
  addition.cpp\
  float.cpp\
  fraction.cpp\
  identity.cpp\
  integer.cpp\
  matrix.cpp\
  product.cpp\
  power.cpp\
  simplify_utils.cpp\
  subtraction.cpp\
  trigo.cpp\
)

# tests += $(addprefix poincare/test/,\
  addition.cpp\
  float.cpp\
  fraction.cpp\
  identity.cpp\
  integer.cpp\
  matrix.cpp\
  product.cpp\
  power.cpp\
  simplify_utils.cpp\
  simplify_addition.cpp\
  simplify_product.cpp\
  subtraction.cpp\
  trigo.cpp\
)

ifdef POINCARE_TESTS_PRINT_EXPRESSIONS
tests += poincare/src/expression_debug.o
SFLAGS += -DPOINCARE_TESTS_PRINT_EXPRESSIONS=1
endif

# Even though flex and bison will generate both implementation and headers at
# once, we don't declare it in the Makefile. If we did, "make -jN" with N>1 may
# call bison or flex twice.

lexer_files = $(addprefix poincare/src/, expression_lexer.cpp expression_lexer.hpp)
poincare/src/expression_lexer.hpp: poincare/src/expression_lexer.cpp
poincare/src/expression_lexer.cpp: poincare/src/expression_lexer.l
	@echo "FLEX    $(lexer_files)"
	@flex -P poincare_expression_yy --header-file=poincare/src/expression_lexer.hpp -o poincare/src/expression_lexer.cpp $<

poincare/src/expression_lexer.o: CXXFLAGS += -Wno-deprecated-register -Wno-unused-value -Wno-unused-function

parser_files = $(addprefix poincare/src/, expression_parser.cpp expression_parser.hpp)
poincare/src/expression_parser.hpp: poincare/src/expression_parser.cpp
poincare/src/expression_parser.cpp: poincare/src/expression_parser.y
	@echo "BISON   $(parser_files)"
	@bison -d -p poincare_expression_yy -o poincare/src/expression_parser.cpp $<

poincare/src/expression.cpp: $(lexer_files) $(parser_files)

products += $(lexer_files) $(parser_files)
