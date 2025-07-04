epsilon%:
	@ $(MAKE) -C epsilon $@

scandium%:
	@ $(MAKE) -C scandium $@

poincare%:
	@ $(MAKE) -C poincare $@

format:
	@ $(MAKE) -C epsilon ROOT=`pwd` $@

reformat:
	@ $(MAKE) -C epsilon ROOT=`pwd` $@
