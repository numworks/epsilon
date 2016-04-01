dir=poincare/src/simplify/rules_generation

rulegen_objs := $(addprefix $(dir)/,\
  rules_parser.o\
  rules_lexer.o\
  builder.o\
  node.o\
  rule.o\
  selector.o\
)

$(dir)/rules_parser.cpp: $(dir)/rules_parser.y
	@echo "BISON   $@"
	bison --defines=$(dir)/rules_tokens.h $< -o $@

$(dir)/rules_lexer.cpp: $(dir)/rules_lexer.l $(dir)/rules_parser.cpp
	@echo "FLEX    $@"
	flex -o $@ $<

rulegen := $(dir)/rulegen

products += $(rulegen_objs) $(rulegen) $(addprefix $(dir)/,\
  rules_tokens.h\
  rules_parser.cpp\
  rules_lexer.cpp\
)

GENERATOR_CXXFLAGS = -std=c++11

HOSTCC = clang
HOSTCXX = clang++

$(rulegen_objs): %.o: %.cpp
	@echo "HOSTCC  $@"
	$(HOSTCXX) $(GENERATOR_CXXFLAGS) -c $< -o $@

$(rulegen): $(rulegen_objs)

# Now let's use the rulegen

$(dir)/rules.cpp: $(dir)/rules.pr
	@echo "RULEGEN $@"
	$(dir)/rulegen < $< > $@

generator_run:
	cat rules.pr | ./parser
