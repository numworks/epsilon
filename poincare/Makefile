SFLAGS += -Ipoincare/include

include poincare/src/simplify/Makefile

objs += $(addprefix poincare/src/,\
  addition.o\
  binary_operation.o\
  commutative_operation.o\
  context.o\
  cosine.o\
  expression.o\
  expression_lexer.o\
  expression_parser.o\
  float.o\
  fraction.o\
  function.o\
  integer.o\
  leaf_expression.o\
  power.o\
  product.o\
  sine.o\
  subtraction.o\
  symbol.o\
  tangent.o\
)
objs += $(addprefix poincare/src/layout/,\
  exponent_layout.o\
  expression_layout.o\
  fraction_layout.o\
  horizontal_layout.o\
  string_layout.o\
)

tests += $(addprefix poincare/test/,\
  addition.cpp\
  fraction.cpp\
  integer.cpp\
  product.cpp\
  simplify.cpp\
  simplify_addition_integer.cpp\
  trigo.cpp\
  subtraction.cpp\
)

# We won't use this very often but the linker should be smart enough to just
# remove it if we don't use it.
tests += poincare/src/expression_debug.o

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
