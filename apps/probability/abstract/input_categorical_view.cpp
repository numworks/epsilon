#include "input_categorical_view.h"

#include <escher/button_delegate.h>
#include "probability/abstract/table_view_controller.h"
#include <escher/horizontal_or_vertical_layout.h>

using namespace Probability;

Escher::View * InputCategoricalView::ContentView::InnerVerticalLayout::subviewAtIndex(int i) {
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

Probability::InputCategoricalView::ContentView::ContentView(
    Escher::SelectableTableView * dataInputTableView,
    Escher::MessageTableCellWithEditableTextWithMessage * significanceCell,
    Escher::Button * next) :
      m_dataInputTableView(dataInputTableView), m_innerView(significanceCell, next) {
  significanceCell->setMessage(I18n::Message::GreekAlpha);
  significanceCell->setSubLabelMessage(I18n::Message::SignificanceLevel);
}

Escher::View * InputCategoricalView::ContentView::subviewAtIndex(int i) {
  switch (i) {
    case k_indexOfTable:
      return m_dataInputTableView;
      break;
    case k_indexOfInnerLayout:
      return &m_innerView;
      break;
  }
  assert(false);
  return nullptr;
}

InputCategoricalView::InputCategoricalView(Responder * parentResponder,
                                           Escher::ButtonDelegate * buttonDelegate,
                                           TableViewController * tableViewController,
                                           Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
                                           Escher::TextFieldDelegate * textFieldDelegate) :
      Escher::ScrollView(&m_contentView, &m_scrollDataSource),
      Responder(parentResponder),
      m_tableViewController(tableViewController),
      m_significanceCell(this, inputEventHandlerDelegate, textFieldDelegate),
      m_next(this,
             I18n::Message::Next,
             buttonDelegate->buttonActionInvocation(),
             Escher::Palette::WallScreenDark),
      m_contentView(tableViewController ? tableViewController->selectableTableView() : nullptr,
                    &m_significanceCell,
                    &m_next) {
  setTopMargin(Escher::Metric::CommonTopMargin);
  setBottomMargin(Escher::Metric::CommonBottomMargin);
  setBackgroundColor(Escher::Palette::WallScreenDark);
}

void InputCategoricalView::didBecomeFirstResponder() {
  // Pass focus to subview
  if (m_viewSelection.selectedRow() < 0) {
    m_viewSelection.selectRow(0);
  }
  setResponderForSelectedRow();
  highlightCorrectView();
}

bool InputCategoricalView::handleEvent(Ion::Events::Event event) {
  // Move selection between views
  if (event == Ion::Events::Up || event == Ion::Events::Down) {
    if (event == Ion::Events::Up && m_viewSelection.selectedRow() > 0) {
      int jump = 1 + (m_viewSelection.selectedRow() == k_indexOfSpacer + 1);
      m_viewSelection.selectRow(m_viewSelection.selectedRow() - jump);
      highlightCorrectView();
      setResponderForSelectedRow();
    }
    if (event == Ion::Events::Down && m_viewSelection.selectedRow() < k_indexOfNext) {
      int jump = 1 + (m_viewSelection.selectedRow() == k_indexOfSpacer - 1);
      m_viewSelection.selectRow(m_viewSelection.selectedRow() + jump);
      highlightCorrectView();
      setResponderForSelectedRow();
    }
    return true;
  }
  return false;
}

Escher::Responder * InputCategoricalView::responderForRow(int row) {
  switch (m_viewSelection.selectedRow()) {
    case k_indexOfTable:
      return m_tableViewController;
    case k_indexOfSignificance:
      return &m_significanceCell;
    case k_indexOfNext:
      return &m_next;
  }
  assert(false);
  return nullptr;
}

void InputCategoricalView::setResponderForSelectedRow() {
  Escher::Container::activeApp()->setFirstResponder(responderForRow(m_viewSelection.selectedRow()));
}

void InputCategoricalView::highlightCorrectView() {
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
    KDPoint offset = m_contentView.pointFromPointInView(view, view->bounds().bottomRight());
    scrollToContentPoint(offset);
  } else {
    m_significanceCell.setHighlighted(false);
  }
}

KDSize Probability::InputCategoricalView::minimalSizeForOptimalDisplay() const {
  // Pass expected size to VerticalLayout to propagate to TableCells
  ContentView * contentView = const_cast<ContentView *>(&m_contentView);
  if (contentView->bounds().width() <= 0) {
    contentView->setSize(KDSize(bounds().width(), contentView->bounds().height()));
  }
  KDSize requiredSize = ScrollView::minimalSizeForOptimalDisplay();
  return KDSize(bounds().width() + leftMargin() + rightMargin(), requiredSize.height());
}

void Probability::InputCategoricalView::selectViewAtIndex(int index) {
  m_viewSelection.selectRow(index);
  highlightCorrectView();
  setResponderForSelectedRow();
}

void Probability::InputCategoricalView::setSignificanceCellText(const char * text) {
  m_significanceCell.textField()->setText(text);
}

void Probability::InputCategoricalView::setTableView(TableViewController * tableViewController) {
  m_tableViewController = tableViewController;
  Escher::SelectableTableView * tableView = tableViewController->selectableTableView();
  m_contentView.setTableView(tableView);
  tableView->setMargins(0, Escher::Metric::CommonRightMargin, k_marginVertical, Escher::Metric::CommonLeftMargin);
  tableView->setBackgroundColor(Escher::Palette::WallScreenDark);
  tableView->setDecoratorType(Escher::ScrollView::Decorator::Type::None);
}

void Probability::InputCategoricalView::tableViewDataSourceDidChangeSize() {
  /* Relayout when inner table changes size. We need to reload the table because
   * its width might change but it won't relayout as its frame isn't changed by
   * the InputCategoricalController */
  m_tableViewController->selectableTableView()->reloadData(false);
  layoutSubviews();
}
