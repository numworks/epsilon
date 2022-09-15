#include "app.h"

#include <apps/apps_container.h>
#include <apps/shared/text_field_delegate_app.h>
#include <apps/exam_mode_configuration.h>

#include "distributions_icon.h"
#include "images/probability.h"

namespace Distributions {

const Escher::Image * App::Descriptor::icon() const {
  return ImageStore::DistributionsIcon;
}

App * App::Snapshot::unpack(Container * container) {
  inference()->init();
  return new (container->currentAppBuffer()) App(this, static_cast<AppsContainer *>(container)->globalContext());
}

App::App(Snapshot * snapshot, Poincare::Context * parentContext) :
    ExpressionFieldDelegateApp(snapshot, &m_inputViewController),
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
    m_menuController(
        &m_stackViewController,
        {&m_distributionController},
        {{I18n::Message::ProbaApp, I18n::Message::ProbaDescr}},
        {ImageStore::Probability,},
        this
      ),
    m_stackViewController(&m_modalViewController, &m_menuController, StackViewController::Style::GrayGradation),
    m_inputViewController(&m_modalViewController, &m_stackViewController, nullptr, nullptr, nullptr),
    m_bufferDestructor(nullptr)
{
}

void App::didBecomeActive(Window * window) {
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
  if (subAppIndex >= 0 && Inference::Initialize(snapshot()->inference(), static_cast<Inference::SubApp>(subAppIndex))) {
    m_distributionController.selectRow(0);
  }
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

}  // namespace Distributions
