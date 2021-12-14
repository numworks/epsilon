#ifndef PROBABILITY_APP_H
#define PROBABILITY_APP_H

#include <apps/shared/shared_app.h>
#include <apps/shared/text_field_delegate_app.h>
#include <escher/app.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>

#include "abstract/dynamic_cells_data_source.h"
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
    ~Snapshot();
    const Descriptor * descriptor() const override;
    void tidy() override;
    void reset() override;
    Data::AppNavigation * navigation() { return &m_navigation; }

  private:
    friend App;
    Data::DataProxy * data() { return &m_data; }
    Data::AppNavigation m_navigation;
    Data::DataProxy m_data;
  };

  static App * app() { return static_cast<App *>(Escher::Container::activeApp()); }
  void didBecomeActive(Window * window) override;

  // Data access
  Data::Page page() { return snapshot()->navigation()->page(); }
  void setPage(Data::Page p) { snapshot()->navigation()->setPage(p); }
  Data::SubApp subapp() { return snapshot()->navigation()->subapp(); }
  void setSubapp(Data::SubApp subapp) { return snapshot()->navigation()->setSubapp(subapp); }
  Data::Test test() { return snapshot()->data()->test(); }
  Data::TestType testType() { return snapshot()->data()->testType(); }
  Data::CategoricalType categoricalType() { return snapshot()->data()->categoricalType(); }

  // Buffer API
  void * buffer(size_t offset = 0) { return m_buffer + offset; }
  void cleanBuffer(DynamicCellsDataSourceDestructor * destructor);

  static constexpr int k_dynamicCellsSizes[] = {
    sizeof(ExpressionCellWithBufferWithMessage) * k_maxNumberOfExpressionCellsWithBufferWithMessage,
    sizeof(ExpressionCellWithEditableTextWithMessage) * k_maxNumberOfExpressionCellsWithEditableTextWithMessage,
    sizeof(EvenOddBufferTextCell) * (k_homogeneityTableNumberOfReusableHeaderCells + k_homogeneityTableNumberOfReusableInnerCells),
    sizeof(EvenOddEditableTextCell) * k_homogeneityTableNumberOfReusableInnerCells + sizeof(EvenOddBufferTextCell) * k_homogeneityTableNumberOfReusableHeaderCells,
    sizeof(EvenOddEditableTextCell) * k_inputGoodnessTableNumberOfReusableCells
  };
  static constexpr int k_bufferSize = arrayMax(k_dynamicCellsSizes);

  TELEMETRY_ID("Probability");

private:
  App(Snapshot *);
  void initTableSelections(Data::Page page,
                           Data::SubApp subapp,
                           Data::Test test,
                           Data::TestType type,
                           Data::CategoricalType categoricalType);
  Snapshot * snapshot() const { return static_cast<Snapshot *>(Escher::App::snapshot()); }

  // Controllers
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
  /* Buffer used for allocating table cells to avoid duplicating required
   * space for these memory-needy tables. */
  char m_buffer[k_bufferSize];
  DynamicCellsDataSourceDestructor * m_bufferDestructor;
};

}  // namespace Probability

#endif /* PROBABILITY_APP_H */
