epsilon.$(EXE): $(objs) $(app_objs) $(app_image_objs) $(addprefix ion/src/blackbox/, boot.o events.o)

products += $(addprefix ion/src/blackbox/, boot.o events.o)
products += $(wildcard libepsilon_*.dylib)
products += $(wildcard ion/src/blackbox/library_*.o)

ion/src/blackbox/library_%.o: SFLAGS += -D EPSILON_LIB_PREFIX=$(*F)
ion/src/blackbox/library_%.o: ion/src/blackbox/library.cpp
	@echo "CXX     $@"
	$(Q) $(CXX) $(SFLAGS) $(CXXFLAGS) -c $< -o $@

libepsilon_%.dylib: LDFLAGS += -exported_symbols_list ion/src/blackbox/lib_export_list.txt
libepsilon_%.dylib: $(objs) $(app_objs) $(app_image_objs) ion/src/blackbox/library_%.o
	@echo "LD      $@"
	$(Q) $(LD) $^ $(LDFLAGS) -shared -s -o $@

compare.elf: ion/src/blackbox/compare.o libepsilon_first.dylib libepsilon_second.dylib
	@echo "LD      $@"
	$(Q) $(LD) $< $(LDFLAGS) -L. -lepsilon_first -lepsilon_second -o $@
