
#include "row_parameter_controller.h"

#include <escher/app.h>

namespace Statistics::Categorical {

RowParameterController::RowParameterController(
    Escher::Responder* parentResponder, TableData* tableData,
    Escher::StackViewController* stackViewController)
    : Escher::ExplicitSelectableListViewController(parentResponder),
      m_rowNameCell(&m_selectableListView, this),
      m_tableData(tableData),
      m_stackViewController(stackViewController) {
  m_rowNameCell.label()->setMessage(I18n::Message::RowName);
  // TODO update once i18n is here
  m_clearColumnCell.label()->setMessage(I18n::Message::ClearColumn);
}

bool RowParameterController::handleEvent(Ion::Events::Event event) {
  Escher::AbstractMenuCell* cell =
      static_cast<Escher::AbstractMenuCell*>(selectedCell());
  assert(m_row >= 0);
  if (!cell->canBeActivatedByEvent(event)) {
    return false;
  }
  if (cell == &m_clearColumnCell) {
    m_tableData->clearRow(m_row);
    m_stackViewController->pop();
    return true;
  }
  return false;
}

bool RowParameterController::textFieldDidFinishEditing(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  const char* name = textField->draftText();
  m_tableData->setCategoryName(name, m_row);
  char buffer[20];
  m_tableData->getCategoryName(m_row, buffer, sizeof(buffer));
  m_rowNameCell.textField()->setText(buffer);
  m_selectableListView.reloadSelectedCell();
  if (event == Ion::Events::Down) {
    m_selectableListView.handleEvent(event);
  }
  return true;
}

const Escher::AbstractMenuCell* RowParameterController::cell(int row) const {
  assert(0 <= row && row < k_numberOfCells);
  std::array<const Escher::AbstractMenuCell*, k_numberOfCells> cells = {
      &m_rowNameCell,
      &m_clearColumnCell,
  };
  return cells[row];
}

const char* RowParameterController::title() const {
  char buffer[20];
  m_tableData->getCategoryName(m_row, buffer, sizeof(buffer));
  Poincare::Print::CustomPrintf(m_titleBuffer, sizeof(m_titleBuffer),
                                I18n::translate(I18n::Message::RowOptions),
                                buffer);
  return m_titleBuffer;
}

void RowParameterController::setRow(int row) {
  m_row = row;
  char buffer[20];
  m_tableData->getCategoryName(row, buffer, sizeof(buffer));
  m_rowNameCell.textField()->setText(buffer);
  m_selectableListView.selectRow(0);
}

}  // namespace Statistics::Categorical
