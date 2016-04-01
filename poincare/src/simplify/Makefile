include poincare/src/simplify/rules_generation/Makefile

dir=poincare/src/simplify

$(dir)/rules.cpp: $(RULEGEN) $(dir)/rules.pr
	@echo "RULEGEN $@"
	$(RULEGEN) < $(dir)/rules.pr > $@

products += $(dir)/rules.cpp

objs += $(addprefix $(dir)/,\
  expression_builder.o\
  expression_match.o\
  expression_selector.o\
  rules.o\
  simplification.o\
  simplification_generator.o\
)
