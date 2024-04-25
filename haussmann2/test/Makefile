
include Makefile

$(eval $(call declare_module,aaa,test/mod/aaa))

$(eval $(call define_module,main,\
	test/dummy.cpp:-do_something\
	test/main.cpp:+do_something\
))

$(eval $(call declare_goal,test,main.do_something aaa))
