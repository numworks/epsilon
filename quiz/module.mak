$(call create_module,quiz,1,$(addprefix src/, \
  assertions.cpp \
  i18n.cpp \
  runner.cpp \
  stopwatch.cpp \
  test_symbols.c \
))

# TODO Requires :+test to be the last taste
# TODO Force remake because it's hard to have prerequisites that depend on the
# current goal
$(OUTPUT_DIRECTORY)/$(PATH_quiz)/src/test_symbols.c: | $$(@D)/.
	$(call rule_label,AWK)
	$(QUIET) awk -v QUIZ_TEST_FILTER=$(QUIZ_TEST_FILTER) -f $(PATH_quiz)/src/symbols.awk \
		$(foreach m,$(filter-out quiz quiz.%,$(MODULES_$(GOAL))), \
			$(call tasteless_filter,$(filter %:+test,$(SOURCES_$(firstword $(subst ., ,$m)))))) \
		> $@

$(call all_objects_for,$(PATH_quiz)/src/test_symbols.c): SFLAGS += -I$(PATH_quiz)/src

