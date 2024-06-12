# How to maintain this Makefile
# - Copy PY_CORE_O_BASENAME from py.mk into _sources_python_py
# - Copy select PY_EXTMOD_O_BASENAME from py.mk into _sources_python_extmod
# - Edit special-case workarounds below as needed
_sources_python_py := $(addprefix src/py/, \
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

_sources_python_extmod := $(addprefix src/extmod/, \
  modurandom.c \
)

_sources_python_port := $(addprefix port/, \
  port.c \
  builtins.c \
  helpers.c \
  mphalport.c \
$(addprefix mod/, \
  ion/modion.cpp \
  ion/modion_table.cpp \
  kandinsky/modkandinsky.cpp \
  kandinsky/modkandinsky_table.c \
  matplotlib/modmatplotlib.cpp \
  matplotlib/modmatplotlib_table.c \
  matplotlib/pyplot/modpyplot.cpp \
  matplotlib/pyplot/modpyplot_table.c \
  matplotlib/pyplot/plot_controller.cpp \
  matplotlib/pyplot/plot_store.cpp \
  matplotlib/pyplot/pyplot_view.cpp \
  ndarray.c \
  ndarray_operators.c \
  ndarray_properties.c \
  numpy/approx.c \
  numpy/carray/carray_tools.c \
  numpy/compare.c \
  numpy/create.c \
  numpy/filter.c \
  numpy/linalg/linalg_tools.c \
  numpy/ndarray/ndarray_iter.c \
  numpy/numerical.c \
  numpy/numpy.c \
  numpy/poly.c \
  numpy/stats.c \
  numpy/transform.c \
  numpy/vector.c \
  time/modtime.c \
  time/modtime_table.c \
  turtle/modturtle.cpp \
  turtle/modturtle_table.c \
  turtle/turtle.cpp \
  ulab.c \
  ulab_tools.c \
))

_sources_python_test := $(patsubst %,test/%:+test, \
  basics.cpp \
  execution_environment.cpp \
  ion.cpp \
  kandinsky.cpp \
  math.cpp \
  matplotlib.cpp \
  numpy.cpp \
  random.cpp \
  time.cpp \
  turtle.cpp \
)

# Workarounds - begin

# Rename urandom to random
$(call all_objects_for,$(PATH_python)/src/py/objmodule.c): SFLAGS += -DMP_QSTR_urandom="MP_QSTR_random"
$(call all_objects_for,$(PATH_python)/src/extmod/modurandom.c): SFLAGS += -DMP_QSTR_urandom="MP_QSTR_random"

# Dummy headers to satisfy Ulab module unused files includes like
# "numpy/carray/carray.h"
$(call all_objects_for,$(PATH_python)/port/mod/%.c): SFLAGS += -I$(PATH_python)/port/mod/include
# Useful for both "../scipy/signal/signal.h" and "carray/carray.h"
$(call all_objects_for,$(PATH_python)/port/mod/%.c): SFLAGS += -I$(PATH_python)/port/mod/include/numpy

# Handle upward-growing stack
# Some platforms such as emscripten have a stack that grows up. We've rewritten
# the stack control file to handle this case.
_sources_python_py := $(filter-out src/py/stackctrl.c,$(_sources_python_py))
_sources_python_port += port/stackctrl.c

# Fix the GC on emscripten
# With optimizations, register and stack variables might be held in a JavaScript
# local variables, which breaks garbage collection. Indeed, these JavaScript
# variables cannot be marked as root during garbage collection, which means that
# the heap objects they depend on will likely be destroyed. When the Python
# computing resumes, if necessary heap objects have been destroyed, the Python
# program crashes.
# This fix takes advantage of the fact that the last optimization flag takes
# priority.
ifeq ($(TOOLCHAIN),emscripten)
$(call all_objects_for,$(_sources_python_py)): SFLAGS += -O0
endif

# Workarounds - end

$(call create_module,python,1, \
  $(_sources_python_py) \
  $(_sources_python_extmod) \
  $(_sources_python_port) \
  $(_sources_python_test) \
)

# Override the default python include path
SFLAGS_python := \
  -I$(PATH_python)/src \
  -I$(PATH_python)/port \
  -I$(OUTPUT_DIRECTORY)/$(PATH_python)/port

SFLAGS_python += -DULAB_CONFIG_FILE="\"numworks_ulab_config.h\""

# QSTR generation

PYTHON_QSTRDEFS := $(OUTPUT_DIRECTORY)/$(PATH_python)/port/genhdr/qstrdefs.generated.h

$(PYTHON_QSTRDEFS): $(PATH_python)/port/genhdr/qstrdefs.in.h $(PATH_python)/src/py/makeqstrdata.py | $$(@D)/.
	$(call rule_label,QSTRDAT)
	$(QUIET) $(PYTHON) $(filter %.py,$^) $< > $@

$(call all_objects_for,$(SOURCES_python)): $(PYTHON_QSTRDEFS)
