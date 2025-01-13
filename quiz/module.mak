$(call create_module,quiz,1,$(addprefix src/, \
  assertions.cpp \
  i18n.cpp \
  runner.cpp \
  runner_helpers_pool.cpp:-nopool \
  runner_helpers_no_pool.cpp:+nopool \
  stopwatch.cpp \
  test_symbols.c \
))

ifeq ($(TOOLCHAIN),emscripten)
LDFLAGS_quiz := -s MODULARIZE=0
endif

# List of the files that may contain a QUIZ_CASE and should cause test_symbols.c
# to be rebuilt. It is coarser than using goals but it is hard to have
# prerequisites that depend on the current goal.
# TODO: Each app has to be nammed here to avoid (slowly) searching in output.
#       Find a better solution.
$(call assert_defined,QUIZ_cases_directories)
_test_sources := $(shell find $(QUIZ_cases_directories) -path "**/test/**.c" -or -path "**/test/**.cpp")

# TODO Requires :+test to be the last taste
$(OUTPUT_DIRECTORY)/$(PATH_quiz)/src/test_symbols.c: $(_test_sources) | $$(@D)/.
	$(call rule_label,AWK)
	awk -v QUIZ_TEST_FILTER=$(QUIZ_TEST_FILTER) -f $(PATH_quiz)/src/symbols.awk \
		$(foreach m,$(filter %.test,$(MODULES_$(GOAL))), \
			$(call tasteless_filter,$(filter %:+test,$(SOURCES_$(firstword $(subst ., ,$m)))))) \
		> $@

$(call all_objects_for,$(PATH_quiz)/src/test_symbols.c): SFLAGS += -I$(PATH_quiz)/src

$(call assert_defined,KANDINSKY_fonts_dependencies)
$(call all_objects_for,$(SOURCES_quiz)): $(KANDINSKY_fonts_dependencies)
