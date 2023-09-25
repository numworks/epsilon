ifeq ($(PLATFORM),simulator)
SFLAGS += -Ipoincare_junior/omgpj/include
else
ifeq ($(FIRMWARE_COMPONENT),userland)
#omgpj/include is inserted in front of the list to override liba with its assert
SFLAGS := -Ipoincare_junior/omgpj/include $(SFLAGS)
endif
endif

omgpj_src += $(addprefix poincare_junior/omgpj/src/,\
  arithmetic.cpp \
  float.cpp \
  list.cpp \
  memory.cpp \
  unicode_helper.cpp \
)
