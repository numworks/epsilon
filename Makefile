ifeq ($(findstring multiple,${MAKECMDGOALS}),multiple)

SUBDIRS := epsilon scandium poincare

multiple:
	@for subdir in $(SUBDIRS); do \
		targets=`echo $(MAKECMDGOALS) | xargs -n1 | grep -E "^$${subdir}" | xargs`; \
		if [ -n "$$targets" ]; then \
			echo Running: make -C $$subdir $$targets; \
			$(MAKE) -C $$subdir $$targets; \
		fi; \
	done

%:
	@:
else

epsilon%:
	@ $(MAKE) -C epsilon $@

scandium%:
	@ $(MAKE) -C scandium $@

poincare%:
	@ $(MAKE) -C poincare $@

# Using format from epsilon is a simple hack to avoid setting up haussmann here
format:
	@ $(MAKE) -C epsilon ROOT=`pwd` $@

reformat:
	@ $(MAKE) -C epsilon ROOT=`pwd` $@

clean:
	@ $(MAKE) -C epsilon $@
	@ $(MAKE) -C scandium $@
	@ $(MAKE) -C poincare $@
endif
