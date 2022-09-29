SFLAGS += -Ipython/src
SFLAGS += -Ipython/port
SFLAGS += -I$(BUILD_DIR)/python/port

# How to maintain this Makefile
# - Copy PY_CORE_O_BASENAME from py.mk into py_src
# - Copy select PY_EXTMOD_O_BASENAME from py.mk into extmod_src
# - Edit special-case workarounds below as needed

py_src = $(addprefix python/src/py/,\
  mpstate.c \
  nlr.c \
  nlrx86.c \
  nlrx64.c \
  nlrthumb.c \
  nlraarch64.c \
  nlrpowerpc.c \
  nlrxtensa.c \
  nlrsetjmp.c \
  malloc.c \
  gc.c \
  pystack.c \
  qstr.c \
  vstr.c \
  mpprint.c \
  unicode.c \
  mpz.c \
  reader.c \
  lexer.c \
  parse.c \
  scope.c \
  compile.c \
  emitcommon.c \
  emitbc.c \
  asmbase.c \
  asmx64.c \
  emitnx64.c \
  asmx86.c \
  emitnx86.c \
  asmthumb.c \
  emitnthumb.c \
  emitinlinethumb.c \
  asmarm.c \
  emitnarm.c \
  asmxtensa.c \
  emitnxtensa.c \
  emitinlinextensa.c \
  emitnxtensawin.c \
  formatfloat.c \
  parsenumbase.c \
  parsenum.c \
  emitglue.c \
  persistentcode.c \
  runtime.c \
  runtime_utils.c \
  scheduler.c \
  nativeglue.c \
  pairheap.c \
  ringbuf.c \
  stackctrl.c \
  argcheck.c \
  warning.c \
  profile.c \
  map.c \
  obj.c \
  objarray.c \
  objattrtuple.c \
  objbool.c \
  objboundmeth.c \
  objcell.c \
  objclosure.c \
  objcomplex.c \
  objdeque.c \
  objdict.c \
  objenumerate.c \
  objexcept.c \
  objfilter.c \
  objfloat.c \
  objfun.c \
  objgenerator.c \
  objgetitemiter.c \
  objint.c \
  objint_longlong.c \
  objint_mpz.c \
  objlist.c \
  objmap.c \
  objmodule.c \
  objobject.c \
  objpolyiter.c \
  objproperty.c \
  objnone.c \
  objnamedtuple.c \
  objrange.c \
  objreversed.c \
  objset.c \
  objsingleton.c \
  objslice.c \
  objstr.c \
  objstrunicode.c \
  objstringio.c \
  objtuple.c \
  objtype.c \
  objzip.c \
  opmethods.c \
  sequence.c \
  stream.c \
  binary.c \
  builtinimport.c \
  builtinevex.c \
  builtinhelp.c \
  modarray.c \
  modbuiltins.c \
  modcollections.c \
  modgc.c \
  modio.c \
  modmath.c \
  modcmath.c \
  modmicropython.c \
  modstruct.c \
  modsys.c \
  moduerrno.c \
  modthread.c \
  vm.c \
  bc.c \
  showbc.c \
  repl.c \
  smallint.c \
  frozenmod.c \
)

extmod_src += $(addprefix python/src/extmod/,\
  modurandom.c \
)

port_src += $(addprefix python/port/,\
  port.c \
  builtins.c \
  helpers.c \
  mod/ion/modion.cpp \
  mod/ion/modion_table.cpp \
  mod/kandinsky/modkandinsky.cpp \
  mod/kandinsky/modkandinsky_table.c \
  mod/matplotlib/modmatplotlib.cpp \
  mod/matplotlib/modmatplotlib_table.c \
  mod/matplotlib/pyplot/modpyplot.cpp \
  mod/matplotlib/pyplot/modpyplot_table.c \
  mod/matplotlib/pyplot/plot_controller.cpp \
  mod/matplotlib/pyplot/plot_store.cpp \
  mod/matplotlib/pyplot/pyplot_view.cpp \
  mod/time/modtime.c \
  mod/time/modtime_table.c \
  mod/turtle/modturtle.cpp \
  mod/turtle/modturtle_table.c \
  mod/turtle/turtle.cpp \
  mphalport.c \
)

# Workarounds

# Rename urandom to random
# In order to change the name of the micropython module 'urandom' to 'random'
# (without altering micropython files), we redefined the macro MP_QSTR_urandom
# by DMP_QSTR_random.
$(call object_for,python/src/py/objmodule.c): SFLAGS += -DMP_QSTR_urandom="MP_QSTR_random"
$(call object_for,python/src/extmod/modurandom.c): SFLAGS += -DMP_QSTR_urandom="MP_QSTR_random"

# Handle upward-growing stack
# Some platforms such as emscripten have a stack that grows up. We've rewritten
# the stack control file to handle this case.
py_src := $(filter-out python/src/py/stackctrl.c, $(py_src))
port_src += python/port/stackctrl.c

# Fix the GC on emscripten
# With optimizations, register and stack variables might be held in a JavaScript
# local variables, which breaks garbage collection. Indeed, these JavaScript
# variables cannot be marked as root during garbage collection, which means that
# the heap objects they depend on will likely be destroyed. When the Python
# computing resumes, if necessary heap objects have been destroyed, the Python
# program crashes.
ifeq ($(PLATFORM),emscripten)
$(call object_for,$(py_src)): SFLAGS := $(subst -Os,-O0,$(SFLAGS))
endif

python_src = $(py_src) $(extmod_src) $(port_src)

# QSTR generation

$(eval $(call rule_for, \
  QSTRDAT, \
  python/port/genhdr/qstrdefs.generated.h, \
  python/port/genhdr/qstrdefs.in.h, \
  $$(PYTHON) python/src/py/makeqstrdata.py $$< > $$@, \
  global \
))

$(call object_for,$(python_src)): $(BUILD_DIR)/python/port/genhdr/qstrdefs.generated.h

tests_src += $(addprefix python/test/,\
  basics.cpp \
  execution_environment.cpp \
  ion.cpp \
  kandinsky.cpp \
  math.cpp \
  random.cpp \
  time.cpp \
  turtle.cpp \
  matplotlib.cpp \
)
