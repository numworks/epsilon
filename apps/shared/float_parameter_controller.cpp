#include "float_parameter_controller.h"
#include <assert.h>

using namespace Escher;
using namespace Poincare;

namespace Shared {

template<typename T>
FloatParameterController<T>::FloatParameterController(Responder * parentResponder) :
  SimpleFloatParameterController<T>(parentResponder),
  m_okButton(&(this->m_selectableTableView), I18n::Message::Ok, Invocation([](void * context, void * sender) {
      FloatParameterController * parameterController = (FloatParameterController *) context;
      parameterController->buttonAction();
      return true;
    }, this))
{}

template<typename T>
void FloatParameterController<T>::viewWillAppear() {
  if (this->selectedRow() == this->numberOfRows()-1) {
    FloatParameterController<T>::selectCellAtLocation(0, 0);
  };
  SimpleFloatParameterController<T>::viewWillAppear();
}

template<typename T>
int FloatParameterController<T>::typeAtIndex(int index) {
  if (index == this->numberOfRows()-1) {
    return k_buttonCellType;
  }
  return SimpleFloatParameterController<T>::typeAtIndex(index);
}

template<typename T>
int FloatParameterController<T>::reusableCellCount(int type) {
  if (type == k_buttonCellType) {
    return 1;
  }
  return SimpleFloatParameterController<T>::reusableCellCount(type);
}

template<typename T>
HighlightCell * FloatParameterController<T>::reusableCell(int index, int type) {
  if (type == k_buttonCellType) {
    return &m_okButton;
  }
  return SimpleFloatParameterController<T>::reusableCell(index, type);
}

template<typename T>
KDCoordinate FloatParameterController<T>::nonMemoizedRowHeight(int j) {
  if (j == this->numberOfRows()-1) {
    return m_okButton.minimalSizeForOptimalDisplay().height();
  }
  return SimpleFloatParameterController<T>::nonMemoizedRowHeight(j);
}

template<typename T>
void FloatParameterController<T>::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == this->numberOfRows()-1) {
    return;
  }
  SimpleFloatParameterController<T>::willDisplayCellForIndex(cell, index);
}

template<typename T>
void FloatParameterController<T>::buttonAction() {
  StackViewController * stack = FloatParameterController<T>::stackController();
  stack->pop();
}

template class FloatParameterController<float>;
template class FloatParameterController<double>;

}
