ifneq (,$(findstring com.numworks.code,${APPS_LIST}))

app_objs += $(addprefix apps/code/,\
  app.o\
  editor_controller.o\
  executor_controller.o\
  menu_controller.o\
  program.o\
)

tests += $(addprefix apps/code/test/,\
  mpprint.cpp\
)

app_images += apps/code/code_icon.png

endif
