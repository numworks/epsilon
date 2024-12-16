#ifndef SHARED_FLOAT_PARAMETER_CONTROLLER_H
#define SHARED_FLOAT_PARAMETER_CONTROLLER_H

#include <escher/button_cell.h>
#include <escher/list_with_top_and_bottom_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field.h>

#include "parameter_text_field_delegate.h"

namespace Shared {

/* This controller edits float parameter of any model (given through
 * parameterAtIndex and setParameterAtIndex). */

template <typename T>
class FloatParameterController : public Escher::ListWithTopAndBottomController,
                                 public ParameterTextFieldDelegate {
 public:
  FloatParameterController(Escher::Responder* parentResponder,
                           Escher::View* topView = nullptr);

  // ListWithTopAndBottomController
  bool handleEvent(Ion::Events::Event event) override;

  // MemoizedListViewDataSource
  int typeAtRow(int row) const override;
  int reusableCellCount(int type) const override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  KDCoordinate separatorBeforeRow(int row) override {
    return typeAtRow(row) == k_buttonCellType ? k_defaultRowSeparator : 0;
  }

  // ParameterTextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

 protected:
  constexpr static int k_parameterCellType = 0;
  constexpr static int k_buttonCellType = 1;

  enum class InfinityTolerance { None, PlusInfinity, MinusInfinity };
  Escher::StackViewController* stackController() {
    return static_cast<Escher::StackViewController*>(parentResponder());
  }
  virtual T parameterAtIndex(int index) = 0;
  virtual void buttonAction();
  virtual bool hasUndefinedValue(const char* text, T floatValue, int row) const;

  Escher::ButtonCell m_okButton;

 private:
  virtual InfinityTolerance infinityAllowanceForRow(int row) const {
    return InfinityTolerance::None;
  }
  virtual int reusableParameterCellCount(int type) const = 0;
  virtual Escher::HighlightCell* reusableParameterCell(int index, int type) = 0;
  virtual Escher::TextField* textFieldOfCellAtIndex(Escher::HighlightCell* cell,
                                                    int index) = 0;
  virtual bool setParameterAtIndex(int parameterIndex, T f) = 0;
};

}  // namespace Shared

#endif
