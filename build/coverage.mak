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
  $(patsubst %,'%/**',$(wildcard /Applications)) \
  $(patsubst %,'%/**',$(wildcard /Library))

# initialize_diagnosis, <test_category>, <coverage_dir>
define initialize_diagnosis
	rm -f $2/coverage_$1.info
	lcov --zerocounters --directory $2
endef

# TODO: No need to filter unit tests once they are all fixed.
# run_unit_tests, <test_bin>
define run_unit_tests
	./$1 --headless --limit-stack-usage -f poincare
	./$1 --headless --limit-stack-usage -f pcj
endef

# run_screenshot_tests, <epsilon_bin>
define run_screenshot_tests
	for state_file in tests/screenshots_dataset/*/*.nws; do ./output/debug/macos/arm64/coverage/coverage_epsilon.bin --headless --limit-stack-usage --load-state-file $$state_file; done
endef

# generate_coverage_info, <test_category>, <coverage_dir>
define generate_coverage_info
	lcov --capture --directory $2 --output-file $2/$1.info --ignore-errors inconsistent --filter range || (rm -f $2/$1.info; false)
	lcov --remove $2/$1.info $(_coverage_excludes) -o $2/$1.info --ignore-errors unused --ignore-errors inconsistent --filter range || (rm -f $2/$1.info; false)
endef

# rule_for_coverage_info,<coverage_dir>
define rule_for_coverage_info
$(eval \
coverage_info: $1/coverage_test.bin $1/coverage_epsilon.bin
	$(call initialize_diagnosis,all_tests,$1)
	$(call run_screenshot_tests,$$(word 2,$$^))
	$(call run_unit_tests,$$(word 2,$$^))
	$(call generate_coverage_info,all_tests,$1)

	$(call initialize_diagnosis,screenshot_tests,$1)
	$(call run_screenshot_tests,$$(word 2,$$^))
	$(call generate_coverage_info,screenshot_tests,$1)

	$(call initialize_diagnosis,unit_tests,$1)
	$(call run_unit_tests,$$(word 2,$$^))
	$(call generate_coverage_info,unit_tests,$1)
)
endef

# generate_diagnosis, <test_category>, <coverage_dir>
define generate_diagnosis
	genhtml $2/$1.info -s --legend --output-directory $2/diagnosis_$1 --ignore-errors inconsistent --filter range
	open $2/diagnosis_$1/index.html
endef

.PHONY: coverage

# Checks whether ARCHS is composed of several words. The coverage target is invalid if there are more than one architecture.
ifneq ($(findstring $( ),$(ARCHS)),)
coverage:
	$(error Several archs exist for platform, select one by overriding the ARCHS variable)
else
$(call rule_for_coverage_info,$(OUTPUT_DIRECTORY)/$(if $(ARCHS),$(ARCHS)/,)coverage) \
$(call rule_for_diagnosis,$(OUTPUT_DIRECTORY)/$(if $(ARCHS),$(ARCHS)/,)coverage)
coverage: coverage_info
	$(call generate_diagnosis,unit_tests,$(OUTPUT_DIRECTORY)/$(if $(ARCHS),$(ARCHS)/,)coverage)
	$(call generate_diagnosis,screenshot_tests,$(OUTPUT_DIRECTORY)/$(if $(ARCHS),$(ARCHS)/,)coverage)
	$(call generate_diagnosis,all_tests,$(OUTPUT_DIRECTORY)/$(if $(ARCHS),$(ARCHS)/,)coverage)
endif

$(call document_other_target,coverage,Generate a coverage diagnosis by running unit and screenshot tests)
