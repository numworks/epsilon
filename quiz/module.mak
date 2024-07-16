$(call create_module,quiz,1,$(addprefix src/, \
  assertions.cpp \
  i18n.cpp \
  runner.cpp \
  stopwatch.cpp \
  test_symbols.c \
))


# List of the files that may contain a QUIZ_CASE and should cause test_symbols.c
# to be rebuilt. It is coarser than using goals but it is hard to have
# prerequisites that depend on the current goal.
_test_sources := $(shell git ls-files -- "*/test/**.c" "*/test/**.cpp")

# TODO Requires :+test to be the last taste
$(OUTPUT_DIRECTORY)/$(PATH_quiz)/src/test_symbols.c: $(_test_sources) | $$(@D)/.
	$(call rule_label,AWK)
	awk -v QUIZ_TEST_FILTER=$(QUIZ_TEST_FILTER) -f $(PATH_quiz)/src/symbols.awk \
		$(foreach m,$(filter-out quiz quiz.%,$(MODULES_$(GOAL))), \
			$(call tasteless_filter,$(filter %:+test,$(SOURCES_$(firstword $(subst ., ,$m)))))) \
		> $@

$(call all_objects_for,$(PATH_quiz)/src/test_symbols.c): SFLAGS += -I$(PATH_quiz)/src

