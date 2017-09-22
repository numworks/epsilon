rulegen_dir=poincare/src/simplification/rulegen

rulegen_objs := $(addprefix $(rulegen_dir)/,\
  rules_parser.o\
  rules_lexer.o\
  node.o\
  rule.o\
)

$(rulegen_dir)/rules_parser.cpp: $(rulegen_dir)/rules_parser.y
	@echo "BISON   $@"
	bison --defines=$(rulegen_dir)/rules_tokens.h $< -o $@

$(rulegen_dir)/rules_lexer.cpp: $(rulegen_dir)/rules_lexer.l $(rulegen_dir)/rules_parser.cpp
	@echo "FLEX    $@"
	@flex -o $@ $<

RULEGEN := $(rulegen_dir)/rulegen

products += $(rulegen_objs) $(RULEGEN) $(addprefix $(rulegen_dir)/,\
  rules_tokens.h\
  rules_parser.cpp\
  rules_lexer.cpp\
)

GENERATOR_CXXFLAGS = -std=c++11 -Wno-deprecated-register

$(rulegen_objs): %.o: %.cpp
	@echo "HOSTCC  $@"
	@$(HOSTCXX) $(GENERATOR_CXXFLAGS) -c $< -o $@

$(RULEGEN): $(rulegen_objs)
	@echo "HOSTLD  $@"
	@$(HOSTCXX) $(rulegen_objs) -o $@
