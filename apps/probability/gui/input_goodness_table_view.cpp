#include "input_goodness_table_view.h"

#include "probability/text_helpers.h"

namespace Probability {

InputGoodnessTableView::InputGoodnessTableView(Escher::Responder * parentResponder,
                                               Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
                                               Chi2Statistic * statistic, Escher::SelectableTableViewDelegate * delegate) :
    SelectableTableView(parentResponder, this, &m_tableSelection, delegate),
    m_numberOfRows(k_initialNumberOfRows),
    m_statistic(statistic) {
  m_header[0].setMessage(I18n::Message::Observed);
  m_header[1].setMessage(I18n::Message::Expected);
  m_header[0].setEven(true);
  m_header[1].setEven(true);
  m_header[0].setMessageFont(KDFont::SmallFont);
  m_header[1].setMessageFont(KDFont::SmallFont);

  for (int i = 0; i < sizeof(m_cells) / sizeof(Escher::EvenOddEditableTextCell); i++) {
    m_cells[i].setParentResponder(this);
    m_cells[i].editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate, this);
    m_cells[i].editableTextCell()->setMargins(0, k_innerCellRightMargin, 0, 0);
    m_cells[i].setEven((i / 2) % 2 == 1);
    m_cells[i].setFont(KDFont::SmallFont);
  }
}

int InputGoodnessTableView::reusableCellCount(int type) {
  if (type == k_typeOfHeader) {
    return k_numberOfColumns;
  }
   return k_maxNumberOfRows * k_numberOfColumns;
}

Escher::HighlightCell * InputGoodnessTableView::reusableCell(int i, int type) {
  assert(i < reusableCellCount(type));
  if (type == k_typeOfHeader) {
    assert(i < 2);
    return &m_header[i];
  }
  return &m_cells[i];
}

void Probability::InputGoodnessTableView::willDisplayCellAtLocation(Escher::HighlightCell * cell,
                                                                    int i,
                                                                    int j) {
  if (j == 0) {  // Header
    return;
  }
  int index = 2 * (j - 1) + i;
  float p = m_statistic->paramAtIndex(index);
  Escher::EvenOddEditableTextCell * myCell = static_cast<Escher::EvenOddEditableTextCell *>(cell);
  if (std::isnan(p)) {
    myCell->editableTextCell()->textField()->setText("");
  } else {
    constexpr int bufferSize = 20;
    char buffer[bufferSize];
    defaultParseFloat(p, buffer, bufferSize);
    myCell->editableTextCell()->textField()->setText(buffer);
  }
}

bool Probability::InputGoodnessTableView::textFieldShouldFinishEditing(
    Escher::TextField * textField,
    Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;  // TODO up and down too
}

bool Probability::InputGoodnessTableView::textFieldDidFinishEditing(Escher::TextField * textField,
                                                                    const char * text,
                                                                    Ion::Events::Event event) {
  float p;
  if (textFieldDelegateApp()->hasUndefinedValue(text, p, false, false)) {
    return false;
  }

  int index = 2 * (selectedRow() - 1) + selectedColumn();
  m_statistic->setParamAtIndex(index, p);
  if (selectedRow() == numberOfRows() - 1 && numberOfRows() < k_maxNumberOfRows) {
    m_numberOfRows++;
  }
  selectCellAtLocation(selectedColumn(), selectedRow() + 1);
  reloadData(false);
  return true;
}

}  // namespace Probability
