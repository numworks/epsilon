products += apps/main.ast build/struct_layout/data.json

.PHONY: apps_container_struct_layout
OPEN = open

ifeq ($(CXX),clang++)

%.ast: %.cpp %.o
	@echo "AST     $@"
	@$(CXX) -fno-color-diagnostics -Xclang -fdump-record-layouts $(SFLAGS) $(CXXFLAGS) -MF /dev/null -c $< -o /dev/null > $@

%.ast.json: %.ast
	@echo "JSON    $@"
	@cat $< | ruby build/struct_layout/ast_to_json.rb AppsContainer > $@

apps_container_struct_layout: apps/main.ast.json
	$(OPEN) build/struct_layout/visualization.html

else

apps_container_struct_layout:
	@echo "Struct layout requires the use of Clang"

endif
