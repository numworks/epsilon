# This is a standalone Makefile
# Invoke using "make -f build/all.mak"

IOS_MOBILE_PROVISION ?= build/artifacts/NumWorks_Graphing_Calculator_Distribution.mobileprovision
EMCC ?= emcc

include build/file_checker.mak

.PHONY: all
all:
	$(call file_check,$(IOS_MOBILE_PROVISION))
	$(call command_check,$(EMCC))
	@ rm -rf output/all_official
	@ mkdir -p output/all_official
	@ echo "BUILD_FIRMWARE    DEVICE N0110"
	@ $(MAKE) MODEL=n0110 clean
	@ $(MAKE) epsilon.onboarding.dfu
	@ cp output/release/device/n0110/epsilon/epsilon.onboarding.dfu output/all_official/epsilon.device.n0110.dfu
	@ $(MAKE) epsilon.onboarding.allow3rdparty.dfu
	@ cp output/release/device/n0110/epsilon/epsilon.onboarding.allow3rdparty.dfu output/all_official/epsilon.device.n0110.allow3rdparty.dfu
	@ echo "BUILD_FIRMWARE    DEVICE N0120"
	@ $(MAKE) MODEL=n0120 clean
	@ $(MAKE) MODEL=n0120 epsilon.onboarding.dfu
	@ cp output/release/device/n0120/epsilon/epsilon.onboarding.dfu output/all_official/epsilon.device.n0120.dfu
	@ $(MAKE) MODEL=n0120 epsilon.onboarding.allow3rdparty.dfu
	@ cp output/release/device/n0120/epsilon/epsilon.onboarding.allow3rdparty.dfu output/all_official/epsilon.device.n0120.allow3rdparty.dfu
	@ echo "BUILD_FIRMWARE    SIMULATOR WEB ZIP"
	@ $(MAKE) PLATFORM=simulator TARGET=web clean
	@ $(MAKE) PLATFORM=simulator TARGET=web epsilon.zip
	@ cp output/release/simulator/web/epsilon.zip output/all_official/simulator.web.zip
	@ echo "BUILD_FIRMWARE    SIMULATOR WEB JS"
	@ $(MAKE) PLATFORM=simulator TARGET=web htmlpack.zip
	@ cp output/release/simulator/web/htmlpack.zip output/all_official/htmlpack.zip
	@ echo "BUILD_FIRMWARE    SIMULATOR WEB PYTHON JS"
	@ $(MAKE) PLATFORM=simulator TARGET=web clean
	@ $(MAKE) PLATFORM=simulator TARGET=web EPSILON_GETOPT=1 EPSILON_APPS=code epsilon.js
	@ cp output/release/simulator/web/epsilon.js output/all_official/epsilon.python.js
	@ echo "BUILD_FIRMWARE    SIMULATOR ANDROID"
	@ $(MAKE) PLATFORM=simulator TARGET=android clean
	@ $(MAKE) PLATFORM=simulator TARGET=android epsilon.apk
	@ cp output/release/simulator/android/epsilon.apk output/all_official/epsilon.apk
	@ cp output/release/simulator/android/app/outputs/native-debug-symbols/release/native-debug-symbols.zip output/all_official/native-debug-symbols.zip
	@ echo "BUILD_FIRMWARE    SIMULATOR IOS"
	@ $(MAKE) PLATFORM=simulator TARGET=ios clean
	@ $(MAKE) PLATFORM=simulator TARGET=ios IOS_PROVISIONNING_PROFILE=$(IOS_MOBILE_PROVISION) epsilon.ipa
	@ cp output/release/simulator/ios/epsilon.ipa output/all_official/epsilon.ipa
