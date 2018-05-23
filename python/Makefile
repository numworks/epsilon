SFLAGS += -Ipython/src
SFLAGS += -Ipython/port

# How to maintain this Makefile
# - Copy PY_CORE_O_BASENAME from py.mk into py_objs
# - Copy select PY_EXTMOD_O_BASENAME from py.mk into extmod_objs
# - Edit special-case workarounds below as needed

py_objs = $(addprefix python/src/py/,\
  mpstate.o \
  nlr.o \
  nlrx86.o \
  nlrx64.o \
  nlrthumb.o \
  nlrxtensa.o \
  nlrsetjmp.o \
  malloc.o \
  gc.o \
  pystack.o \
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
  emitnx64.o \
  asmx86.o \
  emitnx86.o \
  asmthumb.o \
  emitnthumb.o \
  emitinlinethumb.o \
  asmarm.o \
  emitnarm.o \
  asmxtensa.o \
  emitnxtensa.o \
  emitinlinextensa.o \
  formatfloat.o \
  parsenumbase.o \
  parsenum.o \
  emitglue.o \
  persistentcode.o \
  runtime.o \
  runtime_utils.o \
  scheduler.o \
  nativeglue.o \
  stackctrl.o \
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
  objdeque.o \
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
  repl.o \
  smallint.o \
  frozenmod.o \
)

extmod_objs += $(addprefix python/src/extmod/,\
  modurandom.o \
)

port_objs += $(addprefix python/port/,\
  port.o \
  builtins.o\
  helpers.o \
  modkandinsky.o \
  modkandinsky_impl.o \
  mphalport.o \
)

# Workarounds

# Rename urandom to random
# In order to change the name of the micropython module 'urandom' to 'random'
# (without altering micropython files), we redefined the macro MP_QSTR_urandom
# by DMP_QSTR_random.
python/src/py/objmodule.o: SFLAGS += -DMP_QSTR_urandom="MP_QSTR_random"
python/src/extmod/modurandom.o: SFLAGS += -DMP_QSTR_urandom="MP_QSTR_random"

# Handle upward-growing stack
# Some platforms such as emscripten have a stack that grows up. We've rewritten
# the stack control file to handle this case.
py_objs := $(filter-out python/src/py/stackctrl.o, $(py_objs))
port_objs += python/port/stackctrl.o

# Fix the GC on emscripten
# With optimizations, register and stack variables might be held in a JavaScript
# local variables, which breaks garbage collection. Indeed, these JavaScript
# variables cannot be marked as root during garbage collection, which means that
# the heap objects they depend on will likely be destroyed. When the Python
# computing resumes, if necessary heap objects have been destroyed, the Python
# program crashes.
ifeq ($(PLATFORM),emscripten)
$(py_objs): SFLAGS := $(subst -Os,-O0,$(SFLAGS))
endif


# QSTR generation

generated_headers += $(addprefix python/port/genhdr/, qstrdefs.generated.h)

python/port/genhdr/qstrdefs.generated.h: python/port/genhdr/qstrdefs.in.h
	@echo "QSTRDAT $@"
	$(Q) $(PYTHON) python/src/py/makeqstrdata.py $< > $@

products += python/port/genhdr/qstrdefs.generated.h

$(py_objs) $(extmod_objs) $(port_objs): python/port/genhdr/qstrdefs.generated.h

# List all objects needed

objs += $(extmod_objs) $(py_objs) $(port_objs)
