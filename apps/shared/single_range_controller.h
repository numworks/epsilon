#ifndef SHARED_SINGLE_RANGE_CONTROLLER_H
#define SHARED_SINGLE_RANGE_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/switch_view.h>
#include <poincare/range.h>

#include "float_parameter_controller.h"
#include "pop_up_controller.h"
#include "shared/expression_parameter_controller.h"

namespace Shared {

template <typename ParentType>
class SingleRangeController : public ParentType {
 public:
  SingleRangeController(Escher::Responder* parentResponder,
                        MessagePopUpController* confirmPopUpController);

  /* ParameterType is the return type of the parameterAtIndex method.
   * FloatType is the type of the range parameter (m_rangeParam), which needs to
   * be a floating-point type.
   * ParameterType and FloatType can be the same (like in ParentType =
   * FloatParameterController<float> and ParentType =
   * FloatParameterController<double>), but they can also be different.
   * For instance for ParentType = ExpressionParameterController, it's useful
   * that parameterAtIndex returns a SerializedExpression. That is because in
   * the child class SingleInteractiveCurveViewRangeController, there is another
   * parameter beside the range (namely m_gridUnitParam), which needs to have
   * the same type for the parameterAtIndex method. */
  using ParameterType = typename ParentType::ParameterType;
  using FloatType = typename ParentType::FloatType;

  void viewWillAppear() override;

  int numberOfRows() const override { return k_numberOfBoundsCells + 2; }
  int typeAtRow(int row) const override {
    return row == 0 ? k_autoCellType : ParentType::typeAtRow(row);
  }
  int reusableCellCount(int type) const override {
    return type == k_autoCellType ? 1 : ParentType::reusableCellCount(type);
  }
  Escher::HighlightCell* reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;

  bool handleEvent(Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

 protected:
  constexpr static int k_numberOfBoundsCells = 2;
  constexpr static int k_autoCellType = 2;
  static_assert(k_autoCellType != ParentType::k_parameterCellType &&
                    k_autoCellType != ParentType::k_buttonCellType,
                "k_autoCellType value already taken.");

  virtual I18n::Message parameterMessage(int index) const = 0;
  virtual bool parametersAreDifferent() = 0;
  virtual void extractParameters() = 0;
  ParameterType parameterAtIndex(int index) override;
  void setAutoStatus(bool autoParam);
  virtual void setAutoRange() = 0;
  bool setParameterAtIndex(int parameterIndex, ParameterType value) override;
  void setRange(FloatType min, FloatType max);
  virtual FloatType limit() const = 0;
  virtual void confirmParameters() = 0;
  virtual void pop(bool onConfirmation) = 0;
  int reusableParameterCellCount(int type) const override {
    assert(type == this->k_parameterCellType);
    return k_numberOfBoundsCells;
  }
  Escher::HighlightCell* reusableParameterCell(int index, int type) override;
  Escher::TextField* textFieldOfCellAtIndex(Escher::HighlightCell* cell,
                                            int index) override;

  Escher::MenuCellWithEditableText<Escher::MessageTextView>
      m_boundsCells[k_numberOfBoundsCells];
  Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                   Escher::SwitchView>
      m_autoCell;
  Poincare::Range1D<FloatType> m_rangeParam;
  bool m_autoParam;

 private:
  void buttonAction() override;

  Shared::MessagePopUpController* m_confirmPopUpController;
};

using SingleRangeControllerFloatPrecision =
    SingleRangeController<FloatParameterController<float>>;
using SingleRangeControllerDoublePrecision =
    SingleRangeController<FloatParameterController<double>>;
using SingleRangeControllerExactExpressions =
    SingleRangeController<ExpressionParameterController>;

}  // namespace Shared

#endif
