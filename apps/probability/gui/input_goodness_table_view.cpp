#include "input_goodness_table_view.h"

#include "probability/models/statistic/goodness_statistic.h"
#include "probability/text_helpers.h"

namespace Probability {

InputGoodnessTableView::InputGoodnessTableView(
    Escher::Responder * parentResponder,
    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
    Chi2Statistic * statistic,
    Escher::TextFieldDelegate * textFieldDelegate,
    DynamicTableViewDataSourceDelegate * delegate,
    Escher::SelectableTableViewDelegate * scrollDelegate) :
    SelectableTableView(parentResponder, this, &m_tableSelection, scrollDelegate),
    DynamicTableViewDataSource(delegate),
    m_numberOfRows(k_initialNumberOfRows),
    m_statistic(statistic) {
  m_header[0].setMessage(I18n::Message::Observed);
  m_header[1].setMessage(I18n::Message::Expected);
  m_header[0].setEven(true);
  m_header[1].setEven(true);
  m_header[0].setMessageFont(KDFont::SmallFont);
  m_header[1].setMessageFont(KDFont::SmallFont);

  for (unsigned int i = 0; i < sizeof(m_cells) / sizeof(Escher::EvenOddEditableTextCell); i++) {
    m_cells[i].setParentResponder(this);
    m_cells[i].editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate,
                                                             textFieldDelegate);
    m_cells[i].editableTextCell()->setMargins(0, k_innerCellRightMargin, 0, 0);
    m_cells[i].setEven((i / 2) % 2 == 1);
    m_cells[i].setFont(KDFont::SmallFont);
  }
}

int InputGoodnessTableView::reusableCellCount(int type) {
  if (type == k_typeOfHeader) {
    return k_numberOfColumns;
  }
  return k_maxNumberOfReusableRows * k_numberOfColumns;
}

Escher::HighlightCell * InputGoodnessTableView::reusableCell(int i, int type) {
  assert(i < reusableCellCount(type));
  if (type == k_typeOfHeader) {
    assert(i < 2);
    return &m_header[i];
  }
  return &m_cells[i];
}

bool InputGoodnessTableView::handleEvent(Ion::Events::Event e) {
  if (e == Ion::Events::Backspace) {
    // Remove value
    m_statistic->setParamAtIndex(2 * (selectedRow() - 1) + selectedColumn(),
                                 GoodnessStatistic::k_undefinedValue);
    if (selectedRow() == m_numberOfRows - 2 && m_numberOfRows > k_initialNumberOfRows &&
        std::isnan(m_statistic->paramAtIndex(2 * (selectedRow() - 1) + (1 - selectedColumn())))) {
      deleteLastRow();
      reloadData();
    }
    reloadCellAtLocation(selectedColumn(), selectedRow());
    return true;
  }
  return SelectableTableView::handleEvent(e);
}

void InputGoodnessTableView::addRow() {
  m_numberOfRows++;
  DynamicTableViewDataSource::notify();
}

void InputGoodnessTableView::deleteLastRow() {
  m_numberOfRows--;
  DynamicTableViewDataSource::notify();
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

}  // namespace Probability
