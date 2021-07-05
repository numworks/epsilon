#include "input_table_view.h"

using namespace Probability;

InputTableView::InputTableView(Responder * parentResponder, ButtonDelegate * buttonDelegate, SelectableTableView * table,
                               InputEventHandlerDelegate * inputEventHandlerDelegate,
                               TextFieldDelegate * textFieldDelegate)
    : VerticalLayout(),
      Responder(parentResponder),
      m_dataInputTableView(table),
      m_significanceCell(this, inputEventHandlerDelegate, textFieldDelegate),
      m_next(this, I18n::Message::Ok, buttonDelegate->buttonActionInvocation()) {
  setMargins(Metric::CommonRightMargin, 0);
  m_dataInputTableView->setMargins(Metric::CommonTopMargin, 0, Metric::CommonBottomMargin, 0);
  m_significanceCell.setMessage(I18n::Message::GreekAlpha);
  m_significanceCell.setSubLabelMessage(I18n::Message::SignificanceLevel);
  m_significanceCell.textField()->setText("0.05");  // TODO kinda ugly?
}

void InputTableView::didBecomeFirstResponder() {
  // Pass focus to subview
  if (m_viewSelection.selectedRow() < 0) {
    m_viewSelection.selectRow(0);
  }
  setResponderForSelectedRow();
  selectCorrectView();
}

bool InputTableView::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up || event == Ion::Events::Down) {
    if (event == Ion::Events::Up && m_viewSelection.selectedRow() > 0) {
      m_viewSelection.selectRow(m_viewSelection.selectedRow() - 1);
    }
    if (event == Ion::Events::Down && m_viewSelection.selectedRow() < numberOfSubviews() - 1) {
      m_viewSelection.selectRow(m_viewSelection.selectedRow() + 1);
    }
    setResponderForSelectedRow();
    selectCorrectView();
    return true;
  }
  return false;
}

Responder * InputTableView::responderForRow(int row) {
  switch (m_viewSelection.selectedRow()) {
    case k_indexOfTable:
      return m_dataInputTableView;
    case k_indexOfSignificance:
      return &m_significanceCell;
    case k_indexOfNext:
      return &m_next;
  }
  assert(false);
}

void InputTableView::setResponderForSelectedRow() {
  Escher::Container::activeApp()->setFirstResponder(responderForRow(m_viewSelection.selectedRow()));
}

void InputTableView::selectCorrectView() {
  // TODO set behavior in didBecomeFirstResponder?
  m_significanceCell.setHighlighted(false);
  m_next.setHighlighted(false);
  switch (m_viewSelection.selectedRow()) {
    case k_indexOfTable:
      if (m_dataInputTableView->selectedRow() < 0) {
        m_dataInputTableView->selectCellAtLocation(0, 0);  // TODO or last ?
      }
      break;
    case k_indexOfSignificance:
      m_significanceCell.setHighlighted(true);
      break;
    default:
      assert(m_viewSelection.selectedRow() == k_indexOfNext);
      m_next.setHighlighted(true);
      break;
  }
}

Escher::View * InputTableView::subviewAtIndex(int i) {
  switch (i)
  {
  case k_indexOfTable:
    return m_dataInputTableView;
    break;
  case k_indexOfSignificance:
    return &m_significanceCell;
    break;
  case k_indexOfNext:
    return &m_next;
    break;
  }
  assert(false);
}
