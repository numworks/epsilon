CXXLINT = build/tidy/cxxlint.py
TIDY_CXXFLAGS = -std=c++17 -fno-rtti
ifdef CLANG_PATH
TIDY_CXXFLAGS += -I$(CLANG_PATH)/include -L$(CLANG_PATH)/lib
endif

output/host/tidy.lib: build/tidy/module.cpp build/tidy/reset_globals/check.cpp
	mkdir -p output/host
	SDKROOT=`xcrun --show-sdk-path` clang++ $(TIDY_CXXFLAGS) $^ -shared -o $@ -lclang -lclang-cpp -lclangTidy

$(all_objs): output/host/tidy.lib

.PHONY: tidy_check
tidy_check: output/host/tidy.lib
	clang-tidy -load=output/host/tidy.lib --config-file=build/tidy/config.yml build/tidy/reset_globals/test.cpp --
