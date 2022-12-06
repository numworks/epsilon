#include "app.h"
#include "images/confidence_interval.h"
#include "images/significance_test.h"
#include "inference_icon.h"
#include "models/statistic/homogeneity_test.h"
#include <apps/apps_container.h>
#include <apps/exam_mode_configuration.h>
#include <apps/shared/text_field_delegate_app.h>

using namespace Escher;

namespace Inference {

const Escher::Image * App::Descriptor::icon() const {
  return ImageStore::InferenceIcon;
}

App * App::Snapshot::unpack(Container * container) {
  statistic()->init();
  return new (container->currentAppBuffer()) App(this, static_cast<AppsContainer *>(container)->globalContext());
}

App::App(Snapshot * snapshot, Poincare::Context * parentContext) :
    ExpressionFieldDelegateApp(snapshot, &m_inputViewController),
    m_testGraphController(&m_stackViewController, static_cast<Test *>(snapshot->statistic())),
    m_intervalGraphController(&m_stackViewController, static_cast<Interval *>(snapshot->statistic())),
    m_homogeneityResultsController(
        &m_stackViewController,
        &m_resultsController,
        static_cast<HomogeneityTest *>(snapshot->statistic())),
    m_inputHomogeneityController(
        &m_stackViewController,
        &m_homogeneityResultsController,
        static_cast<HomogeneityTest *>(snapshot->statistic()),
        this),
    m_inputGoodnessController(&m_stackViewController,
                              &m_resultsController,
                              static_cast<GoodnessTest *>(snapshot->statistic()),
                              this),
    m_inputSlopeController(&m_stackViewController,
                              &m_resultsController,
                              snapshot->statistic(),
                              this,
                              parentContext),
    m_resultsController(&m_stackViewController,
                        snapshot->statistic(),
                        &m_testGraphController,
                        &m_intervalGraphController,
                        this,
                        this),
    m_inputController(&m_stackViewController,
                      &m_resultsController,
                      snapshot->statistic(),
                      this),
    m_typeController(&m_stackViewController,
                     &m_hypothesisController,
                     &m_inputController,
                     snapshot->statistic()),
    m_categoricalTypeController(&m_stackViewController,
                                static_cast<Chi2Test *>(snapshot->statistic()),
                                &m_inputGoodnessController,
                                &m_inputHomogeneityController),
    m_hypothesisController(&m_stackViewController,
                           &m_inputController,
                           &m_inputSlopeController,
                           this,
                           static_cast<Test *>(snapshot->statistic())),
    m_testController(&m_stackViewController,
                     &m_hypothesisController,
                     &m_typeController,
                     &m_categoricalTypeController,
                     &m_inputSlopeController,
                     &m_inputController,
                     snapshot->statistic()),
    m_menuController(
        &m_stackViewController,
        {&m_testController, &m_testController},
        {{I18n::Message::Tests, I18n::Message::TestDescr}, {I18n::Message::Intervals, I18n::Message::IntervalDescr}},
        {ImageStore::SignificanceTest, ImageStore::ConfidenceInterval},
        this
      ),
    m_stackViewController(&m_modalViewController, &m_menuController, StackViewController::Style::GrayGradation),
    m_inputViewController(&m_modalViewController, &m_stackViewController, &m_inputSlopeController, &m_inputSlopeController, &m_inputSlopeController),
    m_bufferDestructor(nullptr)
{
}

void App::didBecomeActive(Window * window) {
  Ion::RingBuffer<Escher::ViewController *, Escher::k_maxNumberOfStacks> * queue = snapshot()->pageQueue();
  int queueLength = queue->length();
  Escher::ViewController * currentController = &m_menuController;
  for (int i = 0; i < queueLength; i++) {
    /* The queue is refilled dynamically when "stackOpenPage"ing which prevents
     * from popping until the queue is empty. */
    Escher::ViewController * controller = queue->queuePop();
    currentController->stackOpenPage(controller);
    currentController = controller;
  }
  Escher::App::didBecomeActive(window);
}

void App::willOpenPage(ViewController * controller) {
  snapshot()->pageQueue()->push(controller);
}

void App::didExitPage(ViewController * controller) {
  ViewController * c = snapshot()->pageQueue()->stackPop();
  assert(c == controller);
  (void)c;
}

void App::cleanBuffer(DynamicCellsDataSourceDestructor * destructor) {
  assert(destructor);
  if (m_bufferDestructor) {
    m_bufferDestructor->destroyCells();
  }
  m_bufferDestructor = destructor;
}

void App::selectSubApp(int subAppIndex) {
  if (subAppIndex >= 0 && Statistic::Initialize(snapshot()->statistic(), static_cast<Statistic::SubApp>(subAppIndex))) {
    m_testController.selectRow(0);
    m_hypothesisController.selectRow(0);
    m_typeController.selectRow(0);
  }
}

const App::Descriptor * App::Snapshot::descriptor() const {
  static App::Descriptor s_descriptor;
  return &s_descriptor;
}

void App::Snapshot::tidy() {
  statistic()->tidy();
}

void App::Snapshot::reset() {
  m_pageQueue.reset();
}

bool App::storageWillChangeForRecordName(const Ion::Storage::Record::Name recordName) {
  return !(isVarBoxMenuOpen() || isStoreMenuOpen()) || strcmp(recordName.extension, Ion::Storage::lisExtension) != 0;
}

}
