#ifndef INFERENCE_APP_H
#define INFERENCE_APP_H

#include <apps/shared/math_app.h>
#include <apps/shared/menu_controller.h>
#include <escher/app.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>
#include <omg/ring_buffer.h>

#include "models/statistic_buffer.h"
#include "shared/dynamic_cells_data_source.h"
#include "statistic/chi_square/categorical_type_controller.h"
#include "statistic/chi_square/input_goodness_controller.h"
#include "statistic/chi_square/input_homogeneity_controller.h"
#include "statistic/chi_square/results_goodness_controller.h"
#include "statistic/chi_square/results_homogeneity_controller.h"
#include "statistic/dataset_controller.h"
#include "statistic/input_controller.h"
#include "statistic/interval/interval_graph_controller.h"
#include "statistic/results_controller.h"
#include "statistic/store/input_store_controller.h"
#include "statistic/test/hypothesis_controller.h"
#include "statistic/test/test_graph_controller.h"
#include "statistic/test_controller.h"
#include "statistic/type_controller.h"

namespace Inference {

class App : public Shared::MathApp, public Shared::MenuControllerDelegate {
  using LargeStackViewController = Escher::StackViewController::Custom<9>;

 public:
  // Descriptor
  class Descriptor : public Escher::App::Descriptor {
   public:
    I18n::Message name() const override { return I18n::Message::InferenceApp; };
    I18n::Message upperName() const override {
      return I18n::Message::InferenceAppCapital;
    };
    const Escher::Image* icon() const override;
  };

  // Snapshot
  class Snapshot : public Shared::SharedApp::Snapshot {
   public:
    App* unpack(Escher::Container* container) override;
    const Descriptor* descriptor() const override;
    void tidy() override;
    void reset() override;

    Statistic* statistic() { return m_statisticBuffer.statistic(); }

    OMG::RingBuffer<Escher::ViewController*,
                    LargeStackViewController::k_maxNumberOfChildren>*
    pageQueue() {
      return &m_pageQueue;
    }

   private:
    friend App;
    // TODO: optimize size of Stack
    OMG::RingBuffer<Escher::ViewController*,
                    LargeStackViewController::k_maxNumberOfChildren>
        m_pageQueue;
    StatisticBuffer m_statisticBuffer;
  };

  static App* app() { return static_cast<App*>(Escher::App::app()); }
  void didBecomeActive(Escher::Window* window) override;
  bool storageCanChangeForRecordName(
      const Ion::Storage::Record::Name recordName) const override;

  // Navigation
  void willOpenPage(Escher::ViewController* controller) override;
  void didExitPage(Escher::ViewController* controller) override;

  // Cells buffer API
  void* buffer(size_t offset = 0) { return m_buffer + offset; }
  void cleanBuffer(DynamicCellsDataSourceDestructor* destructor);

  constexpr static int k_bufferSize =  // 21056
      std::max({sizeof(ResultCell) * k_maxNumberOfResultCells,
                sizeof(ParameterCell) * k_maxNumberOfParameterCell,
                sizeof(InferenceEvenOddBufferCell) *
                    (k_homogeneityTableNumberOfReusableHeaderCells +
                     k_homogeneityTableNumberOfReusableInnerCells),
                sizeof(InferenceEvenOddEditableCell) *
                        k_homogeneityTableNumberOfReusableInnerCells +
                    sizeof(InferenceEvenOddBufferCell) *
                        k_homogeneityTableNumberOfReusableHeaderCells,
                sizeof(InferenceEvenOddEditableCell) *
                    k_doubleColumnTableNumberOfReusableCells});

  // Shared::MenuControllerDelegate
  void selectSubApp(int subAppIndex) override;
  int selectedSubApp() const override {
    return static_cast<int>(snapshot()->statistic()->subApp());
  }
  int numberOfSubApps() const override {
    return static_cast<int>(Statistic::k_numberOfSubApps);
  }

  Escher::InputViewController* inputViewController() {
    return &m_inputViewController;
  }

 private:
  App(Snapshot* snapshot, Poincare::Context* parentContext);
  Snapshot* snapshot() const {
    return static_cast<Snapshot*>(Escher::App::snapshot());
  }

  // Controllers
  TestGraphController m_testGraphController;
  IntervalGraphController m_intervalGraphController;
  ResultsHomogeneityTabController m_homogeneityResultsController;
  InputHomogeneityController m_inputHomogeneityController;
  ResultsGoodnessTabController m_goodnessResultsController;
  InputGoodnessController m_inputGoodnessController;
  InputStoreController m_inputStoreController1;
  InputStoreController m_inputStoreController2;
  ResultsController m_resultsController;
  InputController m_inputController;
  TypeController m_typeController;
  CategoricalTypeController m_categoricalTypeController;
  HypothesisController m_hypothesisController;
  DatasetController m_datasetController;
  TestController m_testController;
  Shared::MenuController m_menuController;
  LargeStackViewController m_stackViewController;
  Escher::InputViewController m_inputViewController;
  /* Buffer used for allocating table cells to avoid duplicating required
   * space for these memory-needy tables. */
  char m_buffer[k_bufferSize];
  DynamicCellsDataSourceDestructor* m_bufferDestructor;
};

}  // namespace Inference

#endif
