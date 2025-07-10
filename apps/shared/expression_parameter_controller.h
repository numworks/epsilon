#ifndef SHARED_EXPRESSION_PARAMETER_CONTROLLER_H
#define SHARED_EXPRESSION_PARAMETER_CONTROLLER_H

#include <poincare/expression_or_float.h>

#include "parameters_with_validation_controller.h"

namespace Shared {

class ExpressionParameterController
    : public ParametersWithValidationController {
 public:
  using ParameterType = Poincare::ExpressionOrFloat;
  using FloatType = float;

  ExpressionParameterController(Escher::Responder* parentResponder,
                                Escher::View* topView = nullptr,
                                Escher::View* bottomView = nullptr);

  // MemoizedListViewDataSource
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;

  // ParameterTextFieldDelegate
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

 protected:
  enum class InfinityTolerance { None, PlusInfinity, MinusInfinity };

  virtual ParameterType parameterAtIndex(int index) = 0;
  virtual bool hasUndefinedValue(const char* text, ParameterType value,
                                 int row) const;

 private:
  virtual InfinityTolerance infinityAllowanceForRow(int row) const {
    return InfinityTolerance::None;
  }

  virtual bool setParameterAtIndex(int parameterIndex, ParameterType value) = 0;
};

}  // namespace Shared

#endif
