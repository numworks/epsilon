$(call create_module,quiz,1,$(addprefix src/, \
  cases.cpp \
  runner.cpp \
  runner_perf_counter.cpp:+perf_counter \
  runner_arguments.cpp:+arguments \
  runner_no_arguments.cpp:-arguments \
  runner_exception.cpp:+exception \
  runner_no_exception.cpp:-exception \
  util.cpp \
))

ifeq ($(TOOLCHAIN),emscripten)
LDFLAGS_quiz := -s MODULARIZE=0
endif

$(call assert_defined,KANDINSKY_fonts_dependencies)
$(call all_objects_for,$(SOURCES_quiz)): $(KANDINSKY_fonts_dependencies)
