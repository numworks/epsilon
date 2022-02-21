#include "app.h"

#include <apps/shared/text_field_delegate_app.h>
#include <apps/exam_mode_configuration.h>

#include "models/statistic/homogeneity_test.h"
#include "probability_icon.h"

namespace Probability {

const Escher::Image * App::Descriptor::icon() const {
  return ImageStore::ProbabilityIcon;
}

App::App(Snapshot * snapshot) :
    TextFieldDelegateApp(snapshot, &m_stackViewController),
    m_testGraphController(&m_stackViewController, static_cast<Test *>(snapshot->statistic())),
    m_intervalGraphController(&m_stackViewController, static_cast<Interval *>(snapshot->statistic())),
    m_homogeneityResultsController(
        &m_stackViewController,
        static_cast<HomogeneityTest *>(snapshot->statistic()),
        &m_resultsController),
    m_inputHomogeneityController(
        &m_stackViewController,
        &m_homogeneityResultsController,
        this,
        static_cast<HomogeneityTest *>(snapshot->statistic())),
    m_inputGoodnessController(&m_stackViewController,
                              &m_resultsController,
                              static_cast<GoodnessTest *>(snapshot->statistic()),
                              this),
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
                           this,
                           static_cast<Test *>(snapshot->statistic())),
    m_calculationController(&m_stackViewController,
                            this,
                            snapshot->distribution(),
                            snapshot->calculation()),
    m_parameterController(&m_stackViewController,
                          this,
                          snapshot->distribution(),
                          &m_calculationController),
    m_distributionController(&m_stackViewController,
                             snapshot->distribution(),
                             &m_parameterController),
    m_testController(&m_stackViewController,
                     &m_hypothesisController,
                     &m_typeController,
                     &m_categoricalTypeController,
                     &m_inputController,
                     snapshot->statistic()),
    m_menuController(&m_stackViewController,
                     &m_distributionController,
                     &m_testController,
                     snapshot->inference()),
    m_stackViewController(&m_modalViewController, &m_menuController, StackViewController::Style::GrayGradation),
    m_bufferDestructor(nullptr)
{
}

void App::didBecomeActive(Window * windows) {
  Ion::RingBuffer<Escher::ViewController *, Escher::k_MaxNumberOfStacks> * queue = snapshot()->pageQueue();
  int queueLength = queue->length();
  Escher::ViewController * currentController = &m_menuController;
  for (int i = 0; i < queueLength; i++) {
    /* The queue is refilled dynamically when "stackOpenPage"ing which prevents
     * from popping until the queue is empty. */
    Escher::ViewController * controller = queue->queuePop();
    currentController->stackOpenPage(controller);
    currentController = controller;
  }
  Escher::App::didBecomeActive(windows);
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

const App::Descriptor * App::Snapshot::descriptor() const {
  static App::Descriptor s_descriptor;
  return &s_descriptor;
}

void App::Snapshot::tidy() {
  inference()->tidy();
}

void App::Snapshot::reset() {
  m_pageQueue.reset();
}

}  // namespace Probability
