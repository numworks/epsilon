#include "input_goodness_table_view.h"

#include <algorithm>

#include "probability/models/statistic/goodness_statistic.h"
#include "probability/text_helpers.h"

namespace Probability {

constexpr int InputGoodnessTableView::k_minimumNumberOfRows;

InputGoodnessTableView::InputGoodnessTableView(
    Escher::Responder * parentResponder,
    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
    GoodnessStatistic * statistic,
    Escher::TextFieldDelegate * textFieldDelegate,
    DynamicTableViewDataSourceDelegate * delegate,
    Escher::SelectableTableViewDelegate * scrollDelegate) :
    SelectableTableView(parentResponder, this, &m_tableSelection, scrollDelegate),
    DynamicTableViewDataSource(delegate),
    m_numberOfRows(k_minimumNumberOfRows),
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

void InputGoodnessTableView::recomputeNumberOfRows() {
  m_numberOfRows = std::max(m_statistic->computeNumberOfRows() + 2, k_minimumNumberOfRows);
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
