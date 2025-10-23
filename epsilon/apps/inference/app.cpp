#include "app.h"

#include <apps/apps_container.h>
#include <apps/apps_container_helper.h>

#include "images/confidence_interval.h"
#include "images/significance_test.h"
#include "inference_icon.h"
#include "models/homogeneity_test.h"
#include "shared/global_store.h"

using namespace Escher;

namespace Inference {

const Escher::Image* App::Descriptor::icon() const {
  return ImageStore::InferenceIcon;
}

App* App::Snapshot::unpack(Container* container) {
  inference()->init();
  return new (container->currentAppBuffer()) App(this);
}

App::App(Snapshot* snapshot)
    : MathApp(snapshot, &m_inputViewController),
      m_controllerContainer{
          .m_testGraphController{
              &m_stackViewController,
              static_cast<SignificanceTest*>(snapshot->inference())},
          .m_intervalGraphController{
              &m_stackViewController,
              static_cast<ConfidenceInterval*>(snapshot->inference())},
          .m_homogeneityResultsController{
              &m_stackViewController, &m_controllerContainer,
              static_cast<HomogeneityTest*>(snapshot->inference())},
          .m_inputANOVAController{
              &m_stackViewController, &m_controllerContainer,
              static_cast<ANOVATest*>(snapshot->inference())},
          .m_inputHomogeneityController{
              &m_stackViewController, &m_controllerContainer,
              static_cast<HomogeneityTest*>(snapshot->inference())},
          .m_goodnessResultsController{
              &m_stackViewController, &m_controllerContainer,
              static_cast<GoodnessTest*>(snapshot->inference())},
          .m_inputGoodnessController{
              &m_stackViewController, &m_controllerContainer,
              static_cast<GoodnessTest*>(snapshot->inference())},
          .m_inputStoreController1{&m_stackViewController,
                                   &m_controllerContainer,
                                   snapshot->inference(), 0},
          .m_inputStoreController2{&m_stackViewController,
                                   &m_controllerContainer,
                                   snapshot->inference(), 1},
          .m_resultsController{&m_stackViewController, snapshot->inference(),
                               &m_controllerContainer},
          .m_inputController{&m_stackViewController, &m_controllerContainer,
                             snapshot->inference()},
          .m_typeController{&m_stackViewController, &m_controllerContainer,
                            snapshot->inference()},
          .m_categoricalTypeController{
              &m_stackViewController,
              static_cast<Chi2Test*>(snapshot->inference()),
              &m_controllerContainer},
          .m_hypothesisEditableController{
              &m_stackViewController, &m_controllerContainer,
              static_cast<SignificanceTest*>(snapshot->inference())},
          .m_hypothesisDisplayOnlyController{
              &m_stackViewController, &m_controllerContainer,
              static_cast<SignificanceTest*>(snapshot->inference())},
          .m_datasetController{&m_stackViewController, &m_controllerContainer,
                               snapshot->inference()},
          .m_testController{&m_stackViewController, &m_controllerContainer,
                            snapshot->inference()}},
      m_menuController(
          &m_stackViewController,
          {&m_controllerContainer.m_testController,
           &m_controllerContainer.m_testController},
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
    /* The queue is refilled dynamically when "stackOpenPage"ing
     * which prevents from popping until the queue is empty. */
    Escher::ViewController* controller = queue->queuePop();
    if (stop) {
      continue;
    }
    currentController->stackOpenPage(controller);
    currentController = controller;

    if ((currentController == &m_controllerContainer.m_inputStoreController1) ||
        (currentController == &m_controllerContainer.m_inputStoreController2)) {
      // X1/Y1 data might have changed outside the app.
      if (!snapshot()->inference()->validateInputs()) {
        // If input were invalidated, just stop here.
        stop = true;
      } else {
        // Recompute results
        snapshot()->inference()->compute();
        resultsWereRecomputed = true;
      }
    } else if (resultsWereRecomputed &&
               currentController ==
                   &m_controllerContainer.m_resultsController &&
               !snapshot()->inference()->isGraphable()) {
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
  if (subAppIndex >= 0 && snapshot()->inference()->initializeSubApp(
                              static_cast<SubApp>(subAppIndex))) {
    m_controllerContainer.m_testController.selectRow(0);
    m_controllerContainer.m_hypothesisEditableController.selectRow(0);
    m_controllerContainer.m_typeController.selectRow(0);
  }
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor* App::Snapshot::descriptor() const {
  return &sDescriptor;
}

void App::Snapshot::tidy() {
  inference()->tidy();
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
