#include "app.h"
#include "../apps_container.h"
#include "finance_icon.h"
#include <apps/i18n.h>
#include <escher/palette.h>

namespace Finance {

// App::Descriptor

const Escher::Image * App::Descriptor::icon() const {
  return ImageStore::FinanceIcon;
}

// App::Snapshot

App * App::Snapshot::unpack(Escher::Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

const App::Descriptor * App::Snapshot::descriptor() const {
  constexpr static Descriptor s_descriptor;
  return &s_descriptor;
}

void App::Snapshot::reset() {
  m_data.reset();
  m_pageQueue.reset();
}

void App::didBecomeActive(Escher::Window * window) {
  Ion::RingBuffer<Escher::ViewController *, Snapshot::k_maxNumberOfStacks> * queue = snapshot()->pageQueue();
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

void App::willOpenPage(Escher::ViewController * controller) {
  snapshot()->pageQueue()->push(controller);
}

void App::didExitPage(Escher::ViewController * controller) {
  Escher::ViewController * c = snapshot()->pageQueue()->stackPop();
  assert(c == controller);
  (void)c;
}

// App
App::App(Snapshot * snapshot) :
  Shared::TextFieldDelegateApp(snapshot, &m_stackViewController),
  m_resultController(&m_stackViewController),
  m_interestController(&m_stackViewController, this, &m_resultController),
  m_interestMenuController(&m_stackViewController, &m_interestController),
  m_menuController(&m_stackViewController, &m_interestMenuController),
  m_stackViewController(&m_modalViewController, &m_menuController, Escher::StackViewController::Style::GrayGradation)
{}

}
