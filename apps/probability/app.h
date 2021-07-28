#ifndef APPS_PROBABILITY_APP_H
#define APPS_PROBABILITY_APP_H

#include <apps/shared/shared_app.h>
#include <apps/shared/text_field_delegate_app.h>
#include <escher/app.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>

#include "controllers/categorical_type_controller.h"
#include "controllers/distribution_controller.h"
#include "controllers/hypothesis_controller.h"
#include "controllers/input_controller.h"
#include "controllers/input_goodness_controller.h"
#include "controllers/input_homogeneity_controller.h"
#include "controllers/menu_controller.h"
#include "controllers/parameters_controller.h"
#include "controllers/results_controller.h"
#include "controllers/results_homogeneity_controller.h"
#include "controllers/statistic_graph_controller.h"
#include "controllers/test_controller.h"
#include "controllers/type_controller.h"
#include "models/data.h"

namespace Probability {

class App : public Shared::TextFieldDelegateApp {
public:
  // Descriptor
  class Descriptor : public Escher::App::Descriptor {
  public:
    I18n::Message name() const override { return I18n::Message::DistributionApp; };
    I18n::Message upperName() const override { return I18n::Message::DistributionAppCapital; };
    const Escher::Image * icon() const override;
  };

  // Snapshot
  class Snapshot : public Shared::SharedApp::Snapshot {
  public:
    App * unpack(Escher::Container * container) override {
      return new (container->currentAppBuffer()) App(this);
    };
    const Descriptor * descriptor() const override;
    void reset() override{};
    Data::AppNavigation * navigation() { return &m_navigation; }

  private:
    friend App;
    Data::Data * data() { return &m_data; }
    Data::AppNavigation m_navigation;
    Data::Data m_data;
  };

  static App * app() { return static_cast<App *>(Escher::Container::activeApp()); }

  // Data access
  Data::Page page() { return snapshot()->navigation()->page(); }
  void setPage(Data::Page p) { snapshot()->navigation()->setPage(p); }
  Data::SubApp subapp() { return snapshot()->navigation()->subapp(); }
  Data::Test test() { return snapshot()->data()->test(); }
  Data::TestType testType() { return snapshot()->data()->testType(); }
  Data::CategoricalType categoricalType() { return snapshot()->data()->categoricalType(); }
  void setCategoricalType(Data::CategoricalType type) {
    snapshot()->data()->setCategoricalType(type);
  }

  TELEMETRY_ID("Probability");
  // TODO better handling
  bool textFieldDidFinishEditing(TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) {
    textField->setText(text);
    return true;
  }

private:
  App(Snapshot *);
  Snapshot * snapshot() const { return static_cast<Snapshot *>(Escher::App::snapshot()); }

  // Controllers
  // TODO store only memory for one controller (as a union ?)
  StatisticGraphController m_statisticGraphController;
  ResultsHomogeneityController m_homogeneityResultsController;
  InputHomogeneityController m_inputHomogeneityController;
  InputGoodnessController m_inputGoodnessController;
  ResultsController m_resultsController;
  InputController m_inputController;
  TypeController m_typeController;
  CategoricalTypeController m_categoricalTypeController;
  HypothesisController m_hypothesisController;
  CalculationController m_calculationController;
  ParametersController m_parameterController;
  DistributionController m_distributionController;
  TestController m_testController;
  MenuController m_menuController;
  Escher::StackViewController m_stackViewController;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_APP_H */
