define file_check
@ if test ! -f $(1); \
  then \
  echo "Missing file: $(1)"; \
  exit 1; \
fi
endef

define command_check
@ if ! command -v $(1) > /dev/null; \
  then \
  echo "Missing command: $(1), did you forget to source?"; \
  exit 1; \
fi
endef

# This function checks if given folder exists
define folder_check
if test ! -d $(1); then \
  echo "Missing folder: $(1)."; \
fi
endef

# This function returns info about the nearest folder from given missing folder
define existing_parent_folder
existing_folder=$(1); \
while test ! -d $$existing_folder; do \
  previous_folder=$${existing_folder%/*}; \
  if [ $$previous_folder == $$existing_folder ]; then \
    echo "Missing folder: $(1)."; \
    exit 1; \
  fi; \
  existing_folder=$$previous_folder; \
done; \
echo "Missing folder: $(1)."; \
echo "Nearest folder is $$existing_folder, containing :"; \
ls $$existing_folder;
endef

