ifeq ($(PLATFORM),android)
TERMS_OF_USE := 1
# If MICROPY_NLR_SETJMP is 0, the MicroPython NLR is done by
# python/src/py/nlrthumb.c and creates code containing relocations, which is not
# accepted by Android.
SFLAGS += -DMICROPY_NLR_SETJMP=1
else
TERMS_OF_USE := 0
endif

ifeq ($(PLATFORM),foxglove)
EPSILON_GETOPT := 0
else
EPSILON_GETOPT := 1
endif

ifeq ($(PLATFORM),web)
ION_em_module_js := epsilon.js
endif

ifeq ($(PLATFORM),linux)
# If MICROPY_NLR_SETJMP is 0, the MicroPython NLR done by
# python/src/py/nlrx64.c crashes on linux.
SFLAGS += -DMICROPY_NLR_SETJMP=1
endif

SFLAGS += \
  -DTERMS_OF_USE=$(TERMS_OF_USE) \
  -DEPSILON_GETOPT=$(EPSILON_GETOPT)
