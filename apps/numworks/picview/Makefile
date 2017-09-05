ifneq (,$(findstring numworks.picview,${APPS_LIST}))

app_objs += $(addprefix apps/numworks/picview/,\
  pic_view.o\
  picview_app.o\
  picview_controller.o\
)

apps/numworks/picview/pic_view.cpp: apps/picview/image.c

apps/numworks/picview/image.c: apps/picview/image.raw
	@echo "RAW2C   $@"
	@echo "const /* Needed otherwise the image will eat up all RAM */" > $@
	@xxd -i $^ >> $@

apps/numworks/picview/image.raw: apps/picview/image.png
	@echo "PNG2RAW $@"
	@ffmpeg -loglevel panic -vcodec png -i $^ -vcodec rawvideo -f rawvideo -pix_fmt rgb565 $@

products += $(addprefix apps/numworks/picview/, image.raw image.c)

endif
