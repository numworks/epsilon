SFLAGS += -Ipython/src
SFLAGS += -Ipython/port

# Native code emition is actually not needed
# It's only for the @micropython.native decorator
# which, to be honest, we don't really need

#BUILD = python/build

# How to write this makefile
# // Copy PY_O_BASENAME from py.mk and add those to objs
# // Add special-case optimizations


# We don't care about emitting multiple archs at once
# so we'll just build one emitnative.o
  #emitnx64.o \
  emitnx86.o \
  emitnthumb.o \
  emitnarm.o \
  emitnxtensa.o \
  emitinlinextensa.o \



# FIXME: Use customized optimization level for some files



py_objs = $(addprefix python/src/py/,\
  mpstate.o \
  nlrx86.o \
  nlrx64.o \
  nlrthumb.o \
  nlrxtensa.o \
  nlrsetjmp.o \
  malloc.o \
  gc.o \
  qstr.o \
  vstr.o \
  mpprint.o \
  unicode.o \
  mpz.o \
  reader.o \
  lexer.o \
  parse.o \
  scope.o \
  compile.o \
  emitcommon.o \
  emitbc.o \
  asmbase.o \
  asmx64.o \
  asmx86.o \
  asmthumb.o \
  emitinlinethumb.o \
  asmarm.o \
  asmxtensa.o \
  formatfloat.o \
  parsenumbase.o \
  parsenum.o \
  emitglue.o \
  persistentcode.o \
  runtime.o \
  runtime_utils.o \
  scheduler.o \
  nativeglue.o \
  argcheck.o \
  warning.o \
  map.o \
  obj.o \
  objarray.o \
  objattrtuple.o \
  objbool.o \
  objboundmeth.o \
  objcell.o \
  objclosure.o \
  objcomplex.o \
  objdict.o \
  objenumerate.o \
  objexcept.o \
  objfilter.o \
  objfloat.o \
  objfun.o \
  objgenerator.o \
  objgetitemiter.o \
  objint.o \
  objint_longlong.o \
  objint_mpz.o \
  objlist.o \
  objmap.o \
  objmodule.o \
  objobject.o \
  objpolyiter.o \
  objproperty.o \
  objnone.o \
  objnamedtuple.o \
  objrange.o \
  objreversed.o \
  objset.o \
  objsingleton.o \
  objslice.o \
  objstr.o \
  objstrunicode.o \
  objstringio.o \
  objtuple.o \
  objtype.o \
  objzip.o \
  opmethods.o \
  sequence.o \
  stream.o \
  binary.o \
  builtinimport.o \
  builtinevex.o \
  builtinhelp.o \
  modarray.o \
  modbuiltins.o \
  modcollections.o \
  modgc.o \
  modio.o \
  modmath.o \
  modcmath.o \
  modmicropython.o \
  modstruct.o \
  modsys.o \
  moduerrno.o \
  modthread.o \
  vm.o \
  bc.o \
  showbc.o \
  smallint.o \
  frozenmod.o \
)
#  repl.o \

py_objs += python/src/py/emitnative.o
python/src/py/emitnative.o: CFLAGS += -DN_THUMB


extmod_objs += $(addprefix python/src/extmod/,\
  modurandom.o \
)

# In order to change the name of the micropython module 'urandom' to 'random'
# (without altering micropython files), we redefined the macro MP_QSTR_urandom
# by DMP_QSTR_random.
python/src/py/objmodule.o: SFLAGS += -DMP_QSTR_urandom="MP_QSTR_random"
python/src/extmod/modurandom.o: SFLAGS += -DMP_QSTR_urandom="MP_QSTR_random"

#extmod_objs += $(addprefix python/src/extmod/,\
  moductypes.o \
  modujson.o \
  modure.o \
  moduzlib.o \
  moduheapq.o \
  modutimeq.o \
  moduhashlib.o \
  modubinascii.o \
  virtpin.o \
  machine_mem.o \
  machine_pinbase.o \
  machine_signal.o \
  machine_pulse.o \
  machine_i2c.o \
  machine_spi.o \
  modussl_axtls.o \
  modussl_mbedtls.o \
  modurandom.o \
  moduselect.o \
  modwebsocket.o \
  modwebrepl.o \
  modframebuf.o \
  vfs.o \
  vfs_reader.o \
  vfs_fat.o \
  vfs_fat_diskio.o \
  vfs_fat_file.o \
  vfs_fat_misc.o \
  utime_mphal.o \
  uos_dupterm.o \
)

port_objs += $(addprefix python/port/,\
  port.o \
  builtins.o\
  interrupt_helper.o \
  modkandinsky.o \
  modkandinsky_impl.o \
  mphalport.o \
  stackctrl.o \
)

# Reduce optimization for the emscripten platform.
# With optimization, register and stack variables might be held in a JavaScript
# local variable, which breaks garbage collection. Indeed, these JavaScript
# variables cannot be marked as root during garbage collection, which means that
# the heap objects they depend on will likely be destroyed. When the Python
# computing resumes, if necessary heap objects have been destroyed, the Python
# program crashes.

ifeq ($(PLATFORM),emscripten)
$(py_objs): SFLAGS := $(subst -Os,-O0,$(SFLAGS))
endif

# QSTR generation

python/port/genhdr/qstrdefs.generated.h: python/port/genhdr/qstrdefs.in.h
	@echo "QSTRDAT $@"
	$(Q) $(PYTHON) python/src/py/makeqstrdata.py $< > $@

products += python/port/genhdr/qstrdefs.generated.h

$(py_objs) $(extmod_objs) $(port_objs): python/port/genhdr/qstrdefs.generated.h

# List all objects needed

objs += $(extmod_objs) $(py_objs) $(port_objs)
