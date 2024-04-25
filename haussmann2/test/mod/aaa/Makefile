$(eval $(call create_module,aaa,\
	a1.cpp:-alternate\
	a2.cpp:+alternate\
	forbidden.cpp:+forbidden\
))

PRIVATE_SFLAGS_aaa := -fno-lto
