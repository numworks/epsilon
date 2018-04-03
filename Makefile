include build/config.mak

default: epsilon.$(EXE)

.PHONY: info
info:
	@echo "EPSILON_VERSION = $(EPSILON_VERSION)"
	@echo "EPSILON_ONBOARDING_APP = $(EPSILON_ONBOARDING_APP)"
	@echo "EPSILON_SOFTWARE_UPDATE_PROMPT = $(EPSILON_SOFTWARE_UPDATE_PROMPT)"
	@echo "EPSILON_APPS = $(EPSILON_APPS)"
	@echo "EPSILON_I18N = $(EPSILON_I18N)"

# Each sub-Makefile can either add objects to the $(objs) variable or define a
# new executable target. The $(objs) variable lists the objects that will be
# linked to every executable being generated. Each Makefile is also responsible
# for keeping the $(product) variable updated. This variable lists all files
# that could be generated during the build and that needs to be cleaned up
# afterwards.

products :=

# Library Makefiles
ifeq ($(USE_LIBA),0)
include liba/Makefile.bridge
else
SFLAGS += -ffreestanding -nostdinc -nostdlib
include liba/Makefile
include libaxx/Makefile
endif
include ion/Makefile
include kandinsky/Makefile
include poincare/Makefile
include python/Makefile
include escher/Makefile
# Executable Makefiles
include apps/Makefile
include build/struct_layout/Makefile
include quiz/Makefile # Quiz needs to be included at the end

products += $(objs)

all_objs = $(filter %.o, $(products))
dependencies = $(all_objs:.o=.d)
-include $(dependencies)
products += $(dependencies)

$(all_objs): $(generated_headers)

.SECONDARY: $(objs)
%.$(EXE): $(objs)
	@echo "LD      $@"
	$(Q) $(LD) $^ $(LDFLAGS) -o $@

%.o: %.c
	@echo "CC      $@"
	$(Q) $(CC) $(SFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.s
	@echo "AS      $@"
	$(Q) $(CC) $(SFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "CXX     $@"
	$(Q) $(CXX) $(SFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "CLEAN"
	$(Q) rm -f $(products)

.PHONY: cowsay_%
cowsay_%:
	@echo " -------"
	@echo "| $(*F) |"
	@echo " -------"
	@echo "        \\   ^__^"
	@echo "         \\  (oo)\\_______"
	@echo "            (__)\\       )\\/\\"
	@echo "                ||----w |"
	@echo "                ||     ||"

.PHONY: clena
clena: cowsay_CLENA clean

-include build/targets.$(PLATFORM).mak
