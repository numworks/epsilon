
PATH_haussmann := ..

OUTPUT_ROOT := out
DEBUG := 1
PLATFORM := macos

include $(PATH_haussmann)/Makefile

$(eval $(call declare_module,aaa,mod/aaa))

$(eval $(call declare_module,main,mod))
$(eval $(call define_module,main,\
	dummy.cpp:-do_something\
	main.cpp:+do_something\
))

$(eval $(call declare_goal,test,main.do_something aaa))
