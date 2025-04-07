$(call create_goal,coverage_epsilon,$(MODULES_epsilon),coverage)

$(call create_goal,coverage_test,$(MODULES_test),coverage)

$(call all_targets_named,coverage/%.bin): SFLAGS += --coverage

_coverage_excludes := \
  '**/eadk/**' \
  '**/external/**' \
  '*/output/**' \
  '**/test/**' \
  '**/python/src/**' \
  '**/quiz/src/**' \
  '/usr/**' \

# initialize_diagnosis, <file_name>, <coverage_dir>
define initialize_diagnosis
	@echo Cleaning coverage data in $2 and removing $2/$1.info
	rm -f $2/$1.info
	lcov --zerocounters --directory $2
endef

# run_unit_tests, <test_bin>
define run_unit_tests
	@echo Running unit tests with executable $1
	./$1 --headless --limit-stack-usage
endef

# run_screenshot_tests, <epsilon_bin>
define run_screenshot_tests
	@echo Running screenshot tests with executable $1
	python3 build/screenshots/compare_crc.py --no-screenshots --ignore-failure $1
endef

# generate_coverage_info, <file_name>, <coverage_dir>
define generate_coverage_info
	@echo Generating coverage info for files in $2. Result will be stored in $2/$1.info.
	lcov -j 32 --capture --directory $2 --output-file $2/$1.info \
	$$(foreach pattern,$$(_coverage_excludes),--exclude $$(pattern)) \
	--rc function_coverage=0 --rc geninfo_unexecuted_blocks=1
endef

# rule_for_coverage,<coverage_dir>
define rule_for_coverage
$(eval \
coverage: $1/coverage_test.bin $1/coverage_epsilon.bin
	$(call initialize_diagnosis,code_coverage,$1)
	$(call run_screenshot_tests,$$(word 2,$$^))
	$(call run_unit_tests,$$<)
	$(call generate_coverage_info,code_coverage,$1)
)
endef

.PHONY: coverage

ifneq ($(findstring $( ),$(ARCHS)),)
# Checks whether ARCHS is composed of several words. The coverage target is invalid if there are more than one architecture.
coverage:
	$(error Several archs exist for platform, select one by overriding the ARCHS variable)
else ifneq ($(TOOLCHAIN),host-gcc)
coverage:
	$(error The coverage target needs a gcc compiler)
else
$(call rule_for_coverage,$(OUTPUT_DIRECTORY)/$(if $(ARCHS),$(ARCHS)/,)coverage)
endif

$(call document_other_target,coverage,Generate a coverage diagnosis by running unit and screenshot tests)
