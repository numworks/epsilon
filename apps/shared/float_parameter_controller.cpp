#include "float_parameter_controller.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

template<typename T>
FloatParameterController<T>::FloatParameterController(Responder * parentResponder) :
  FloatParameterControllerWithoutButton<T>(parentResponder),
  m_okButton(&(this->m_selectableTableView), I18n::Message::Ok, Invocation([](void * context, void * sender) {
      FloatParameterController * parameterController = (FloatParameterController *) context;
      parameterController->buttonAction();
      return true;
    }, this))
{}

template<typename T>
void FloatParameterController<T>::viewWillAppear() {
  int selRow = FloatParameterControllerWithoutButton<T>::selectedRow();
  if (typeAtIndex(selRow) == k_buttonCellType) {
    ViewController::viewWillAppear();
    FloatParameterControllerWithoutButton<T>::selectCellAtLocation(0, 0);
    FloatParameterControllerWithoutButton<T>::resetMemoization();
    FloatParameterControllerWithoutButton<T>::m_selectableTableView.reloadData();
    return;
  }
  FloatParameterControllerWithoutButton<T>::viewWillAppear();
}

template<typename T>
int FloatParameterController<T>::typeAtIndex(int index) {
  if (index == this->numberOfRows() - 1) {
    return k_buttonCellType;
  }
  return FloatParameterControllerWithoutButton<T>::k_parameterCellType;
}

template<typename T>
int FloatParameterController<T>::reusableCellCount(int type) {
  if (type == k_buttonCellType) {
    return 1;
  }
  return reusableParameterCellCount(type);
}

template<typename T>
HighlightCell * FloatParameterController<T>::reusableCell(int index, int type) {
  if (type == k_buttonCellType) {
    return &m_okButton;
  }
  return reusableParameterCell(index, type);
}

template<typename T>
void FloatParameterController<T>::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (typeAtIndex(index) == k_buttonCellType) {
    return;
  }
  FloatParameterControllerWithoutButton<T>::willDisplayCellForIndex(cell, index);
}

template<typename T>
KDCoordinate FloatParameterController<T>::nonMemoizedRowHeight(int j) {
  if (typeAtIndex(j) == k_buttonCellType) {
    return m_okButton.minimalSizeForOptimalDisplay().height();
  }
  return FloatParameterControllerWithoutButton<T>::nonMemoizedRowHeight(j);
}

template<typename T>
void FloatParameterController<T>::buttonAction() {
  StackViewController * stack = FloatParameterController<T>::stackController();
  stack->pop();
}

template class FloatParameterController<float>;
template class FloatParameterController<double>;

}
