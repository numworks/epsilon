CC = gcc
CXX = g++
LD = g++
EXE = exe

SFLAGS += -D_USE_MATH_DEFINES
LDFLAGS += -static -mwindows

# Work around command-line length limit
# On Msys2 the max command line is 32 000 characters. Our standard LD command
# can be longer than that because we have quite a lot of object files. To work
# around this issue, we write the object list in a "target.objs" file, and tell
# the linker to read its arguments from this file.
$(BUILD_DIR)/%.$(EXE):
	$(Q) echo $^ > $@.objs
	@echo "LD      $(@:$(BUILD_DIR)/%=%)"
	$(Q) $(LD) @$@.objs $(LDFLAGS) -o $@
	$(Q) rm $@.objs
