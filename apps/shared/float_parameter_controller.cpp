#include "float_parameter_controller.h"
#include "../shared/poincare_helpers.h"
#include <poincare/preferences.h>
#include <assert.h>
#include <cmath>

using namespace Poincare;

namespace Shared {

template<typename T>
FloatParameterController<T>::FloatParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this),
  m_okButton(&m_selectableTableView, I18n::Message::Ok, Invocation([](void * context, void * sender) {
      FloatParameterController * parameterController = (FloatParameterController *) context;
      parameterController->buttonAction();
      return true;
    }, this))
{
}

template<typename T>
void FloatParameterController<T>::didBecomeFirstResponder() {
  if (selectedRow() >= 0) {
    int selRow = selectedRow();
    selRow = selRow >= numberOfRows() ? numberOfRows()-1 : selRow;
    int selColumn = selectedColumn();
    selColumn = selColumn >= numberOfColumns() ? numberOfColumns() - 1 : selColumn;
    selectCellAtLocation(selColumn, selRow);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

template<typename T>
void FloatParameterController<T>::viewWillAppear() {
  ViewController::viewWillAppear();
  if (selectedRow() == -1 || selectedRow() == numberOfRows()-1) {
    selectCellAtLocation(0, 0);
  } else {
    int selRow = selectedRow();
    selRow = selRow >= numberOfRows() ? numberOfRows()-1 : selRow;
    int selColumn = selectedColumn();
    selColumn = selColumn >= numberOfColumns() ? numberOfColumns() - 1 : selColumn;
    selectCellAtLocation(selColumn, selRow);
  }
  m_selectableTableView.reloadData();
}

template<typename T>
void FloatParameterController<T>::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == nullptr) {
    return;
  }
  if (parentResponder() == nullptr) {
    m_selectableTableView.deselectTable();
    m_selectableTableView.scrollToCell(0,0);
  }
}

template<typename T>
bool FloatParameterController<T>::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    stackController()->pop();
    return true;
  }
  return false;
}

template<typename T>
int FloatParameterController<T>::typeAtLocation(int i, int j) {
  if (j == numberOfRows()-1) {
    return 0;
  }
  return 1;
}

template<typename T>
int FloatParameterController<T>::reusableCellCount(int type) {
  if (type == 0) {
    return 1;
  }
  return reusableParameterCellCount(type);
}

template<typename T>
HighlightCell * FloatParameterController<T>::reusableCell(int index, int type) {
  if (type == 0) {
    return &m_okButton;
  }
  return reusableParameterCell(index, type);
}

template<typename T>
KDCoordinate FloatParameterController<T>::rowHeight(int j) {
  if (j == numberOfRows()-1) {
    return Metric::ParameterCellHeight+k_buttonMargin;
  }
  return Metric::ParameterCellHeight;
}

template<typename T>
KDCoordinate FloatParameterController<T>::cumulatedHeightFromIndex(int j) {
  if (j == numberOfRows()) {
    return j*Metric::ParameterCellHeight+k_buttonMargin;
  }
  return Metric::ParameterCellHeight*j;
}

template<typename T>
int FloatParameterController<T>::indexFromCumulatedHeight(KDCoordinate offsetY) {
  if (offsetY > numberOfRows()*Metric::ParameterCellHeight + k_buttonMargin) {
    return numberOfRows();
  }
  return (offsetY - 1) / Metric::ParameterCellHeight;
}

template<typename T>
void FloatParameterController<T>::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *) cell;
  if (myCell->isEditing()) {
    return;
  }
  constexpr int precision = Preferences::LargeNumberOfSignificantDigits;
  constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  PoincareHelpers::ConvertFloatToTextWithDisplayMode<T>(parameterAtIndex(index), buffer, bufferSize, precision, Preferences::PrintFloatMode::Decimal);
  myCell->setAccessoryText(buffer);
}

template<typename T>
bool FloatParameterController<T>::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return (event == Ion::Events::Down && selectedRow() < numberOfRows()-1)
     || (event == Ion::Events::Up && selectedRow() > 0)
     || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

template<typename T>
bool FloatParameterController<T>::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  T floatBody;
  int row = selectedRow();
  InfinityTolerance infTolerance = infinityAllowanceForRow(row);
  if (textFieldDelegateApp()->hasUndefinedValue(text, floatBody, infTolerance == InfinityTolerance::PlusInfinity, infTolerance == InfinityTolerance::MinusInfinity)) {
    return false;
  }
  if (!setParameterAtIndex(row, floatBody)) {
    return false;
  }
  m_selectableTableView.reloadCellAtLocation(0, activeCell());
  m_selectableTableView.reloadData();
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    m_selectableTableView.selectCellAtLocation(selectedColumn(), row + 1);
  } else {
    m_selectableTableView.handleEvent(event);
  }
  return true;
}

template<typename T>
int FloatParameterController<T>::activeCell() {
  return selectedRow();
}

template<typename T>
StackViewController * FloatParameterController<T>::stackController() {
  return (StackViewController *)parentResponder();
}

template<typename T>
void FloatParameterController<T>::buttonAction() {
  StackViewController * stack = stackController();
  stack->pop();
}

template class FloatParameterController<float>;
template class FloatParameterController<double>;

}
