ifeq ($(DEBUG),1)
ifeq ($(ASSERTIONS),0)
$(error ASSERTIONS cannot be equal to 0 when DEBUG=1)
endif
endif

SFLAGS += -DDEBUG=$(DEBUG)
SFLAGS += -DASSERTIONS=$(ASSERTIONS)

ifeq ($(PLATFORM),device)
# This needs to be done here, in case the ASSERTIONS flag has been modified for
# the target
LDFLAGS += -Wl,--defsym ASSERTIONS=$(ASSERTIONS)
endif

# This is for libc and micropython. Prefer using ASSERTIONS for epsilon
ifeq ($(ASSERTIONS),0)
SFLAGS += -DNDEBUG
endif
