include apps/home/Makefile
include apps/graph/Makefile
include apps/probability/Makefile
include apps/calcul/Makefile
#include apps/picview/Makefile

app_objs += $(addprefix apps/,\
  apps_container.o\
  main.o\
)

$(app_objs): $(inline_images:.png=.o)

products += $(app_objs) app.elf app.hex app.bin

app.elf: $(app_objs)
