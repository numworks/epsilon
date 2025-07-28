# Define the DEBUG and ASSERTIONS flags. This can't be done in defaults.mak
# since ASSERTIONS could have been altered by targets.mak or toolchain.mak,
# after defaults.mak was applied.

SFLAGS += -DDEBUG=$(DEBUG)
SFLAGS += -DASSERTIONS=$(ASSERTIONS)

ifeq ($(PLATFORM),device)
LDFLAGS += -Wl,--defsym,ASSERTIONS=$(ASSERTIONS)
endif

# This is for libc and micropython. Prefer using ASSERTIONS for epsilon
ifeq ($(ASSERTIONS),0)
SFLAGS += -DNDEBUG
endif
