_simulator_app_binary = $(_simulator_app)/Contents/MacOS/$(APP_NAME)
_simulator_app_plist = $(_simulator_app)/Contents/Info.plist

include  $(PATH_haussmann)/src/rules/shared.apple.mak
