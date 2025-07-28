CXXLINT = build/tidy/cxxlint.py
CLANG_TIDY = clang-tidy
TIDY_CXXFLAGS = -std=c++17 -fno-rtti -fPIC
ifdef LLVM_PATH
LLVM_REAL_PATH = $(shell realpath $(LLVM_PATH))
TIDY_CXXFLAGS += -I$(LLVM_REAL_PATH)/include -L$(LLVM_REAL_PATH)/lib
export CLANG_TIDY = $(LLVM_REAL_PATH)/bin/clang-tidy
endif

output/host/tidy.lib: build/tidy/module.cpp build/tidy/reset_globals/check.cpp
	$(Q) mkdir -p output/host
	$(call rule_label,TIDYLIB)
	$(Q) $(LLVM_REAL_PATH)/bin/clang++ $(TIDY_CXXFLAGS) $^ -shared -o $@ -Wl,-undefined,dynamic_lookup -lclangTidy 2>/dev/null

$(all_objs): output/host/tidy.lib

.PHONY: tidy_check
tidy_check: output/host/tidy.lib
	$(CLANG_TIDY) -load=output/host/tidy.lib --config-file=build/tidy/config.yml build/tidy/reset_globals/test.cpp --
