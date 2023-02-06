ifeq ($(DEBUG),1)
ifeq ($(ASSERTIONS),0)
$(error ASSERTIONS cannot be equal to 0 when DEBUG=1)
endif
endif

SFLAGS += -DDEBUG=$(DEBUG)
SFLAGS += -DASSERTIONS=$(ASSERTIONS)
LDFLAGS += -Wl,--defsym ASSERTIONS=$(ASSERTIONS)

# This is for libc and micropython. Prefer using ASSERTIONS for epsilon
ifeq ($(ASSERTIONS),0)
SFLAGS += -DNDEBUG
endif
