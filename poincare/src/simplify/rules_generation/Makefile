dir=poincare/src/simplify/rules_generation

rulegen_objs := $(addprefix $(dir)/,\
  rules_parser.o\
  rules_lexer.o\
  node.o\
  rule.o\
)

$(dir)/rules_parser.cpp: $(dir)/rules_parser.y
	@echo "BISON   $@"
	@bison --defines=poincare/src/simplify/rules_generation/rules_tokens.h $< -o $@

$(dir)/rules_lexer.cpp: $(dir)/rules_lexer.l $(dir)/rules_parser.cpp
	@echo "FLEX    $@"
	@flex -o $@ $<

RULEGEN := $(dir)/rulegen

products += $(rulegen_objs) $(RULEGEN) $(addprefix $(dir)/,\
  rules_tokens.h\
  rules_parser.cpp\
  rules_lexer.cpp\
)

GENERATOR_CXXFLAGS = -std=c++11 -Wno-deprecated-register

HOSTCC = clang
HOSTCXX = clang++

$(rulegen_objs): %.o: %.cpp
	@echo "HOSTCC  $@"
	@$(HOSTCXX) $(GENERATOR_CXXFLAGS) -c $< -o $@

$(RULEGEN): $(rulegen_objs)
	@echo "HOSTLD  $@"
	@$(HOSTCXX) $(rulegen_objs) -o $@
