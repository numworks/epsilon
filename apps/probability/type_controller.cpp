#include "type_controller.h"

#include <apps/i18n.h>
#include <escher/stack_view_controller.h>
#include <ion/events.h>

TypeController::TypeController(Responder * parent, HypothesisController * hypothesisController)
    : SelectableListViewController(parent), m_hypothesisController(hypothesisController) {
  m_cells[k_indexOfTTest].setMessage(I18n::Message::TTest);
  m_cells[k_indexOfTTest].setSubtitle(I18n::Message::Recommended);
  m_cells[k_indexOfPooledTest].setMessage(I18n::Message::PooledTTest);
  m_cells[k_indexOfPooledTest].setSubtitle(I18n::Message::RarelyUsed);
  m_cells[k_indexOfZTest].setMessage(I18n::Message::ZTest);
  m_cells[k_indexOfZTest].setSubtitle(I18n::Message::RarelyUsed);
}

HighlightCell * TypeController::reusableCell(int i, int type) {
  if (i < k_numberOfChoices) {
    return &m_cells[i];
  }
  assert(i == k_numberOfChoices);
  return &m_description;
}

bool TypeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    StackViewController * stack = (StackViewController *)parentResponder();
    stack->push(m_hypothesisController);
    return true;
  }
}
