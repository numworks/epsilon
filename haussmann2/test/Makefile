
PATH_haussmann := ..

APP_NAME := Test
APP_VERSION := 0.0.0
OUTPUT_ROOT := out
DEBUG := 1
PLATFORM := macos

include $(PATH_haussmann)/Makefile

$(call import_module,aaa,mod/aaa)

$(call import_module,main,mod)
$(call create_module,main,1,\
	dummy.cpp:-do_something\
	main.cpp:+do_something\
)
SFLAGS_main += -time:-do_something

$(call create_goal,test,\
	main.do_something aaa,\
	Some test executable that should exit with an error\
)
$(call create_goal,test_dummy,main)

$(call create_zip,test.zip,$(OUTPUT_DIRECTORY)/arm64/test.bin)
