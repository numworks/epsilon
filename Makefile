SUBDIRS := epsilon scandium poincare

.PHONY: subdir_combined_make_call
subdir_combined_make_call:
	@matched=false; \
	for subdir in $(SUBDIRS); do \
		targets=`echo $(MAKECMDGOALS) | xargs -n1 | grep -E "^$${subdir}" | xargs`; \
		if [ -n "$$targets" ]; then \
			matched=true; \
			$(MAKE) -C $$subdir $$targets || exit 1; \
		fi; \
	done; \
	if [ "$$matched" = "false" ]; then \
		unmatched=`echo $(MAKECMDGOALS) | xargs -n1 | grep -vE "^(format|reformat|clean|help)$$" | xargs`; \
		if [ -n "$$unmatched" ]; then \
			echo "Warning: target doesn’t start with a known prefix ($(SUBDIRS))."; \
			echo "Did you mean: make -C epsilon $$unmatched ?"; \
		fi; \
	fi

# Using format from epsilon is a simple hack to avoid setting up haussmann here
format:
	@ $(MAKE) -C epsilon ROOT=`pwd` $@

reformat:
	@ $(MAKE) -C epsilon ROOT=`pwd` $@

clean:
	@ $(MAKE) -C epsilon $@
	@ $(MAKE) -C scandium $@
	@ $(MAKE) -C poincare $@

help:
	@ echo 'Run "make -C epsilon help" or "make -C scandium help"'

%: subdir_combined_make_call
	@:
