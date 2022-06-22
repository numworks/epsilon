#ifndef SHARED_FLOAT_PARAMETER_CONTROLLER_H
#define SHARED_FLOAT_PARAMETER_CONTROLLER_H

#include "float_parameter_controller_without_button.h"
#include "button_with_separator.h"

namespace Shared {

/* This controller edits float parameter of any model (given through
 * parameterAtIndex and setParameterAtIndex). */

template<typename T>
class FloatParameterController : public FloatParameterControllerWithoutButton<T> {
public:
  FloatParameterController(Escher::Responder * parentResponder);
  void viewWillAppear() override;

  int typeAtIndex(int index) override;
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;

protected:
  constexpr static int k_buttonCellType = 1;
  virtual void buttonAction();

  ButtonWithSeparator m_okButton;

private:
  virtual int reusableParameterCellCount(int type) = 0;
  virtual Escher::HighlightCell * reusableParameterCell(int index, int type) = 0;
};

}

#endif
