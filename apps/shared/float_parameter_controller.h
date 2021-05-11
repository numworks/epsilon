#ifndef SHARED_FLOAT_PARAMETER_CONTROLLER_H
#define SHARED_FLOAT_PARAMETER_CONTROLLER_H

#include "simple_float_parameter_controller.h"
#include "button_with_separator.h"

namespace Shared {

template<typename T>
class FloatParameterController : public SimpleFloatParameterController<T> {
public:
  FloatParameterController(Escher::Responder * parentResponder);
  void viewWillAppear() override;

  int typeAtIndex(int index) override;
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
protected:
  static constexpr int k_buttonCellType = 1;

  virtual void buttonAction();
  ButtonWithSeparator m_okButton;
};

}

#endif
