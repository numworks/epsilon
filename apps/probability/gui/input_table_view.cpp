#include "input_table_view.h"

using namespace Probability;

InputTableView::InputTableView(Responder * parentResponder, ButtonDelegate * buttonDelegate, SelectableTableView * table,
                               InputEventHandlerDelegate * inputEventHandlerDelegate,
                               TextFieldDelegate * textFieldDelegate)
    : VerticalLayout(),
      Responder(parentResponder),
      m_tableView(table),
      m_significance(this, inputEventHandlerDelegate, textFieldDelegate),
      m_next(this, I18n::Message::Ok, buttonDelegate->buttonActionInvocation()) {
  // TODO
  // m_significance.setMessage(I18n::Message::A);
  // m_significance.setSubLabelMessage(I18n::Message::SignificanceLevel);
  // m_significance.setAccessoryText("0.05");
  setView(m_tableView, 0);
  setView(&m_significance, 1);
  setView(&m_next, 2);
}

void InputTableView::didBecomeFirstResponder() {
  // Pass focus to subview
  if (m_viewSelection.selectedRow() < 0) {
    m_viewSelection.selectRow(0);
  }
  setResponderForSelectedRow();
  highlightViewForSelectedRow();
}

bool InputTableView::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up || event == Ion::Events::Down) {
    if (event == Ion::Events::Up && m_viewSelection.selectedRow() > 0) {
      m_viewSelection.selectRow(m_viewSelection.selectedRow() - 1);
    }
    if (event == Ion::Events::Down && m_viewSelection.selectedRow() < k_numberOfChildren - 1) {
      m_viewSelection.selectRow(m_viewSelection.selectedRow() + 1);
    }
    setResponderForSelectedRow();
    highlightViewForSelectedRow();
    return true;
  }
  return false;
}

Responder * InputTableView::responderForRow(int row) {
  switch (m_viewSelection.selectedRow()) {
    case k_indexOfTable:
      return m_tableView;
    case k_indexOfSignificance:
      return &m_significance;
    case k_indexOfNext:
      return &m_next;
  }
  assert(false);
}

void InputTableView::setResponderForSelectedRow() {
  Escher::Container::activeApp()->setFirstResponder(responderForRow(m_viewSelection.selectedRow()));
}

void InputTableView::highlightViewForSelectedRow() {
  // TODO set behavior in didBecomeFirstResponder?
  m_significance.setHighlighted(false);
  m_next.setHighlighted(false);
  switch (m_viewSelection.selectedRow()) {
    case k_indexOfTable:
      if (m_tableView->selectedRow() < 0) {
        m_tableView->selectCellAtLocation(0, 0);  // TODO or last ?
      }
      break;
    case k_indexOfSignificance:
      m_significance.setHighlighted(true);
      break;
    case k_indexOfNext:
      m_next.setHighlighted(true);
      break;
  }
}
