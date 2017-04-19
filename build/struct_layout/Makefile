products += build/struct_layout/apps_container_layout.json
build/struct_layout/apps_container_layout.json:
	@echo "AST      $@"
	@clang -cc1 -x c++ -v -fdump-record-layouts apps/main.cpp -Iescher/include -Ikandinsky/include -Iion/include -Ipoincare/include -std=c++11 -emit-llvm-only -o /dev/null > foo.txt
	@echo "JSON     $@"
	@cat foo.txt | ruby build/struct_layout/ast_to_json.rb AppsContainer > $@
	@rm foo.txt

