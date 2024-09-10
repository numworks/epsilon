ifeq ($(PLATFORM),android)
TERMS_OF_USE := 1
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

ifneq ($(filter android ios,$(PLATFORM)),)
EPSILON_TELEMETRY := 1
else
EPSILON_TELEMETRY := 0
endif

SFLAGS += \
  -DTERMS_OF_USE=$(TERMS_OF_USE) \
  -DEPSILON_GETOPT=$(EPSILON_GETOPT) \
  -DEPSILON_TELEMETRY=$(EPSILON_TELEMETRY)
