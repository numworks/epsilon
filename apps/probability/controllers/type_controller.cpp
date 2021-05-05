#include "type_controller.h"

#include <apps/i18n.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/view_controller.h>
#include <ion/events.h>

TypeController::TypeController(Responder * parent, HypothesisController * hypothesisController)
    : Escher::SelectableListViewController(parent),
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
