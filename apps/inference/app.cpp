#include "app.h"

#include <apps/apps_container.h>
#include <apps/apps_container_helper.h>

#include "images/confidence_interval.h"
#include "images/significance_test.h"
#include "inference_icon.h"
#include "models/statistic/homogeneity_test.h"

using namespace Escher;

namespace Inference {

const Escher::Image* App::Descriptor::icon() const {
  return ImageStore::InferenceIcon;
}

App* App::Snapshot::unpack(Container* container) {
  statistic()->init();
  return new (container->currentAppBuffer())
      App(this, static_cast<AppsContainer*>(container)->globalContext());
}

App::App(Snapshot* snapshot, Poincare::Context* parentContext)
    : MathApp(snapshot, &m_inputViewController),
      m_testGraphController(&m_stackViewController,
                            static_cast<Test*>(snapshot->statistic())),
      m_intervalGraphController(&m_stackViewController,
                                static_cast<Interval*>(snapshot->statistic())),
      m_homogeneityResultsController(
          &m_stackViewController, &m_resultsController,
          static_cast<HomogeneityTest*>(snapshot->statistic())),
      m_inputHomogeneityController(
          &m_stackViewController, &m_homogeneityResultsController,
          static_cast<HomogeneityTest*>(snapshot->statistic())),
      m_goodnessResultsController(
          &m_stackViewController, &m_testGraphController,
          &m_intervalGraphController,
          static_cast<GoodnessTest*>(snapshot->statistic())),
      m_inputGoodnessController(
          &m_stackViewController, &m_goodnessResultsController,
          static_cast<GoodnessTest*>(snapshot->statistic())),
      m_inputStoreController1(&m_stackViewController, &m_resultsController,
                              InputStoreController::PageIndex::One,
                              &m_inputStoreController2, snapshot->statistic(),
                              parentContext),
      m_inputStoreController2(&m_stackViewController, &m_resultsController,
                              InputStoreController::PageIndex::Two, nullptr,
                              snapshot->statistic(), parentContext),
      m_resultsController(&m_stackViewController, snapshot->statistic(),
                          &m_testGraphController, &m_intervalGraphController),
      m_inputController(&m_stackViewController, &m_resultsController,
                        snapshot->statistic()),
      m_typeController(&m_stackViewController, &m_hypothesisController,
                       &m_inputController, &m_datasetController,
                       snapshot->statistic()),
      m_categoricalTypeController(
          &m_stackViewController, static_cast<Chi2Test*>(snapshot->statistic()),
          &m_inputGoodnessController, &m_inputHomogeneityController),
      m_hypothesisController(&m_stackViewController, &m_inputController,
                             &m_inputStoreController1, &m_datasetController,
                             static_cast<Test*>(snapshot->statistic())),
      m_datasetController(&m_stackViewController, &m_inputController,
                          &m_inputStoreController1, snapshot->statistic()),
      m_testController(&m_stackViewController, &m_hypothesisController,
                       &m_typeController, &m_categoricalTypeController,
                       &m_inputStoreController1, &m_inputController,
                       snapshot->statistic()),
      m_menuController(
          &m_stackViewController, {&m_testController, &m_testController},
          {{I18n::Message::Tests, I18n::Message::TestDescr},
           {I18n::Message::Intervals, I18n::Message::IntervalDescr}},
          {ImageStore::SignificanceTest, ImageStore::ConfidenceInterval}, this),
      m_stackViewController(&m_modalViewController, &m_menuController,
                            StackViewController::Style::GrayGradation),
      m_inputViewController(&m_modalViewController, &m_stackViewController,
                            Shared::MathLayoutFieldDelegate::Default()),
      m_bufferDestructor(nullptr) {}

void App::didBecomeActive(Window* window) {
  OMG::RingBuffer<Escher::ViewController*,
                  LargeStackViewController::k_maxNumberOfChildren>* queue =
      snapshot()->pageQueue();
  int queueLength = queue->length();
  Escher::ViewController* currentController = &m_menuController;
  bool stop = false;
  bool resultsWereRecomputed = false;
  for (int i = 0; i < queueLength; i++) {
    /* The queue is refilled dynamically when "stackOpenPage"ing which prevents
     * from popping until the queue is empty. */
    Escher::ViewController* controller = queue->queuePop();
    if (stop) {
      continue;
    }
    currentController->stackOpenPage(controller);
    currentController = controller;

    if ((currentController == &m_inputStoreController1) ||
        (currentController == &m_inputStoreController2)) {
      // X1/Y1 data might have changed outside the app.
      if (!snapshot()->statistic()->validateInputs()) {
        // If input were invalidated, just stop here.
        stop = true;
      } else {
        // Recompute results
        snapshot()->statistic()->compute();
        resultsWereRecomputed = true;
      }
    } else if (resultsWereRecomputed &&
               currentController == &m_resultsController &&
               !snapshot()->statistic()->isGraphable()) {
      // The results changed and can't be graphed anymore
      stop = true;
    }
  }
  Escher::App::didBecomeActive(window);
}

void App::willOpenPage(ViewController* controller) {
  snapshot()->pageQueue()->push(controller);
}

void App::didExitPage(ViewController* controller) {
  ViewController* c = snapshot()->pageQueue()->stackPop();
  assert(c == controller);
  (void)c;
}

void App::cleanBuffer(DynamicCellsDataSourceDestructor* destructor) {
  assert(destructor);
  if (m_bufferDestructor) {
    m_bufferDestructor->dynamicCellsTableView()->selectColumn(0);
    m_bufferDestructor->dynamicCellsTableView()->selectRow(-1);
    m_bufferDestructor->dynamicCellsTableView()
        ->resetSizeAndOffsetMemoization();
    m_bufferDestructor->destroyCells();
  }
  m_bufferDestructor = destructor;
}

void App::selectSubApp(int subAppIndex) {
  if (subAppIndex >= 0 &&
      Statistic::Initialize(snapshot()->statistic(),
                            static_cast<Statistic::SubApp>(subAppIndex))) {
    m_testController.selectRow(0);
    m_hypothesisController.selectRow(0);
    m_typeController.selectRow(0);
  }
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor* App::Snapshot::descriptor() const {
  return &sDescriptor;
}

void App::Snapshot::tidy() {
  statistic()->tidy();
  Shared::SharedApp::Snapshot::tidy();
}

void App::Snapshot::reset() {
  Shared::SharedApp::Snapshot::reset();
  m_pageQueue.reset();
}

bool App::storageCanChangeForRecordName(
    const Ion::Storage::Record::Name recordName) const {
  return !m_intrusiveStorageChangeFlag ||
         strcmp(recordName.extension, Ion::Storage::listExtension) != 0;
}

}  // namespace Inference
