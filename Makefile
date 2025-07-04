epsilon%:
	$(MAKE) -C epsilon $@

scandium%:
	$(MAKE) -C scandium $@

format:
	$(MAKE) -C epsilon ROOT=`pwd` $@

reformat:
	$(MAKE) -C epsilon ROOT=`pwd` $@
