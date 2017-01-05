#include "histogram_controller.h"
#include <assert.h>
#include <math.h>

namespace Statistics {

HistogramController::HistogramController(Responder * parentResponder, HeaderViewController * headerViewController, Store * store) :
  ViewController(parentResponder),
  HeaderViewDelegate(headerViewController),
  m_view(HistogramView(store)),
  m_settingButton(Button(this, "Reglages de l'histogramme",Invocation([](void * context, void * sender) {
    HistogramController * histogramController = (HistogramController *) context;
    StackViewController * stack = ((StackViewController *)histogramController->stackController());
    stack->push(histogramController->histogramParameterController());
  }, this))),
  m_store(store),
  m_histogramParameterController(nullptr, store)
{
}

const char * HistogramController::title() const {
  return "Histogramme";
}

View * HistogramController::view() {
  return &m_view;
}

StackViewController * HistogramController::stackController() {
  StackViewController * stack = (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
  return stack;
}

HistogramParameterController * HistogramController::histogramParameterController() {
  return &m_histogramParameterController;
}

bool HistogramController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    if (!m_view.isMainViewSelected()) {
      headerViewController()->setSelectedButton(-1);
      m_view.selectMainView(true);
      m_view.reloadSelection();
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Up) {
    if (!m_view.isMainViewSelected()) {
      headerViewController()->setSelectedButton(-1);
      app()->setFirstResponder(tabController());
      return true;
    }
    m_view.selectMainView(false);
    headerViewController()->setSelectedButton(0);
    return true;
  }
  if (m_view.isMainViewSelected() && (event == Ion::Events::Left || event == Ion::Events::Right)) {
    int direction = event == Ion::Events::Left ? -1 : 1;
    m_view.reloadSelection();
    if (m_store->selectNextBarToward(direction)) {
      m_view.reload();
    } else {
      m_view.reloadSelection();
    }
    return true;
  }
  return false;
}

void HistogramController::didBecomeFirstResponder() {
  uint32_t storeChecksum = m_store->checksum();
  if (m_storeVersion != storeChecksum) {
    m_storeVersion = storeChecksum;
    m_store->initBarParameters();
    m_store->initWindowParameters();
  }
  headerViewController()->setSelectedButton(-1);
  m_view.selectMainView(true);
  m_view.reload();
}

int HistogramController::numberOfButtons() const {
  return 1;
}
Button * HistogramController::buttonAtIndex(int index) {
  return &m_settingButton;
}

bool HistogramController::isEmpty() {
  if (m_store->sumOfColumn(1) == 0) {
    return true;
  }
  return false;
}

const char * HistogramController::emptyMessage() {
  return "Aucune donnee a tracer";
}

Responder * HistogramController::defaultController() {
  return tabController();
}

Responder * HistogramController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

}
