#include "input_categorical_view.h"

#include <escher/invocation.h>
#include "probability/abstract/table_view_controller.h"
#include <escher/horizontal_or_vertical_layout.h>
#include "probability/text_helpers.h"
#include "probability/constants.h"

using namespace Probability;

Escher::View * InputCategoricalView::ContentView::subviewAtIndex(int i) {
  switch (i) {
    case k_indexOfTable:
      return m_dataInputTableView;
      break;
    case k_indexOfInnerLayout:
      return innerView();
      break;
  }
  assert(false);
  return nullptr;
}

InputCategoricalView::InputCategoricalView(Responder * parentResponder,
                                           Escher::Invocation invocation,
                                           TableViewController * tableViewController,
                                           Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
                                           Escher::TextFieldDelegate * textFieldDelegate,
                                           View * contentView) :
      Escher::ScrollView(contentView, &m_scrollDataSource),
      Responder(parentResponder),
      m_tableViewController(tableViewController),
      m_significanceCell(this, inputEventHandlerDelegate, textFieldDelegate),
      m_next(this,
             I18n::Message::Next,
             invocation,
             Escher::Palette::WallScreenDark) {
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
      int jump = 1 + (m_viewSelection.selectedRow() == indexOfSpacer() + 1);
      m_viewSelection.selectRow(m_viewSelection.selectedRow() - jump);
      highlightCorrectView();
      setResponderForSelectedRow();
    }
    if (event == Ion::Events::Down && m_viewSelection.selectedRow() < indexOfNext()) {
      int jump = 1 + (m_viewSelection.selectedRow() == indexOfSpacer() - 1);
      m_viewSelection.selectRow(m_viewSelection.selectedRow() + jump);
      highlightCorrectView();
      setResponderForSelectedRow();
    }
    return true;
  }
  return false;
}

Escher::Responder * InputCategoricalView::responderForRow(int row) {
  if (row == k_indexOfTable) {
    return m_tableViewController;
  }
  if (row == indexOfSignificance()) {
    return &m_significanceCell;
  }
  assert(row == indexOfNext());
  return &m_next;
}

void InputCategoricalView::setResponderForSelectedRow() {
  Escher::Container::activeApp()->setFirstResponder(responderForRow(m_viewSelection.selectedRow()));
}

KDSize Probability::InputCategoricalView::minimalSizeForOptimalDisplay() const {
  /* SelectableTableView must be given a width, so that it can percolate it to
   * its cells. The cells might need their widths to know their heights. */
  constContentView()->tableView()->initWidth(bounds().width());
  KDSize requiredSize = ScrollView::minimalSizeForOptimalDisplay();
  return KDSize(bounds().width() + leftMargin() + rightMargin(), requiredSize.height());
}

void Probability::InputCategoricalView::selectViewAtIndex(int index) {
  m_viewSelection.selectRow(index);
  highlightCorrectView();
  setResponderForSelectedRow();
}

void Probability::InputCategoricalView::updateSignificanceCell(Chi2Test * statistic) {
  setTextFieldText(statistic->threshold(), m_significanceCell.textField());
}

void Probability::InputCategoricalView::setTableView(TableViewController * tableViewController) {
  m_tableViewController = tableViewController;
  Escher::SelectableTableView * tableView = tableViewController->selectableTableView();
  contentView()->setTableView(tableView);
  tableView->setMargins(0, Escher::Metric::CommonRightMargin, k_marginVertical, Escher::Metric::CommonLeftMargin);
  tableView->setBackgroundColor(Escher::Palette::WallScreenDark);
  tableView->setDecoratorType(Escher::ScrollView::Decorator::Type::None);
}

void Probability::InputCategoricalView::setTextFieldText(double value, Escher::TextField * textField) {
  if (std::isnan(value) || value < 0) {
    /* Alpha and DegreeOfFreedom cannot be negative. However, DegreeOfFreedom
     * can be computed to a negative when there are no rows.
     * In that case, the degreeOfFreedom cell should display nothing. */
    return textField->setText("");
  }
  constexpr int bufferSize = Constants::k_shortBufferSize;
  char buffer[bufferSize];
  defaultConvertFloatToText(value, buffer, bufferSize);
  return textField->setText(buffer);
}

void Probability::InputCategoricalView::tableViewDataSourceDidChangeSize() {
  /* Relayout when inner table changes size. We need to reload the table because
   * its width might change but it won't relayout as its frame isn't changed by
   * the InputCategoricalController */
  m_tableViewController->selectableTableView()->reloadData(false);
  layoutSubviews();
}
