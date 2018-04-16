epsilon.$(EXE): $(objs) $(app_objs) $(app_image_objs) $(addprefix ion/src/blackbox/, boot.o events.o)

.PHONY: epsilon.lib
epsilon.lib: libepsilon_${EPSILON_LIB_PREFIX}.dylib


ion/src/blackbox/library.o: SFLAGS += -D EPSILON_LIB_PREFIX=${EPSILON_LIB_PREFIX}

libepsilon_${EPSILON_LIB_PREFIX}.dylib: LDFLAGS += -exported_symbols_list ion/src/blackbox/lib_export_list.txt
libepsilon_${EPSILON_LIB_PREFIX}.dylib: $(objs) $(app_objs) $(app_image_objs) ion/src/blackbox/library.o
	@echo "LD      $@"
	$(Q) $(LD) $^ $(LDFLAGS) -shared -s -o $@

compare.elf: ion/src/blackbox/compare.o libepsilon_first.dylib libepsilon_second.dylib
	@echo "LD      $@"
	$(Q) $(LD) $< $(LDFLAGS) -L. -lepsilon_first -lepsilon_second -o $@
