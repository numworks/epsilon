.PHONY: apps_struct_layout
OPEN = open

ifeq ($(PLATFORM) $(TARGET) $(CXX),simulator macos clang++)

OUTPUT := output/struct_layout

.PHONY: $(OUTPUT)/records_layout.ast
$(OUTPUT)/records_layout.ast: apps/apps_container_storage.cpp
	@echo "AST     $@"
	@mkdir -p $(OUTPUT)
	@$(MAKE) epsilon.bin
	@$(CXX) -fno-color-diagnostics -Xclang -fdump-record-layouts $(SFLAGS) $(CXXFLAGS) $(snapshot_includes) -DAPPS_CONTAINER_SNAPSHOT_CONSTRUCTORS="$(snapshots_construction)" -DAPPS_CONTAINER_SNAPSHOT_LIST="$(snapshots_list)" -DAPPS_CONTAINER_SNAPSHOT_COUNT=$(snapshots_count) -DAPPS_CONTAINER_SNAPSHOT_DECLARATIONS="$(snapshots_declaration)" -DAPPS_CONTAINER_APPS_DECLARATION="$(apps_declaration)" -DAPPS_CONTAINER_SNAPSHOT_DECLARATIONS="$(snapshots_declaration)" -Iapps -MF /dev/null -c $< -o /dev/null > $@

$(OUTPUT)/%.ast.json: $(OUTPUT)/records_layout.ast
	@echo "JSON    $@"
	@cat $< | ruby build/struct_layout/ast_to_json.rb $*::App > $@

$(OUTPUT)/app_sizes.csv: $(OUTPUT)/records_layout.ast
	@echo "SIZE    $@"
	@cat $< | ruby build/struct_layout/ast_to_size_summary.rb $(addsuffix ::App,$(EPSILON_APPS)) > $@

apps_struct_layout: $(addprefix $(OUTPUT)/,$(addsuffix .ast.json,$(EPSILON_APPS))) $(OUTPUT)/app_sizes.csv
	@for arg in $(basename $(basename $(filter-out app_sizes.csv,$(notdir $^)))); do cp build/struct_layout/visualization.html $(OUTPUT)/$${arg}.html; sed -i '' "s/JSON_FILE/$${arg}.ast.json/g" $(OUTPUT)/$${arg}.html; done
	@echo "In a browser, go to http://localhost:8000/[APP_NAME].html to navigate in the app structure"
	@cd $(OUTPUT) && $(PYTHON) -m http.server 8000

else

apps_struct_layout:
	@echo "Struct layout requires the use of Clang: make PLATFORM=simulator TARGET=macos TOOLCHAIN=host-clang apps_struct_layout"

endif
