#include "input_homogeneity_view.h"
#include "probability/text_helpers.h"
#include "probability/constants.h"


using namespace Probability;

Escher::View * InputHomogeneityView::HomogeneityContentView::InnerVerticalLayout::subviewAtIndex(int i) {
  switch (i) {
    case 0:
      return m_significanceCell;
      break;
    case 1:
      return m_next;
  }
  assert(false);
  return nullptr;
}

Probability::InputHomogeneityView::HomogeneityContentView::HomogeneityContentView(
    Escher::SelectableTableView * dataInputTableView,
    Escher::MessageTableCellWithEditableTextWithMessage * significanceCell,
    Escher::Button * next) :
      InputCategoricalView::ContentView(dataInputTableView),
      m_innerView(significanceCell, next) {
  significanceCell->setMessage(I18n::Message::GreekAlpha);
  significanceCell->setSubLabelMessage(I18n::Message::SignificanceLevel);
}

InputHomogeneityView::InputHomogeneityView(Responder * parentResponder,
                                     Escher::Invocation invocation,
                                     TableViewController * tableViewController,
                                     Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
                                     Escher::TextFieldDelegate * textFieldDelegate) :
      InputCategoricalView(parentResponder, invocation, tableViewController, inputEventHandlerDelegate, textFieldDelegate, &m_contentView),
      m_contentView(tableViewController ? tableViewController->selectableTableView() : nullptr,
                    &m_significanceCell,
                    &m_next) {
}

void InputHomogeneityView::highlightCorrectView() {
  if (m_viewSelection.selectedRow() != k_indexOfTable) {
    Escher::HighlightCell * view;
    if (m_viewSelection.selectedRow() == k_indexOfSignificance) {
      view = &m_significanceCell;
      if (m_next.isHighlighted()) {
        m_next.setHighlighted(false);
      }
    } else {
      view = &m_next;
      m_significanceCell.setHighlighted(false);
    }
    view->setHighlighted(true);
    KDPoint offset = contentView()->pointFromPointInView(view, view->bounds().bottomRight());
    scrollToContentPoint(offset);
  } else {
    m_significanceCell.setHighlighted(false);
  }
}
