products += apps/main.ast build/struct_layout/data.json

%.ast: %.cpp %.o
	@echo "AST     $@"
	@$(CXX) -fno-color-diagnostics -Xclang -fdump-record-layouts $(SFLAGS) $(CXXFLAGS) -MF /dev/null -c $< -o /dev/null > $@

%.ast.json: %.ast
	@echo "JSON    $@"
	@cat $< | ruby build/struct_layout/ast_to_json.rb AppsContainer > $@
