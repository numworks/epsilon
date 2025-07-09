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
