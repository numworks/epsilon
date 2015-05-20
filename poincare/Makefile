SFLAGS += -Ipoincare/include
objs += $(addprefix poincare/src/, expression.o number.o fraction.o power.o expression_lexer.o expression_parser.o)

lexer_files = $(addprefix poincare/src/, expression_lexer.cpp expression_lexer.hpp)
$(lexer_files): poincare/src/expression_lexer.l poincare/src/expression_parser.hpp
	@echo "FLEX    $(lexer_files)"
	@flex -P poincare_expression_yy --header-file=poincare/src/expression_lexer.hpp -o poincare/src/expression_lexer.cpp $<

poincare/src/expression_lexer.o: CXXFLAGS += -Wno-deprecated-register -Wno-unused-value -Wno-unused-function

parser_files = $(addprefix poincare/src/, expression_parser.cpp expression_parser.hpp)
$(parser_files): poincare/src/expression_parser.y
	@echo "BISON   $(parser_files)"
	@bison -d -p poincare_expression_yy -o poincare/src/expression_parser.cpp $<

poincare/src/expression.cpp: $(lexer_files) $(parser_files)

products += $(lexer_files) $(parser_files)
