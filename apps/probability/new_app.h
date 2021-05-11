#ifndef NEW_APP_H
#define NEW_APP_H

#include <apps/shared/text_field_delegate_app.h>
#include <apps/shared/shared_app.h>
#include <escher/container.h>
#include <escher/app.h>
#include <escher/stack_view_controller.h>
#include "distribution/binomial_distribution.h"
#include "calculation/discrete_calculation.h"
#include "controllers/menu_controller.h"
#include "controllers/distribution_controller.h"
#include "controllers/test_controller.h"
#include "controllers/parameters_controller.h"
#include "controllers/hypothesis_controller.h"
#include "controllers/categorical_type_controller.h"
#include "controllers/type_controller.h"
#include "controllers/input_controller.h"
#include "controllers/results_controller.h"
#include "controllers/input_goodness_controller.h"
#include "controllers/input_homogeneity_controller.h"

namespace Probability {

class NewApp : public Shared::TextFieldDelegateApp {
public:
class Descriptor : public Escher::App::Descriptor {
    public:
      I18n::Message name() const override { return I18n::Message::DistributionApp; };
      I18n::Message upperName() const override { return I18n::Message::DistributionAppCapital; };
      const Escher::Image * icon() const override;
  };
  class Snapshot : public Shared::SharedApp::Snapshot {
  public:
    Snapshot() {}
    ~Snapshot() {}
    App * unpack(Escher::Container * container) override { return new (container->currentAppBuffer()) NewApp(this); };
    const Descriptor * descriptor() const override {
      static Descriptor s_descriptor;
      return &s_descriptor;
    };
    void reset() override {};
  };
  static App * app() {
    return static_cast<App *>(Escher::Container::activeApp());
  }
  Escher::App::Snapshot * snapshot() const { return Escher::App::snapshot(); }
  TELEMETRY_ID("Probability");
private:
  NewApp(Escher::App::Snapshot *);

  // Controllers
  // TODO store only memory for one controller (as a union ?)
  InputHomogeneityController m_inputHomogeneityController;
  InputGoodnessController m_inputGoodnessController;
  TestResults m_resultsController;
  NormalInputController m_inputController;
  TypeController m_typeController;
  CategoricalTypeController m_categoricalTypeController;
  HypothesisController m_hypothesisController;
  CalculationController m_calculationController;
  ParametersController m_parameterController;
  DistributionController m_distributionController;
  TestController m_testController;
  MenuController m_menuController;
  Escher::StackViewController m_stackViewController;

  // To be removed
  BinomialDistribution m_distribution;
  DiscreteCalculation m_calculation;
};

}

#endif /* NEW_APP_H */
