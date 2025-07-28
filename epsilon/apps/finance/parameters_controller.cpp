#include "parameters_controller.h"

#include <poincare/print.h>

#include "app.h"

using namespace Escher;

namespace Finance {

ParametersController::ParametersController(StackViewController* parent,
                                           ResultController* resultController)
    : Shared::FloatParameterController<double>(parent, &m_messageView),
      m_dropdown(&m_selectableListView, &m_dropdownDataSource, this),
      m_resultController(resultController),
      m_messageView(I18n::Message::DefineParameters, k_messageFormat) {
  for (size_t i = 0; i < k_numberOfReusableInputs; i++) {
    m_cells[i].setParentResponder(&m_selectableListView);
    m_cells[i].setDelegate(this);
  }
  m_dropdownCell.accessory()->setDropdown(&m_dropdown);
}

const char* ParametersController::title() const {
  uint8_t unknownParam = App::GetInterestData()->getUnknown();
  const char* label =
      I18n::translate(App::GetInterestData()->labelForParameter(unknownParam));
  int length = Poincare::Print::UnsafeCustomPrintf(
      m_titleBuffer, k_titleBufferSize,
      I18n::translate(I18n::Message::FinanceSolving), label,
      I18n::translate(
          App::GetInterestData()->sublabelForParameter(unknownParam)));
  if (length >= k_titleBufferSize) {
    // Title did not fit, use a reduced pattern
    Poincare::Print::CustomPrintf(
        m_titleBuffer, k_titleBufferSize,
        I18n::translate(I18n::Message::FinanceSolvingReduced), label);
  }
  return m_titleBuffer;
}

void ParametersController::viewWillAppear() {
  // Init from data
  m_dropdownDataSource.setMessages(
      App::GetInterestData()->dropdownMessageAtIndex(0),
      App::GetInterestData()->dropdownMessageAtIndex(1));
  m_dropdown.selectRow(App::GetInterestData()->m_booleanParam ? 0 : 1);
  m_dropdown.init();
  m_dropdown.reloadCell();
  ListWithTopAndBottomController::viewWillAppear();
}

bool ParametersController::handleEvent(Ion::Events::Event event) {
  return popFromStackViewControllerOnLeftEvent(event);
}

void ParametersController::fillCellForRow(HighlightCell* cell, int row) {
  int type = typeAtRow(row);
  if (type == k_buttonCellType) {
    return;
  }
  uint8_t param = interestParameterAtIndex(row);
  if (type == k_dropdownCellType) {
    assert(&m_dropdownCell == cell);
    m_dropdownCell.label()->setMessage(
        App::GetInterestData()->labelForParameter(param));
    m_dropdownCell.subLabel()->setMessage(
        App::GetInterestData()->sublabelForParameter(param));
    return;
  }
  assert(type == k_inputCellType);
  MenuCellWithEditableText<MessageTextView, MessageTextView>* myCell =
      static_cast<MenuCellWithEditableText<MessageTextView, MessageTextView>*>(
          cell);
  myCell->label()->setMessage(App::GetInterestData()->labelForParameter(param));
  myCell->subLabel()->setMessage(
      App::GetInterestData()->sublabelForParameter(param));
  return Shared::FloatParameterController<double>::fillCellForRow(cell, row);
}

int ParametersController::typeAtRow(int row) const {
  if (row < indexOfDropdown()) {
    return k_inputCellType;
  }
  return (row == indexOfDropdown()) ? k_dropdownCellType : k_buttonCellType;
}

KDCoordinate ParametersController::nonMemoizedRowHeight(int row) {
  int type = typeAtRow(row);
  if (type == k_inputCellType) {
    MenuCellWithEditableText<MessageTextView, MessageTextView> tempCell;
    return protectedNonMemoizedRowHeight(&tempCell, row);
  } else if (type == k_dropdownCellType) {
    return protectedNonMemoizedRowHeight(&m_dropdownCell, row);
  }
  assert(type == k_buttonCellType);
  return Shared::FloatParameterController<double>::nonMemoizedRowHeight(row);
}

int ParametersController::numberOfRows() const {
  return App::GetInterestData()->numberOfParameters();
}

void ParametersController::onDropdownSelected(int selectedRow) {
  App::GetInterestData()->m_booleanParam = (selectedRow == 0);
}

uint8_t ParametersController::interestParameterAtIndex(int index) const {
  uint8_t unknownParam = App::GetInterestData()->getUnknown();
  assert(unknownParam < App::GetInterestData()->numberOfUnknowns());
  if (unknownParam <= index) {
    index += 1;
  }
  assert(index < App::GetInterestData()->numberOfParameters());
  return index;
}

int ParametersController::reusableParameterCellCount(int type) const {
  if (type == k_inputCellType) {
    return k_numberOfReusableInputs;
  }
  return 1;
}

HighlightCell* ParametersController::reusableParameterCell(int i, int type) {
  switch (type) {
    case k_inputCellType:
      assert(i < k_numberOfReusableInputs);
      return m_cells + i;
    default:
      assert(type == k_dropdownCellType && i == 0);
      return &m_dropdownCell;
  }
}

TextField* ParametersController::textFieldOfCellAtIndex(HighlightCell* cell,
                                                        int index) {
  assert(typeAtRow(index) == k_parameterCellType);
  return static_cast<
             MenuCellWithEditableText<MessageTextView, MessageTextView>*>(cell)
      ->textField();
}

double ParametersController::parameterAtIndex(int index) {
  return App::GetInterestData()->getValue(interestParameterAtIndex(index));
}

bool ParametersController::setParameterAtIndex(int parameterIndex, double f) {
  uint8_t param = interestParameterAtIndex(parameterIndex);
  if (!App::GetInterestData()->checkValue(param, f)) {
    App::app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  App::GetInterestData()->setValue(param, f);
  return true;
}

int ParametersController::indexOfDropdown() const {
  return App::GetInterestData()->numberOfDoubleValues() - 1;
}

}  // namespace Finance
