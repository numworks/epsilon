ifeq ($(HOST),macos)
ifeq ($(CXX),g++)
$(error Epsilon cannot be built with the gcc compiler on macOS)
endif
endif

$(call import_module,sdl,ion/src/simulator/external)

$(call create_goal,epsilon, \
  apps \
  eadk \
  escher \
  ion \
  kandinsky \
  liba_bridge \
  omg \
  poincare \
  python \
  sdl \
)

$(call create_goal,test, \
  apps.test \
  eadk \
  escher.test \
  ion.test \
  kandinsky.test \
  liba_bridge \
  omg.test \
  poincare.test \
  python.test \
  quiz.epsilon \
  sdl \
)

include $(PATH_haussmann)/src/rules/coverage.mak

ifeq ($(PLATFORM),web)
$(call create_zip,epsilon%zip,$(addprefix $(OUTPUT_DIRECTORY)/, \
  epsilon%js \
  ion/src/simulator/web/simulator.html \
  app/assets/background.jpg \
))

$(call document_other_target,epsilon.<flavors>.zip)

$(call create_zip,htmlpack%zip,$(addprefix $(OUTPUT_DIRECTORY)/, \
  epsilon%js \
  ion/src/simulator/web/calculator.html \
  ion/src/simulator/web/calculator.css \
  app/assets/background.jpg \
  app/assets/background-no-shadow.webp \
) \
  ion/src/simulator/assets/$(ION_layout_variant)/background-with-shadow.webp \
)

$(call document_other_target,htmlpack.<flavors>.zip)

epsilon%html: $(OUTPUT_DIRECTORY)/epsilon%html
	@ :

$(OUTPUT_DIRECTORY)/epsilon%html: $(addprefix $(OUTPUT_DIRECTORY)/,epsilon%js ion/src/simulator/web/simulator.html app/assets/background.jpg) ion/src/simulator/assets/$(ION_layout_variant)/background-with-shadow.webp ion/src/simulator/web/inline.py
	$(call rule_label,INLINE)
	$(PYTHON) $(filter %.py,$^) \
		--script $(filter %.js,$^) \
		--image $(filter %.webp,$^) \
		--image $(filter %.jpg,$^) \
		$(filter %.html,$^) >$@

$(call document_other_target,epsilon.<flavors>.html)
endif

ifeq ($(PLATFORM),android)

epsilon.apk: $(OUTPUT_DIRECTORY)/epsilon.apk
	@ :

# Simulator resources

simulator_app_deps = $(addprefix ion/src/simulator/assets/,epsilon/background-with-shadow.webp horizontal_arrow.png vertical_arrow.png round.png small_squircle.png large_squircle.png)
$(addprefix $(OUTPUT_DIRECTORY)/,$(addprefix ion/src/simulator/assets/,background.jpg background-no-shadow.webp)): ion/src/simulator/assets/epsilon/background-with-shadow.webp
	$(call rule_label,CONVERT)
	$(Q) mkdir -p $(dir $@)
	$(Q) convert -crop 1005x1975+93+13 -resize 1160x2220 $< $@
simulator_app_deps += $(addprefix $(OUTPUT_DIRECTORY)/,$(addprefix ion/src/simulator/assets/,background.jpg background-no-shadow.webp))

# Android resources
# Some android resources needs to be filtered through ImageMagick. Others are
# simply copied over.

$(OUTPUT_DIRECTORY)/app/res/mipmap/ic_launcher.png: ion/src/simulator/assets/logo.svg | $$(@D)/.
	$(call rule_label,CONVERT)
	$(Q) convert -background "#FFB734" MSVG:$< $@

$(OUTPUT_DIRECTORY)/app/res/mipmap-v26/ic_launcher_foreground.png: ion/src/simulator/assets/logo.svg | $$(@D)/.
	$(call rule_label,CONVERT)
	$(Q) convert -background none MSVG:$< -resize 512x512 -gravity center -background none -extent 1024x1024 $@

$(OUTPUT_DIRECTORY)/app/res/%.xml: ion/src/simulator/android/src/res/%.xml | $$(@D)/.
	$(call rule_label,COPY)
	$(Q) cp $< $@

# We need to put the .so files somewhere Gradle can pick them up.
# We decided to use the location "app/libs/$EXECUTABLE/$ARCH/libepsilon.so"
# This way it's easy to import the shared object from Java code (it's always
# named libepsilon.so), and it's easy to make Gradle use a given executable by
# simply using the jniLibs.src directive.
define path_for_arch_jni_lib
$$(OUTPUT_DIRECTORY)/app/libs/epsilon/$(1)/libepsilon.so
endef

define rule_for_arch_jni_lib
$(call path_for_arch_jni_lib,$(1)): $$(OUTPUT_DIRECTORY)/$(1)/epsilon.so | $$$$(@D)/.
	$(Q) cp $$< $$@
endef

$(foreach ARCH,$(ARCHS),$(eval $(call rule_for_arch_jni_lib,$(ARCH))))

apk_deps = $(foreach ARCH,$(ARCHS),$(call path_for_arch_jni_lib,$(ARCH)))
apk_deps += $(subst ion/src/simulator/android/src/res,$(OUTPUT_DIRECTORY)/app/res,$(wildcard ion/src/simulator/android/src/res/*/*))
apk_deps += $(addprefix $(OUTPUT_DIRECTORY)/app/res/,mipmap/ic_launcher.png mipmap-v26/ic_launcher_foreground.png)

$(OUTPUT_DIRECTORY)/epsilon.apk: $(simulator_app_deps) $(apk_deps)
	$(call rule_label,GRADLE)
	$(Q) ion/src/simulator/android/gradlew -b ion/src/simulator/android/build.gradle -PappVersion=$(APP_VERSION) -PoutputDirectory=$(OUTPUT_DIRECTORY) -PndkBundleVersion=$(NDK_BUNDLE_VERSION) assembleRelease
	$(Q) cp $(OUTPUT_DIRECTORY)/app/outputs/apk/release/android-release*.apk $@

endif
