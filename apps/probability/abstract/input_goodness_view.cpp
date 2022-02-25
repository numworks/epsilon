#include "input_goodness_view.h"


using namespace Probability;

Escher::View * InputGoodnessView::GoodnessContentView::InnerVerticalLayout::subviewAtIndex(int i) {
  switch (i) {
    case 0:
      return m_degreeOfFreedomCell;
    case 1:
      return m_significanceCell;
    default:
      assert(i==2);
      return m_next;
  }
}

Probability::InputGoodnessView::GoodnessContentView::GoodnessContentView(
    Escher::SelectableTableView * dataInputTableView,
    Escher::MessageTableCellWithEditableTextWithMessage * degreeOfFreedomCell,
    Escher::MessageTableCellWithEditableTextWithMessage * significanceCell,
    Escher::Button * next) :
      InputCategoricalView::ContentView(dataInputTableView),
      m_innerView(degreeOfFreedomCell, significanceCell, next) {
  degreeOfFreedomCell->setMessage(I18n::Message::DegreesOfFreedom);
  degreeOfFreedomCell->setSubLabelMessage(I18n::Message::Default);
  significanceCell->setMessage(I18n::Message::GreekAlpha);
  significanceCell->setSubLabelMessage(I18n::Message::SignificanceLevel);
}

InputGoodnessView::InputGoodnessView(Responder * parentResponder,
                                     Escher::Invocation invocation,
                                     TableViewController * tableViewController,
                                     Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
                                     Escher::TextFieldDelegate * textFieldDelegate) :
      InputCategoricalView(parentResponder, invocation, tableViewController, inputEventHandlerDelegate, textFieldDelegate, &m_contentView),
      m_degreeOfFreedomCell(this, inputEventHandlerDelegate, textFieldDelegate),
      m_contentView(tableViewController ? tableViewController->selectableTableView() : nullptr,
                    &m_degreeOfFreedomCell,
                    &m_significanceCell,
                    &m_next) {
}

Escher::Responder * InputGoodnessView::responderForRow(int row) {
  if (row == k_indexOfDegreeOfFreedom) {
    return &m_degreeOfFreedomCell;
  }
  return InputCategoricalView::responderForRow(row);
}

void InputGoodnessView::highlightCorrectView() {
  if (m_viewSelection.selectedRow() != k_indexOfTable) {
    Escher::HighlightCell * view;
    if (m_viewSelection.selectedRow() == k_indexOfDegreeOfFreedom) {
      view = &m_degreeOfFreedomCell;
      m_significanceCell.setHighlighted(false);
      if (m_next.isHighlighted()) {
        m_next.setHighlighted(false);
      }
    } else if (m_viewSelection.selectedRow() == k_indexOfSignificance) {
      view = &m_significanceCell;
      m_degreeOfFreedomCell.setHighlighted(false);
      if (m_next.isHighlighted()) {
        m_next.setHighlighted(false);
      }
    } else {
      view = &m_next;
      m_significanceCell.setHighlighted(false);
      m_degreeOfFreedomCell.setHighlighted(false);
    }
    view->setHighlighted(true);
    KDPoint offset = m_contentView.pointFromPointInView(view, view->bounds().bottomRight());
    scrollToContentPoint(offset);
  } else {
    m_significanceCell.setHighlighted(false);
    m_degreeOfFreedomCell.setHighlighted(false);
  }
}

// Update the degree of freedom cell according to the statistic
void Probability::InputGoodnessView::updateDegreeOfFreedomCell(Chi2Statistic * statistic) {
  setTextFieldText(static_cast<double>(statistic->degreeOfFreedom()), m_degreeOfFreedomCell.textField());
}
