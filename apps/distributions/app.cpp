#include "app.h"

#include <apps/apps_container.h>
#include <apps/exam_mode_configuration.h>
#include <apps/shared/text_field_delegate_app.h>

#include "distributions_icon.h"

using namespace Escher;

namespace Distributions {

const Escher::Image *App::Descriptor::icon() const {
  return ImageStore::DistributionsIcon;
}

App *App::Snapshot::unpack(Container *container) {
  return new (container->currentAppBuffer())
      App(this, static_cast<AppsContainer *>(container)->globalContext());
}

App::App(Snapshot *snapshot, Poincare::Context *parentContext)
    : ExpressionFieldDelegateApp(snapshot, &m_stackViewController),
      m_calculationController(&m_stackViewController, this,
                              snapshot->distribution(),
                              snapshot->calculation()),
      m_parameterController(&m_stackViewController, this,
                            snapshot->distribution(), &m_calculationController),
      m_distributionController(&m_stackViewController, snapshot->distribution(),
                               &m_parameterController),
      m_stackViewController(&m_modalViewController, &m_distributionController,
                            StackViewController::Style::GrayGradation) {}

void App::didBecomeActive(Window *window) {
  Ion::RingBuffer<Escher::ViewController *, Snapshot::k_maxNumberOfPages>
      *queue = snapshot()->pageQueue();
  int queueLength = queue->length();
  Escher::ViewController *currentController = &m_distributionController;
  for (int i = 0; i < queueLength; i++) {
    /* The queue is refilled dynamically when "stackOpenPage"ing which prevents
     * from popping until the queue is empty. */
    Escher::ViewController *controller = queue->queuePop();
    currentController->stackOpenPage(controller);
    currentController = controller;
  }
  Escher::App::didBecomeActive(window);
}

void App::willOpenPage(ViewController *controller) {
  snapshot()->pageQueue()->push(controller);
}

void App::didExitPage(ViewController *controller) {
  ViewController *c = snapshot()->pageQueue()->stackPop();
  assert(c == controller);
  (void)c;
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor *App::Snapshot::descriptor() const {
  return &sDescriptor;
}

void App::Snapshot::reset() { m_pageQueue.reset(); }

}  // namespace Distributions
