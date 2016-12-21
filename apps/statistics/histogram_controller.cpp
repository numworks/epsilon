#include "histogram_controller.h"
#include <assert.h>
#include <math.h>

namespace Statistics {

HistogramController::HistogramController(Responder * parentResponder, HeaderViewController * headerViewController, Data * data) :
  ViewController(parentResponder),
  HeaderViewDelegate(headerViewController),
  m_view(HistogramView(data)),
  m_settingButton(Button(this, "Reglages de l'histogramme",Invocation([](void * context, void * sender) {
    HistogramController * histogramController = (HistogramController *) context;
    StackViewController * stack = ((StackViewController *)histogramController->stackController());
    stack->push(histogramController->histogramParameterController());
  }, this))),
  m_data(data),
  m_histogramParameterController(nullptr, data)
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
    if (!m_view.selectedBins()) {
      headerViewController()->setSelectedButton(-1);
      m_view.selectBins(true);
      m_view.reload(m_data->selectedBin());
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Up) {
    if (!m_view.selectedBins()) {
      headerViewController()->setSelectedButton(-1);
      app()->setFirstResponder(tabController());
      return true;
    }
    m_view.selectBins(false);
    headerViewController()->setSelectedButton(0);
    return true;
  }
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    int direction = event == Ion::Events::Left ? -1 : 1;
    m_view.reload(m_data->selectedBin());
    if (m_data->selectNextBinToward(direction)) {
      m_view.reload(NAN);
    } else {
      m_view.reload(m_data->selectedBin());
    }
    return true;
  }
  return false;
}

void HistogramController::didBecomeFirstResponder() {
  headerViewController()->setSelectedButton(-1);
  m_view.selectBins(true);
  m_view.reload(NAN);
}

int HistogramController::numberOfButtons() const {
  return 1;
}
Button * HistogramController::buttonAtIndex(int index) {
  return &m_settingButton;
}

bool HistogramController::isEmpty() {
  if (m_data->numberOfPairs() == 0) {
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
