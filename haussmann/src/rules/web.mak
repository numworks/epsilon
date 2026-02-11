# NOTE: order matter here as the following %zip pattern is a superset of %htmlpack.zip
$(call create_zip,%htmlpack.zip, $(call generated_sources_for, \
  %js \
  app/src/calculator.html \
  app/src/calculator.css \
  app/src/calculator.js \
  app/src/calculator_shared.css \
  app/src/external_shared.css \
  app/assets/background.jpg \
  app/assets/background-no-shadow.webp \
  app/src/background-with-shadow.webp \
) \
)

$(call create_zip,%zip, $(call generated_sources_for, \
  %js \
  app/src/simulator_%html \
  app/assets/background.jpg \
))

%html: $(OUTPUT_DIRECTORY)/%html
	@ :

_python_inlining_script := ../shared/ion/src/simulator/web/inline.py

$(OUTPUT_DIRECTORY)/%html: $(call generated_sources_for, %js app/src/simulator_%html app/assets/background.jpg app/src/background-with-shadow.webp) $(_python_inlining_script)
	$(call rule_label,INLINE)
	$(PYTHON) $(filter %.py,$^) \
      --script $(filter %.js,$^) \
      --image $(filter %.webp,$^) \
      --image $(filter %.jpg,$^) \
      $(filter %.html,$^) >$@

$(call document_other_target,<calculator>.<flavors>.zip)
$(call document_other_target,<calculator>.<flavors>.htmlpack.zip,Archive of the web simulator for integration in the website)
$(call document_other_target,<calculator>.<flavors>.html,Standalone html simulator)
