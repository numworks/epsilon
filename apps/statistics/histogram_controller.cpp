#include "histogram_controller.h"
#include <assert.h>

namespace Statistics {

HistogramController::HistogramController(Responder * parentResponder, HeaderViewController * headerViewController, Data * data) :
  ViewController(parentResponder),
  HeaderViewDelegate(headerViewController),
  m_view(HistogramView(data)),
  m_settingButton(Button(this, "Reglages de l'histogramme",Invocation([](void * context, void * sender) {}, this))),
  m_data(data)
{
}

const char * HistogramController::title() const {
  return "Histogramme";
}

View * HistogramController::view() {
  return &m_view;
}

bool HistogramController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    if (!m_view.selectedBins()) {
      headerViewController()->setSelectedButton(-1);
      m_view.selectBins(true);
      m_view.reload();
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
  if (event == Ion::Events::Left) {
    m_data->selectNextBinToward(-1);
    m_view.reload();
    return true;
  }
  if (event == Ion::Events::Right) {
    m_data->selectNextBinToward(1);
    m_view.reload();
    return true;
  }
  return false;
}

void HistogramController::didBecomeFirstResponder() {
  m_view.selectBins(true);
  m_view.reload();
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
