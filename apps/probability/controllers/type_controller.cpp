#include "type_controller.h"

#include <apps/i18n.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/view_controller.h>
#include <ion/events.h>

using namespace Probability;

TypeController::TypeController(StackViewController * parent, HypothesisController * hypothesisController)
    : SelectableListViewPage(parent),
      m_hypothesisController(hypothesisController),
      m_contentView(&m_selectableTableView, &m_description) {
  m_cells[k_indexOfTTest].setMessage(I18n::Message::TTest);
  m_cells[k_indexOfTTest].setSubtitle(I18n::Message::Recommended);
  m_cells[k_indexOfPooledTest].setMessage(I18n::Message::PooledTTest);
  m_cells[k_indexOfPooledTest].setSubtitle(I18n::Message::RarelyUsed);
  m_cells[k_indexOfZTest].setMessage(I18n::Message::ZTest);
  m_cells[k_indexOfZTest].setSubtitle(I18n::Message::RarelyUsed);

  m_description.setMessage(I18n::Message::TypeDescr);
}

HighlightCell * TypeController::reusableCell(int i, int type) { return &m_cells[i]; }

bool TypeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
  Escher::ViewController * view;
  switch (selectedRow()) {
    case k_indexOfTTest:
    case k_indexOfPooledTest:
    case k_indexOfZTest:
      view = m_hypothesisController;
      break;
  }
  assert(view != nullptr);
  openPage(view, false);
  return true;
}
return false;
}

