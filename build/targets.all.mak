ANDROID_GRADLE_KEYSTORE ?= ~/.gradle/google-play-upload.keystore
ANDROID_GRADLE_PROPERTIES ?= ~/.gradle/gradle.properties
IOS_MOBILE_PROVISION ?= build/artifacts/NumWorks_Graphing_Calculator_Distribution.mobileprovision
EMCC ?= emcc

define source_emsdk
source ~/emsdk/emsdk_env.sh > /dev/null
endef

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

.PHONY: all_official
all_official:
	$(call file_check,$(ANDROID_GRADLE_KEYSTORE))
	$(call file_check,$(ANDROID_GRADLE_PROPERTIES))
	$(call file_check,$(IOS_MOBILE_PROVISION))
	$(call command_check,$(EMCC))
	$(Q) rm -rf output/all_official
	$(Q) mkdir -p output/all_official
	$(Q) echo "BUILD_FIRMWARE    DEVICE N0110"
	$(Q) $(MAKE) clean
	$(Q) $(MAKE) epsilon.official.onboarding.dfu
	$(Q) cp output/release/device/n0110/epsilon.official.onboarding.dfu output/all_official/epsilon.device.n0110.dfu
	$(Q) echo "BUILD_FIRMWARE    DEVICE N0100"
	$(Q) $(MAKE) MODEL=n0100 clean
	$(Q) $(MAKE) MODEL=n0100 epsilon.official.onboarding.dfu
	$(Q) cp output/release/device/n0100/epsilon.official.onboarding.dfu output/all_official/epsilon.device.n0100.dfu
	$(Q) echo "BUILD_FIRMWARE    SIMULATOR WEB ZIP"
	$(Q) $(MAKE) DEBUG=0 PLATFORM=simulator TARGET=web clean
	$(Q) $(call source_emsdk); $(MAKE) DEBUG=0 PLATFORM=simulator TARGET=web output/release/simulator/web/simulator.official.zip
	$(Q) cp output/release/simulator/web/simulator.official.zip output/all_official/simulator.web.zip
	$(Q) echo "BUILD_FIRMWARE    SIMULATOR WEB JS"
	$(Q) $(call source_emsdk); $(MAKE) DEBUG=0 PLATFORM=simulator TARGET=web epsilon.official.js
	$(Q) cp output/release/simulator/web/epsilon.official.js output/all_official/epsilon.js
	$(Q) cp output/release/simulator/web/epsilon.official.js.mem output/all_official/epsilon.js.mem
	$(Q) echo "BUILD_FIRMWARE    SIMULATOR WEB PYTHON JS"
	$(Q) $(MAKE) DEBUG=0 PLATFORM=simulator TARGET=web clean
	$(Q) $(call source_emsdk); $(MAKE) DEBUG=0 PLATFORM=simulator TARGET=web EPSILON_GETOPT=1 EPSILON_APPS=code epsilon.official.js
	$(Q) cp output/release/simulator/web/epsilon.official.js output/all_official/epsilon.python.js
	$(Q) cp output/release/simulator/web/epsilon.official.js.mem output/all_official/epsilon.python.js.mem
	$(Q) echo "BUILD_FIRMWARE    SIMULATOR ANDROID"
	$(Q) $(MAKE) PLATFORM=simulator TARGET=android clean
	$(Q) $(MAKE) PLATFORM=simulator TARGET=android epsilon.official.signed.apk
	$(Q) cp output/release/simulator/android/app/outputs/apk/codesigned/android-codesigned.apk output/all_official/epsilon.official.apk
	$(Q) echo "BUILD_FIRMWARE    SIMULATOR IOS"
	$(Q) $(MAKE) PLATFORM=simulator TARGET=ios clean
	$(Q) $(MAKE) PLATFORM=simulator TARGET=ios IOS_PROVISIONNING_PROFILE=$(IOS_MOBILE_PROVISION) output/release/simulator/ios/app/epsilon.official.ipa
	$(Q) cp output/release/simulator/ios/app/epsilon.official.ipa output/all_official/epsilon.ipa
