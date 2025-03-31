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
  quiz \
  sdl \
)

include build/coverage.mak

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

$(OUTPUT_DIRECTORY)/epsilon.apk: $(simulator_app_deps) $(apk_deps)
	$(call rule_label,GRADLE)
	$(Q) ion/src/simulator/android/gradlew -b ion/src/simulator/android/build.gradle -PappVersion=$(APP_VERSION) -PoutputDirectory=$(OUTPUT_DIRECTORY) -PndkBundleVersion=$(NDK_BUNDLE_VERSION) assembleRelease
	$(Q) cp $(OUTPUT_DIRECTORY)/app/outputs/apk/release/android-release*.apk $@

endif
