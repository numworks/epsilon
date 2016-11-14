include apps/home/Makefile
include apps/graph/Makefile
include apps/probability/Makefile
include apps/calculation/Makefile
#include apps/picview/Makefile

app_objs += $(addprefix apps/,\
  apps_container.o\
  constant.o\
  expression_text_field_delegate.o\
  main.o\
  node.o\
  node_list_view_controller.o\
  node_navigation_controller.o\
  toolbox_controller.o\
  toolbox_leaf_cell.o\
  toolbox_node.o\
  variable_box_controller.o\
  variable_box_node.o\
)

$(app_objs): $(inline_images:.png=.o)

products += $(app_objs) app.elf app.hex app.bin

app.$(EXE): $(app_objs)
