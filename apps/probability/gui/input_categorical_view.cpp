#include "input_categorical_view.h"

#include "probability/text_helpers.h"

using namespace Probability;

InputCategoricalView::InputCategoricalView(Responder * parentResponder,
                               ButtonDelegate * buttonDelegate,
                               SelectableTableView * table,
                               InputEventHandlerDelegate * inputEventHandlerDelegate,
                               TextFieldDelegate * textFieldDelegate) :
    VerticalLayout(KDColorOrange),
    Responder(parentResponder),
    m_dataInputTableView(table),
    m_significanceCell(this, inputEventHandlerDelegate, textFieldDelegate),
    m_next(this, I18n::Message::Ok, buttonDelegate->buttonActionInvocation(), KDFont::LargeFont),
    m_spacer1(Palette::WallScreenDark, 0, k_marginVertical),
    m_spacer2(Palette::WallScreenDark, 0, k_marginVertical) {
  setMargins(Metric::CommonRightMargin, 0);
  m_dataInputTableView->setMargins(Metric::CommonTopMargin, 0, 0, 0);
  m_dataInputTableView->setBackgroundColor(Palette::WallScreenDark);
  m_significanceCell.setMessage(I18n::Message::GreekAlpha);
  m_significanceCell.setSubLabelMessage(I18n::Message::SignificanceLevel);
  m_significanceCell.textField()->setText("0.05");  // TODO kinda ugly?
}

void InputCategoricalView::didBecomeFirstResponder() {
  // Pass focus to subview
  if (m_dataInputTableView->selectedRow() < 0) {
    m_dataInputTableView->selectRow(1);
  }
  if (m_viewSelection.selectedRow() < 0) {
    m_viewSelection.selectRow(0);
  }
  setResponderForSelectedRow();
  selectCorrectView();
}

bool InputCategoricalView::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up || event == Ion::Events::Down) {
    if (event == Ion::Events::Up && m_viewSelection.selectedRow() > 0) {
      m_viewSelection.selectRow(m_viewSelection.selectedRow() - 2);
    }
    if (event == Ion::Events::Down && m_viewSelection.selectedRow() < numberOfSubviews() - 1) {
      m_viewSelection.selectRow(m_viewSelection.selectedRow() + 2);
    }
    setResponderForSelectedRow();
    selectCorrectView();
    return true;
  }
  return false;
}

Responder * InputCategoricalView::responderForRow(int row) {
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

void InputCategoricalView::setResponderForSelectedRow() {
  Escher::Container::activeApp()->setFirstResponder(responderForRow(m_viewSelection.selectedRow()));
}

void InputCategoricalView::selectCorrectView() {
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

Escher::View * InputCategoricalView::subviewAtIndex(int i) {
  switch (i) {
    case k_indexOfTable:
      return m_dataInputTableView;
      break;
    case k_indexOfSpacer1:
      return &m_spacer1;
      break;
    case k_indexOfSignificance:
      return &m_significanceCell;
      break;
    case k_indexOfSpacer2:
      return &m_spacer2;
      break;
    case k_indexOfNext:
      return &m_next;
      break;
  }
  assert(false);
}
