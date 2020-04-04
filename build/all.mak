# This is a standalone Makefile
# Invoke using "make -f build/all.mak"

ANDROID_GRADLE_KEYSTORE ?= ~/.gradle/google-play-upload.keystore
ANDROID_GRADLE_PROPERTIES ?= ~/.gradle/gradle.properties
IOS_MOBILE_PROVISION ?= build/artifacts/NumWorks_Graphing_Calculator_Distribution.mobileprovision
EMCC ?= emcc

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

.PHONY: all
all:
	$(call file_check,$(ANDROID_GRADLE_KEYSTORE))
	$(call file_check,$(ANDROID_GRADLE_PROPERTIES))
	$(call file_check,$(IOS_MOBILE_PROVISION))
	$(call command_check,$(EMCC))
	@ rm -rf output/all_official
	@ mkdir -p output/all_official
	@ echo "BUILD_FIRMWARE    DEVICE N0110"
	@ $(MAKE) clean
	@ $(MAKE) epsilon.official.onboarding.dfu
	@ cp output/release/device/n0110/epsilon.official.onboarding.dfu output/all_official/epsilon.device.n0110.dfu
	@ echo "BUILD_FIRMWARE    DEVICE N0100"
	@ $(MAKE) MODEL=n0100 clean
	@ $(MAKE) MODEL=n0100 epsilon.official.onboarding.dfu
	@ cp output/release/device/n0100/epsilon.official.onboarding.dfu output/all_official/epsilon.device.n0100.dfu
	@ echo "BUILD_FIRMWARE    SIMULATOR WEB ZIP"
	@ $(MAKE) PLATFORM=simulator TARGET=web clean
	@ $(MAKE) PLATFORM=simulator TARGET=web epsilon.official.zip
	@ cp output/release/simulator/web/epsilon.official.zip output/all_official/simulator.web.zip
	@ echo "BUILD_FIRMWARE    SIMULATOR WEB JS"
	@ $(MAKE) PLATFORM=simulator TARGET=web epsilon.official.js
	@ cp output/release/simulator/web/epsilon.official.js output/all_official/epsilon.js
	@ echo "BUILD_FIRMWARE    SIMULATOR WEB PYTHON JS"
	@ $(MAKE) PLATFORM=simulator TARGET=web clean
	@ $(MAKE) PLATFORM=simulator TARGET=web EPSILON_GETOPT=1 EPSILON_APPS=code epsilon.official.js
	@ cp output/release/simulator/web/epsilon.official.js output/all_official/epsilon.python.js
	@ echo "BUILD_FIRMWARE    SIMULATOR ANDROID"
	@ $(MAKE) PLATFORM=simulator TARGET=android clean
	@ $(MAKE) PLATFORM=simulator TARGET=android epsilon.official.apk
	@ cp output/release/simulator/android/epsilon.official.apk output/all_official/epsilon.official.apk
	@ echo "BUILD_FIRMWARE    SIMULATOR IOS"
	@ $(MAKE) PLATFORM=simulator TARGET=ios clean
	@ $(MAKE) PLATFORM=simulator TARGET=ios IOS_PROVISIONNING_PROFILE=$(IOS_MOBILE_PROVISION) epsilon.official.ipa
	@ cp output/release/simulator/ios/epsilon.official.ipa output/all_official/epsilon.ipa
