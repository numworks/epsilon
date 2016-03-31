all:
	bison --defines=rules_tokens.h rules_parser.y -o rules_parser.cpp
	clang++ -c -std=c++11 rules_parser.cpp
	flex -o rules_lexer.cpp rules_lexer.l
	clang++ -c -std=c++11 rules_lexer.cpp
	clang++ -c -std=c++11 rule.cpp
	clang++ -c -std=c++11 builder.cpp
	clang++ -c -std=c++11 selector.cpp
	clang++ -c -std=c++11 node.cpp
	clang++ rules_lexer.o rules_parser.o rule.o builder.o selector.o node.o -o parser

run:
	cat rules.pr | ./parser

clean:
	rm -f rules_token.h rules_lexer.cpp rules_parser.cpp parser *.o
