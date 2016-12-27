#include "histogram_controller.h"

namespace Statistics {

HistogramController::HistogramController(Responder * parentResponder, HeaderViewController * headerViewController, Data * data) :
  ViewController(parentResponder),
  HeaderViewDelegate(headerViewController),
  m_view(SolidColorView(KDColorGreen)),
  m_settingButton(Button(this, "Reglages de l'histogramme",Invocation([](void * context, void * sender) {}, this))),
  m_selectedBin(0),
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
    if (m_selectedBin == -1) {
      headerViewController()->setSelectedButton(-1);
      m_selectedBin = 0;
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Up) {
    if (m_selectedBin == -1) {
      headerViewController()->setSelectedButton(-1);
      app()->setFirstResponder(tabController());
      return true;
    }
    m_selectedBin = -1;
    headerViewController()->setSelectedButton(0);
    return true;
  }
  return false;
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
  return "Aucune donnee à tracer";
}

Responder * HistogramController::defaultController() {
  return tabController();
}

Responder * HistogramController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

}
