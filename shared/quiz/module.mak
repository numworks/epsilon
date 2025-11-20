$(call create_module,quiz,1,$(addprefix src/, \
  assertions.cpp \
  i18n.cpp:+epsilon \
  symbols.cpp \
  runner.cpp \
  runner_helpers.cpp \
  runner_helpers_epsilon.cpp:+epsilon \
  runner_helpers_scandium.cpp:+scandium \
  stopwatch.cpp \
))

ifeq ($(TOOLCHAIN),emscripten)
LDFLAGS_quiz := -s MODULARIZE=0
endif

$(call assert_defined,KANDINSKY_fonts_dependencies)
$(call all_objects_for,$(SOURCES_quiz)): $(KANDINSKY_fonts_dependencies)
