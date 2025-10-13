
#include "column_parameter_controller.h"

#include <escher/app.h>

#include "store_controller.h"

namespace Statistics::Categorical {

ColumnParameterController::ColumnParameterController(
    Escher::Responder* parentResponder, Store* store,
    Escher::StackViewController* stackViewController,
    StoreController* storeController)
    : Escher::ExplicitSelectableListViewController(parentResponder),
      m_columnNameCell(&m_selectableListView, this),
      m_store(store),
      m_stackViewController(stackViewController),
      m_storeController(storeController) {
  m_columnNameCell.label()->setMessage(I18n::Message::ColumnName);
  m_showInGraphCell.label()->setMessage(
      I18n::Message::CategoricalActivateDeactivateStoreParamTitle);
  m_showInGraphCell.subLabel()->setMessage(
      I18n::Message::CategoricalActivateDeactivateStoreParamDescription);
  m_clearColumnCell.label()->setMessage(I18n::Message::ClearColumn);
  m_relativeFreqCell.label()->setMessage(I18n::Message::RelativeFrequency);
}

bool ColumnParameterController::handleEvent(Ion::Events::Event event) {
  Escher::AbstractMenuCell* cell =
      static_cast<Escher::AbstractMenuCell*>(selectedCell());
  assert(m_column >= 0);
  if (!cell->canBeActivatedByEvent(event)) {
    return false;
  }
  if (cell == &m_clearColumnCell) {
    m_stackViewController->pop();
    m_storeController->popupConfirmation(true, m_column);
    return true;
  }
  if (cell == &m_showInGraphCell) {
    if (m_store->isGroupEmpty(m_column)) {
      Escher::App::app()->displayWarning(I18n::Message::DataNotSuitable);
    } else {
      bool newState = !m_store->isGroupActive(m_column);
      m_store->setGroupActive(newState, m_column);
      m_showInGraphCell.accessory()->setState(newState);
      m_selectableListView.reloadSelectedCell();
    }
    return true;
  }
  if (cell == &m_relativeFreqCell) {
    bool newState = !m_store->isRelativeFrequencyColumnActive(m_column);
    m_store->setRelativeFrequencyColumn(m_column, newState);
    m_relativeFreqCell.accessory()->setState(newState);
    m_selectableListView.reloadSelectedCell();
    return true;
  }
  return false;
}

bool ColumnParameterController::textFieldShouldFinishEditing(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  return TextFieldDelegate::textFieldShouldFinishEditing(textField, event) ||
         event == Ion::Events::Down;
}

bool ColumnParameterController::textFieldDidFinishEditing(
    Escher::AbstractTextField* textField, Ion::Events::Event event) {
  const char* name = textField->draftText();
  m_store->setGroupName(name, m_column);
  char buffer[20];
  m_store->getGroupName(m_column, buffer, sizeof(buffer));
  m_columnNameCell.textField()->setText(buffer);
  m_selectableListView.reloadSelectedCell();
  if (event == Ion::Events::Down) {
    m_selectableListView.handleEvent(event);
  }
  return true;
}

const Escher::AbstractMenuCell* ColumnParameterController::cell(int row) const {
  assert(0 <= row && row < k_numberOfCells);
  std::array<const Escher::AbstractMenuCell*, k_numberOfCells> cells = {
      &m_columnNameCell, &m_showInGraphCell, &m_clearColumnCell,
      &m_relativeFreqCell};
  return cells[row];
}

const char* ColumnParameterController::title() const {
  char buffer[20];
  m_store->getGroupName(m_column, buffer, sizeof(buffer));
  Poincare::Print::CustomPrintf(m_titleBuffer, sizeof(m_titleBuffer),
                                I18n::translate(I18n::Message::ColumnOptions),
                                buffer);
  return m_titleBuffer;
}

void ColumnParameterController::setColumn(int col) {
  m_column = col;
  m_showInGraphCell.accessory()->setState(m_store->isGroupActive(m_column));
  m_relativeFreqCell.accessory()->setState(
      m_store->isRelativeFrequencyColumnActive(m_column));
  char buffer[20];
  m_store->getGroupName(col, buffer, sizeof(buffer));
  m_columnNameCell.textField()->setText(buffer);
  m_selectableListView.selectRow(0);
}

}  // namespace Statistics::Categorical
