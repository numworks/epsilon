app_objs += $(addprefix apps/picview/,\
  pic_view.o\
  picview_app.o\
  picview_controller.o\
)

apps/picview/pic_view.cpp: apps/picview/image.c

apps/picview/image.c: apps/picview/image.raw
	@echo "RAW2C   $@"
	@echo "const /* Needed otherwise the image will eat up all RAM */" > $@
	@xxd -i $^ >> $@

apps/picview/image.raw: apps/picview/image.png
	@echo "PNG2RAW $@"
	@ffmpeg -loglevel panic -vcodec png -i $^ -vcodec rawvideo -f rawvideo -pix_fmt rgb565 $@

products += $(addprefix apps/picview/, image.raw image.c)
