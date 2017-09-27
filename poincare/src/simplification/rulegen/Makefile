prefix = poincare/src/simplification/rulegen

rulegen_objs := $(addprefix $(prefix)/,\
  rules_parser.o\
  rules_lexer.o\
  node.o\
  rule.o\
)

# Note: we cannot use $prefix in the receipe because Make expands it when the
# command is run, at which time prefix might have been replaced.
$(prefix)/rules_parser.cpp: $(prefix)/rules_parser.y
	@echo "BISON   $@"
	@bison --defines=poincare/src/simplification/rulegen/rules_tokens.h $< -o $@

$(prefix)/rules_lexer.cpp: $(prefix)/rules_lexer.l $(prefix)/rules_parser.cpp
	@echo "FLEX    $@"
	@flex -o $@ $<

RULEGEN := $(prefix)/rulegen

products += $(rulegen_objs) $(RULEGEN) $(addprefix $(prefix)/,\
  rules_tokens.h\
  rules_parser.cpp\
  rules_lexer.cpp\
)

GENERATOR_CXXFLAGS = -std=c++11 -Wno-deprecated-register -Iion/include

$(rulegen_objs): %.o: %.cpp
	@echo "HOSTCC  $@"
	@$(HOSTCXX) $(GENERATOR_CXXFLAGS) -c $< -o $@

$(RULEGEN): $(rulegen_objs)
	@echo "HOSTLD  $@"
	@$(HOSTCXX) $(rulegen_objs) -o $@
