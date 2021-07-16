#include "input_categorical_view.h"

#include "probability/text_helpers.h"

using namespace Probability;

Escher::View * InputCategoricalView::ContentView::InnerVerticalLayout::subviewAtIndex(int i) {
  switch (i) {
    case 0:
      return m_significanceCell;
      break;
    case 1:
      return m_spacer;
    case 2:
      return m_next;
  }
  return nullptr;
}

Probability::InputCategoricalView::ContentView::ContentView(
    SelectableTableView * dataInputTableView,
    MessageTableCellWithEditableTextWithMessage * significanceCell,
    Escher::Button * next) :
    VerticalLayout(Palette::WallScreenDark),
    m_dataInputTableView(dataInputTableView),
    m_innerView(significanceCell, &m_spacer, next),
    m_spacer(Palette::WallScreenDark, 0, k_marginVertical) {
  significanceCell->setMessage(I18n::Message::GreekAlpha);
  significanceCell->setSubLabelMessage(I18n::Message::SignificanceLevel);
  significanceCell->textField()->setText("0.05");  // TODO kinda ugly?
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
}

InputCategoricalView::InputCategoricalView(Responder * parentResponder,
                                           ButtonDelegate * buttonDelegate,
                                           TableViewController * tableViewController,
                                           InputEventHandlerDelegate * inputEventHandlerDelegate,
                                           TextFieldDelegate * textFieldDelegate) :
    Escher::ScrollView(&m_contentView, &m_scrollDataSource),
    Responder(parentResponder),
    m_tableViewController(tableViewController),
    m_significanceCell(this, inputEventHandlerDelegate, textFieldDelegate),
    m_next(this, I18n::Message::Ok, buttonDelegate->buttonActionInvocation(), KDFont::LargeFont),
    m_contentView(tableViewController ? tableViewController->selectableTableView() : nullptr,
                  &m_significanceCell,
                  &m_next) {
  m_tableSelection.selectColumn(-1);
}

void InputCategoricalView::didBecomeFirstResponder() {
  // Pass focus to subview
  if (m_viewSelection.selectedRow() < 0) {
    m_viewSelection.selectRow(0);
  }
  setResponderForSelectedRow();
  selectCorrectView();
}

bool InputCategoricalView::handleEvent(Ion::Events::Event event) {
  // Move selection between views
  if (event == Ion::Events::Up || event == Ion::Events::Down) {
    if (event == Ion::Events::Up && m_viewSelection.selectedRow() > 0) {
      int jump = 1 + (m_viewSelection.selectedRow() == k_indexOfSpacer + 1);
      m_viewSelection.selectRow(m_viewSelection.selectedRow() - jump);
    }
    if (event == Ion::Events::Down && m_viewSelection.selectedRow() < k_indexOfNext) {
      int jump = 1 + (m_viewSelection.selectedRow() == k_indexOfSpacer - 1);
      m_viewSelection.selectRow(m_viewSelection.selectedRow() + jump);
    }
    selectCorrectView();
    setResponderForSelectedRow();
    return true;
  }
  return false;
}

Responder * InputCategoricalView::responderForRow(int row) {
  switch (m_viewSelection.selectedRow()) {
    case k_indexOfTable:
      return m_tableViewController;
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
      break;
    case k_indexOfSignificance:
      m_significanceCell.setHighlighted(true);
      // TODO compute position of cell and scrollToContentRect
      scrollToContentPoint(m_significanceCell.pointFromPointInView(this, KDPointZero)
                               .opposite()
                               .translatedBy(KDPoint(0, 100)));
      break;
    default:
      assert(m_viewSelection.selectedRow() == k_indexOfNext);
      m_next.setHighlighted(true);
      scrollToContentPoint(
          m_next.pointFromPointInView(this, KDPointZero).opposite().translatedBy(KDPoint(0, 100)));
      break;
  }
}

KDSize Probability::InputCategoricalView::minimalSizeForOptimalDisplay() const {
  // Pass expected size to VerticalLayout to propagate to TableCells
  ContentView * contentView = const_cast<ContentView *>(&m_contentView);
  contentView->setSize(KDSize(bounds().width(), 10000));
  KDSize requiredSize = contentView->minimalSizeForOptimalDisplay();
  return KDSize(bounds().width(), requiredSize.height());
}

void Probability::InputCategoricalView::setSignificanceCellText(const char * text) {
  m_significanceCell.textField()->setText(text);
}

void Probability::InputCategoricalView::setTableView(TableViewController * tableViewController) {
  m_tableViewController = tableViewController;
  SelectableTableView * tableView = tableViewController->selectableTableView();
  m_contentView.setTableView(tableView);
  tableView->setMargins(Metric::CommonTopMargin,
                        Metric::CommonRightMargin,
                        k_marginVertical,
                        Metric::CommonLeftMargin);
  tableView->setBackgroundColor(Palette::WallScreenDark);
  tableView->setDecoratorType(Escher::ScrollView::Decorator::Type::None);
}

void Probability::InputCategoricalView::tableViewDataSourceDidChangeSize() {
  // Relayout when inner table changes size
  layoutSubviews(true);
}
