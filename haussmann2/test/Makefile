
include Makefile

$(eval $(call declare_module,aaa,test/mod/aaa))

$(eval $(call define_module,main,test/main.cpp))

$(eval $(call declare_goal,test,main aaa))
