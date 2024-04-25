
PATH_haussmann := ..

OUTPUT_ROOT := out
DEBUG := 1
PLATFORM := macos

include $(PATH_haussmann)/Makefile

$(eval $(call import_module,aaa,mod/aaa))

$(eval $(call import_module,main,mod))
$(eval $(call create_module,main,\
	dummy.cpp:-do_something\
	main.cpp:+do_something\
))

$(eval $(call create_goal,test,main.do_something aaa))
$(eval $(call create_goal,test_dummy,main))
