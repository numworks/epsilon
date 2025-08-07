PATH_ion = ../shared/ion

$(call create_zip,%htmlpack.zip, $(call generated_sources_for, \
  %js \
  $(PATH_ion)/src/simulator/web/calculator.html \
  $(PATH_ion)/src/simulator/web/calculator.css \
  $(PATH_ion)/src/simulator/web/calculator.js \
  app/assets/background.jpg \
  app/assets/background-no-shadow.webp \
) \
  $(PATH_ion)/src/simulator/assets/$(ION_layout_variant)/background-with-shadow.webp \
)

$(call create_zip,%zip, $(call generated_sources_for, \
  %js \
  $(PATH_ion)/src/simulator/web/simulator_%html \
  app/assets/background.jpg \
))

%html: $(OUTPUT_DIRECTORY)/%html
	@ :

$(OUTPUT_DIRECTORY)/%html: $(call generated_sources_for, %js $(PATH_ion)/src/simulator/web/simulator_%html app/assets/background.jpg) $(PATH_ion)/src/simulator/assets/$(ION_layout_variant)/background-with-shadow.webp $(PATH_ion)/src/simulator/web/inline.py
	$(call rule_label,INLINE)
	$(PYTHON) $(filter %.py,$^) \
      --script $(filter %.js,$^) \
      --image $(filter %.webp,$^) \
      --image $(filter %.jpg,$^) \
      $(filter %.html,$^) >$@

$(call document_other_target,<calculator>.<flavors>.zip)
$(call document_other_target,<calculator>.<flavors>.htmlpack.zip,Archive of the web simulator for integration in the website)
$(call document_other_target,<calculator>.<flavors>.html,Standalone html simulator)
