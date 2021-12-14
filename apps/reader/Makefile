apps += Reader::App
app_headers += apps/reader/app.h

SFLAGS += -DHAS_READER

app_sreader_src = $(addprefix apps/reader/,\
  app.cpp \
  list_book_controller.cpp \
  utility.cpp \
  read_book_controller \
  word_wrap_view.cpp \
  tex_parser.cpp \
)

apps_src += $(app_sreader_src)

app_images += apps/reader/reader_icon.png

i18n_files += $(call i18n_without_universal_for,reader/base)

$(eval $(call depends_on_image,apps/reader/app.cpp,apps/reader/reader_icon.png))
