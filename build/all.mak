# This is a standalone Makefile
# Invoke using "make -f build/all.mak all_release" or "make -f build/all.mak all_beta"

IOS_MOBILE_PROVISION ?= build/artifacts/NumWorks_Graphing_Calculator_Distribution.mobileprovision
EMCC ?= emcc

# Errors out if the file does not exist
# file_check, <file>
define file_check
$(if $(shell if test ! -f $(1) 2>&1 >/dev/null; then echo error; fi),\
	$(error "Missing file: $(1)"),)
endef

# Errors out if the command does not exist
# command_check, <command>
define command_check
$(if $(shell if ! command -v $1; then echo error; fi),\
	$(error "Missing command: $(1), did you forget to source?"),)
endef

.PHONY: all_release
all_release:
	$(call file_check,$(IOS_MOBILE_PROVISION))
	$(call command_check,$(EMCC))
	@ rm -rf output/all_official
	@ mkdir -p output/all_official
	@ echo "BUILD_FIRMWARE    DEVICE N0110"
	@ $(MAKE) TARGET=n0110 clean
	@ $(MAKE) TARGET=n0110 epsilon.onboarding.dfu
	@ cp output/release/n0110/epsilon.onboarding.dfu output/all_official/unsigned_epsilon.n0110.onboarding.dfu
	@ $(MAKE) TARGET=n0110 epsilon.onboarding.allow3rdparty.dfu
	@ cp output/release/n0110/epsilon.onboarding.allow3rdparty.dfu output/all_official/unsigned_epsilon.n0110.onboarding.allow3rdparty.dfu
	@ echo "BUILD_FIRMWARE    DEVICE N0115"
	@ $(MAKE) TARGET=n0115 clean
	@ $(MAKE) TARGET=n0115 epsilon.onboarding.dfu
	@ cp output/release/n0115/epsilon.onboarding.dfu output/all_official/unsigned_epsilon.n0115.onboarding.dfu
	@ $(MAKE) TARGET=n0115 epsilon.onboarding.allow3rdparty.dfu
	@ cp output/release/n0115/epsilon.onboarding.allow3rdparty.dfu output/all_official/unsigned_epsilon.n0115.onboarding.allow3rdparty.dfu
	@ echo "BUILD_FIRMWARE    DEVICE N0120"
	@ $(MAKE) TARGET=n0120 clean
	@ $(MAKE) TARGET=n0120 EMBED_EXTRA_DATA=1 epsilon.onboarding.dfu
	@ cp output/release/n0120/epsilon.onboarding.dfu output/all_official/unsigned_epsilon.n0120.onboarding.dfu
	@ $(MAKE) TARGET=n0120 EMBED_EXTRA_DATA=1 epsilon.onboarding.allow3rdparty.dfu
	@ cp output/release/n0120/epsilon.onboarding.allow3rdparty.dfu output/all_official/unsigned_epsilon.n0120.onboarding.allow3rdparty.dfu
	@ echo "BUILD_FIRMWARE    SIMULATOR WEB HTML"
	@ $(MAKE) TARGET=web clean
	@ $(MAKE) TARGET=web epsilon.html
	@ cp output/release/web/epsilon.html output/all_official/epsilon.html
	@ echo "BUILD_FIRMWARE    SIMULATOR WEB JS"
	@ $(MAKE) TARGET=web htmlpack.zip
	@ cp output/release/web/htmlpack.zip output/all_official/htmlpack.zip
	@ echo "BUILD_FIRMWARE    SIMULATOR ANDROID"
	@ $(MAKE) TARGET=android clean
	@ $(MAKE) TARGET=android epsilon.apk
	@ cp output/release/android/epsilon.apk output/all_official/epsilon.apk
	@ cp output/release/android/app/outputs/native-debug-symbols/release/native-debug-symbols.zip output/all_official/native-debug-symbols.zip
	@ echo "BUILD_FIRMWARE    SIMULATOR IOS"
	@ $(MAKE) TARGET=ios clean
	@ $(MAKE) TARGET=ios IOS_PROVISIONNING_PROFILE=$(IOS_MOBILE_PROVISION) epsilon.ipa
	@ cp output/release/ios/epsilon.ipa output/all_official/epsilon.ipa

.PHONY: all_beta
all_beta:
	$(call command_check,$(EMCC))
	@ rm -rf output/all_official_beta
	@ mkdir -p output/all_official_beta
	@ echo "BUILD_BETA_FIRMWARE    DEVICE N0110"
	@ $(MAKE) TARGET=n0110 clean
	@ $(MAKE) TARGET=n0110 epsilon.onboarding.beta.dfu
	@ cp output/release/n0110/epsilon.onboarding.beta.dfu output/all_official_beta/unsigned_epsilon.n0110.onboarding.beta.dfu
	@ $(MAKE) TARGET=n0110 epsilon.onboarding.beta.allow3rdparty.dfu
	@ cp output/release/n0110/epsilon.onboarding.beta.allow3rdparty.dfu output/all_official_beta/unsigned_epsilon.n0110.onboarding.beta.allow3rdparty.dfu
	@ echo "BUILD_BETA_FIRMWARE    DEVICE N0115"
	@ $(MAKE) TARGET=n0115 clean
	@ $(MAKE) TARGET=n0115 epsilon.onboarding.beta.dfu
	@ cp output/release/n0115/epsilon.onboarding.beta.dfu output/all_official_beta/unsigned_epsilon.n0115.onboarding.beta.dfu
	@ $(MAKE) TARGET=n0115 epsilon.onboarding.beta.allow3rdparty.dfu
	@ cp output/release/n0115/epsilon.onboarding.beta.allow3rdparty.dfu output/all_official_beta/unsigned_epsilon.n0115.onboarding.beta.allow3rdparty.dfu
	@ echo "BUILD_BETA_FIRMWARE    DEVICE N0120"
	@ $(MAKE) TARGET=n0120 clean
	@ $(MAKE) TARGET=n0120 EMBED_EXTRA_DATA=1 epsilon.onboarding.beta.dfu
	@ cp output/release/n0120/epsilon.onboarding.beta.dfu output/all_official_beta/unsigned_epsilon.n0120.onboarding.beta.dfu
	@ $(MAKE) TARGET=n0120 EMBED_EXTRA_DATA=1 epsilon.onboarding.beta.allow3rdparty.dfu
	@ cp output/release/n0120/epsilon.onboarding.beta.allow3rdparty.dfu output/all_official_beta/unsigned_epsilon.n0120.onboarding.beta.allow3rdparty.dfu
	@ echo "BUILD_BETA_FIRMWARE    SIMULATOR WEB HTML"
	@ $(MAKE) TARGET=web clean
	@ $(MAKE) TARGET=web epsilon.html
	@ cp output/release/web/epsilon.html output/all_official_beta/epsilon.html
	@ echo "BUILD_BETA_FIRMWARE    SIMULATOR WEB JS"
	@ $(MAKE) TARGET=web htmlpack.zip
	@ cp output/release/web/htmlpack.zip output/all_official_beta/htmlpack.zip
