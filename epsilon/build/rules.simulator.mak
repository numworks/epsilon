ifeq ($(HOST),macos)
ifeq ($(CXX),g++)
$(error Epsilon cannot be built with the gcc compiler on macOS)
endif
endif

$(call import_module,sdl,$(PATH_ion)/src/simulator/external)

$(call create_goal,epsilon, \
  apps \
  eadk \
  escher \
  ion \
  kandinsky \
  liba_bridge \
  omg \
  poincare \
  python \
  sdl \
)

$(call create_goal,epsilon_test, \
  apps.test \
  eadk \
  escher.test \
  ion.test \
  kandinsky.test \
  liba_bridge \
  omg.test \
  poincare.test \
  python.test \
  quiz.epsilon \
  sdl \
)

COVERAGE_main_goal := epsilon
include $(PATH_haussmann)/src/rules/coverage.mak


