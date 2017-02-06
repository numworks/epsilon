#include "values_controller.h"

namespace Sequence {

ValuesController::ValuesController(Responder * parentResponder, SequenceStore * sequenceStore,
    HeaderViewController * header) :
  ViewController(parentResponder),
  HeaderViewDelegate(header),
  m_view(SolidColorView(KDColorRed)),
  m_sequenceStore(sequenceStore)
{
}
  
const char * ValuesController::title() const {
  return "Valeurs";
}

View * ValuesController::view() {
  return &m_view;
}

void ValuesController::didBecomeFirstResponder() {
  headerViewController()->setSelectedButton(-1);
}

int ValuesController::numberOfButtons() const {
  return 0;
}

Button * ValuesController::buttonAtIndex(int index) {
  return nullptr;
}

bool ValuesController::isEmpty() const {
  if (m_sequenceStore->numberOfActiveFunctions() == 0) {
    return true;
  }
  return false;
}

const char * ValuesController::emptyMessage() {
  if (m_sequenceStore->numberOfDefinedFunctions() == 0) {
    return "Aucune fonction";
  }
  return "Aucune fonction selectionnee";
}

Responder * ValuesController::defaultController() {
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

}
