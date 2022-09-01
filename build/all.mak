# This is a standalone Makefile
# Invoke using "make -f build/all.mak all_release" or "make -f build/all.mak all_beta"

IOS_MOBILE_PROVISION ?= build/artifacts/NumWorks_Graphing_Calculator_Distribution.mobileprovision
EMCC ?= emcc

include build/file_checker.mak

.PHONY: all_release
all_release:
	$(call file_check,$(IOS_MOBILE_PROVISION))
	$(call command_check,$(EMCC))
	@ rm -rf output/all_official
	@ mkdir -p output/all_official
	@ echo "BUILD_FIRMWARE    DEVICE N0110"
	@ $(MAKE) MODEL=n0110 clean
	@ $(MAKE) EMBED_EXTRA_DATA=0 MODEL=n0110 epsilon.onboarding.dfu
	@ cp output/release/device/n0110/epsilon/epsilon.onboarding.dfu output/all_official/unsigned_epsilon.n0110.onboarding.dfu
	@ $(MAKE) EMBED_EXTRA_DATA=0 MODEL=n0110 epsilon.onboarding.allow3rdparty.dfu
	@ cp output/release/device/n0110/epsilon/epsilon.onboarding.allow3rdparty.dfu output/all_official/unsigned_epsilon.n0110.onboarding.allow3rdparty.dfu
	@ echo "BUILD_FIRMWARE    DEVICE N0115"
	@ $(MAKE) MODEL=n0115 clean
	@ $(MAKE) EMBED_EXTRA_DATA=0 MODEL=n0115 epsilon.onboarding.dfu
	@ cp output/release/device/n0115/epsilon/epsilon.onboarding.dfu output/all_official/unsigned_epsilon.n0115.onboarding.dfu
	@ $(MAKE) EMBED_EXTRA_DATA=0 MODEL=n0115 epsilon.onboarding.allow3rdparty.dfu
	@ cp output/release/device/n0115/epsilon/epsilon.onboarding.allow3rdparty.dfu output/all_official/unsigned_epsilon.n0115.onboarding.allow3rdparty.dfu
	@ echo "BUILD_FIRMWARE    DEVICE N0120"
	@ $(MAKE) MODEL=n0120 clean
	@ $(MAKE) EMBED_EXTRA_DATA=1 MODEL=n0120 epsilon.onboarding.dfu
	@ cp output/release/device/n0120/epsilon/epsilon.onboarding.dfu output/all_official/unsigned_epsilon.n0120.onboarding.dfu
	@ $(MAKE) EMBED_EXTRA_DATA=1 MODEL=n0120 epsilon.onboarding.allow3rdparty.dfu
	@ cp output/release/device/n0120/epsilon/epsilon.onboarding.allow3rdparty.dfu output/all_official/unsigned_epsilon.n0120.onboarding.allow3rdparty.dfu
	@ echo "BUILD_FIRMWARE    SIMULATOR WEB ZIP"
	@ $(MAKE) PLATFORM=simulator TARGET=web clean
	@ $(MAKE) PLATFORM=simulator TARGET=web epsilon.zip
	@ cp output/release/simulator/web/epsilon.zip output/all_official/simulator.web.zip
	@ echo "BUILD_FIRMWARE    SIMULATOR WEB JS"
	@ $(MAKE) PLATFORM=simulator TARGET=web htmlpack.zip
	@ cp output/release/simulator/web/htmlpack.zip output/all_official/htmlpack.zip
	@ echo "BUILD_FIRMWARE    SIMULATOR ANDROID"
	@ $(MAKE) PLATFORM=simulator TARGET=android clean
	@ $(MAKE) PLATFORM=simulator TARGET=android epsilon.apk
	@ cp output/release/simulator/android/epsilon.apk output/all_official/epsilon.apk
	@ cp output/release/simulator/android/app/outputs/native-debug-symbols/release/native-debug-symbols.zip output/all_official/native-debug-symbols.zip
	@ echo "BUILD_FIRMWARE    SIMULATOR IOS"
	@ $(MAKE) PLATFORM=simulator TARGET=ios clean
	@ $(MAKE) PLATFORM=simulator TARGET=ios IOS_PROVISIONNING_PROFILE=$(IOS_MOBILE_PROVISION) epsilon.ipa
	@ cp output/release/simulator/ios/epsilon.ipa output/all_official/epsilon.ipa

.PHONY: all_beta
all_beta:
	$(call command_check,$(EMCC))
	@ rm -rf output/all_official_beta
	@ mkdir -p output/all_official_beta
	@ echo "BUILD_BETA_FIRMWARE    DEVICE N0110"
	@ $(MAKE) MODEL=n0110 clean
	@ $(MAKE) EMBED_EXTRA_DATA=0 MODEL=n0110 epsilon.onboarding.beta.dfu
	@ cp output/release/device/n0110/epsilon/epsilon.onboarding.beta.dfu output/all_official_beta/unsigned_epsilon.n0110.onboarding.beta.dfu
	@ $(MAKE) EMBED_EXTRA_DATA=0 MODEL=n0110 epsilon.onboarding.beta.allow3rdparty.dfu
	@ cp output/release/device/n0110/epsilon/epsilon.onboarding.beta.allow3rdparty.dfu output/all_official_beta/unsigned_epsilon.n0110.onboarding.beta.allow3rdparty.dfu
	@ echo "BUILD_BETA_FIRMWARE    DEVICE N0115"
	@ $(MAKE) MODEL=n0115 clean
	@ $(MAKE) EMBED_EXTRA_DATA=0 MODEL=n0115 epsilon.onboarding.beta.dfu
	@ cp output/release/device/n0115/epsilon/epsilon.onboarding.beta.dfu output/all_official_beta/unsigned_epsilon.n0115.onboarding.beta.dfu
	@ $(MAKE) EMBED_EXTRA_DATA=0 MODEL=n0115 epsilon.onboarding.beta.allow3rdparty.dfu
	@ cp output/release/device/n0115/epsilon/epsilon.onboarding.beta.allow3rdparty.dfu output/all_official_beta/unsigned_epsilon.n0115.onboarding.beta.allow3rdparty.dfu
	@ echo "BUILD_BETA_FIRMWARE    DEVICE N0120"
	@ $(MAKE) MODEL=n0120 clean
	@ $(MAKE) EMBED_EXTRA_DATA=1 MODEL=n0120 epsilon.onboarding.beta.dfu
	@ cp output/release/device/n0120/epsilon/epsilon.onboarding.beta.dfu output/all_official_beta/unsigned_epsilon.n0120.onboarding.beta.dfu
	@ $(MAKE) EMBED_EXTRA_DATA=1 MODEL=n0120 epsilon.onboarding.beta.allow3rdparty.dfu
	@ cp output/release/device/n0120/epsilon/epsilon.onboarding.beta.allow3rdparty.dfu output/all_official_beta/unsigned_epsilon.n0120.onboarding.beta.allow3rdparty.dfu
	@ echo "BUILD_BETA_FIRMWARE    SIMULATOR WEB ZIP"
	@ $(MAKE) PLATFORM=simulator TARGET=web clean
	@ $(MAKE) PLATFORM=simulator TARGET=web epsilon.zip
	@ cp output/release/simulator/web/epsilon.zip output/all_official_beta/simulator.web.zip
	@ echo "BUILD_BETA_FIRMWARE    SIMULATOR WEB JS"
	@ $(MAKE) PLATFORM=simulator TARGET=web htmlpack.zip
	@ cp output/release/simulator/web/htmlpack.zip output/all_official_beta/htmlpack.zip
