epsilon.$(EXE): $(objs) $(app_objs) $(app_image_objs) $(addprefix ion/src/blackbox/, boot.o events.o)

products += $(addprefix ion/src/blackbox/, boot.o events.o)
products += $(wildcard ion/src/blackbox/library_*.o)

ion/src/blackbox/library_%.o: SFLAGS += -D EPSILON_LIB_PREFIX=$(*F)
ion/src/blackbox/library_%.o: ion/src/blackbox/library.cpp
	@echo "CXX     $@"
	$(Q) $(CXX) $(SFLAGS) $(CXXFLAGS) -c $< -o $@

libepsilon_%.o: LDFLAGS += -exported_symbols_list ion/src/blackbox/lib_export_list.txt
libepsilon_%.o: $(objs) $(app_objs) $(app_image_objs) ion/src/blackbox/library_%.o
	@echo "LD      $@"
	$(Q) $(LD) $^ $(LDFLAGS) -r -s -o $@

compare: ion/src/blackbox/compare.o libepsilon_first.o libepsilon_second.o
	@echo "LD      $@"
	$(Q) $(LD) $^ $(LDFLAGS) -L. -o $@
