# Define standard compilation rules

.PHONY: official_authorization
official_authorization:
	@echo "CAUTION: You are about to build an official NumWorks firmware. Distribution of such firmware by a third party is prohibited. Are you sure you want to proceed? Please type "yes" to confirm." && read ans && [ $${ans:-no} = yes ]

$(eval $(call rule_for, \
  AS, %.o, %.s, \
  $$(CC) $$(SFLAGS) -c $$< -o $$@ \
))

$(eval $(call rule_for, \
  CC, %.o, %.c, \
  $$(CC) $$(CFLAGS) $$(SFLAGS) -c $$< -o $$@, \
  with_local_version \
))

$(eval $(call rule_for, \
  CXX, %.o, %.cpp, \
  $$(CXX) $$(CXXFLAGS) $$(SFLAGS) -c $$< -o $$@, \
  with_local_version \
))

$(eval $(call rule_for, \
  OCC, %.o, %.m, \
  $$(CC) $$(CFLAGS) $$(SFLAGS) -c $$< -o $$@ \
))

$(eval $(call rule_for, \
  OCC, %.o, %.mm, \
  $$(CXX) $$(CXXFLAGS) $$(SFLAGS) -c $$< -o $$@ \
))

ifeq ($(OS),Windows_NT)
# Work around command-line length limit
# On Msys2 the max command line is 32 000 characters. Our standard LD command
# can be longer than that because we have quite a lot of object files. To work
# around this issue, we write the object list in a "target.objs" file, and tell
# the linker to read its arguments from this file.
$(eval $(call rule_for, \
  LD, %.$$(EXE), , \
  echo $$^ > $$@.objs && $$(LD) @$$@.objs $$(LDFLAGS) -o $$@ && rm $$@.objs \
))
else
$(eval $(call rule_for, \
  LD, %.$$(EXE), , \
  $$(LD) $$^ $$(LDFLAGS) -o $$@ \
))
endif

$(eval $(call rule_for, \
  WINDRES, %.o, %.rc, \
  $$(WINDRES) $$< -O coff -o $$@ \
))
