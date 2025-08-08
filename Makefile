SUBDIRS := epsilon scandium poincare

.PHONY: subdir_combined_make_call
subdir_combined_make_call:
	@for subdir in $(SUBDIRS); do \
		targets=`echo $(MAKECMDGOALS) | xargs -n1 | grep -E "^$${subdir}" | xargs`; \
		if [ -n "$$targets" ]; then \
			$(MAKE) -C $$subdir $$targets; \
		fi; \
	done

# Using format from epsilon is a simple hack to avoid setting up haussmann here
format:
	@ $(MAKE) -C epsilon ROOT=`pwd` $@

reformat:
	@ $(MAKE) -C epsilon ROOT=`pwd` $@

clean:
	@ $(MAKE) -C epsilon $@
	@ $(MAKE) -C scandium $@
	@ $(MAKE) -C poincare $@

%: subdir_combined_make_call
	@:
