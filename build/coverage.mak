$(call create_goal,coverage_epsilon, \
  apps \
  escher \
  ion \
  kandinsky \
  liba_bridge \
  omg \
  poincare \
  python \
  sdl \
  ,coverage, \
)

$(call create_goal,coverage_test, \
  apps.test \
  escher.test \
  ion.test \
  kandinsky.test \
  liba_bridge \
  omg.test \
  poincare.test \
  python.test \
  quiz \
  sdl \
  ,coverage, \
)

$(call all_targets_named,coverage/%.bin): SFLAGS += --coverage

_coverage_excludes := \
  '**/eadk/**' \
  '**/ion/src/simulator/external/**' \
  '*/output/**' \
  '**/python/src/**' \
  '/Library/**'

#TODO: no need to filter once unit tests are all fixed
_coverage_unit_test_filter := -f poincare

# rule_for_coverage, <arch_dir>
define rule_for_coverage
$(eval \
$(OUTPUT_DIRECTORY)/$1coverage/coverage.info: $(OUTPUT_DIRECTORY)/$1coverage/coverage_test.bin $(OUTPUT_DIRECTORY)/$1coverage/coverage_epsilon.bin
	./$$< --headless --limit-stack-usage $(_coverage_unit_test_filter)
	for state_file in tests/screenshots_dataset/*/*.nws; do ./$$(word 2,$$^) --headless --limit-stack-usage --load-state-file $$$$state_file; done
	lcov --capture --directory $$(@D) --output-file $$@ --ignore-errors inconsistent --filter range || (rm -f $@; false)
	lcov --remove $$@ $(_coverage_excludes) -o $$@ --ignore-errors inconsistent --filter range || (rm -f $@; false)
)
endef

ifneq ($(ARCHS),)
$(foreach a,$(ARCHS),$(call rule_for_coverage,$a/))
else
$(call rule_for_coverage,)
endif

# Checks whether ARCHS is composed of several words. The coverage target is invalid if there are more than one architecture.
ifneq ($(findstring $( ),$(ARCHS)),)
coverage:
	$(error Several archs exist for platform, select one by overriding the ARCHS variable.)
else
coverage: $(OUTPUT_DIRECTORY)/$(if $(ARCHS),$(ARCHS)/,)coverage/coverage.info
	genhtml $< -s --legend --output-directory $(<D)/diagnosis --ignore-errors inconsistent --filter range
	open  $(<D)/diagnosis/index.html
endif

$(call document_other_target,coverage,Generate a coverage diagnosis by running unit and screenshot tests)
