#ifndef ESCHER_EDITABLE_FUNCTION_CELL_H
#define ESCHER_EDITABLE_FUNCTION_CELL_H

#include <escher/button_cell.h>
#include <escher/editable_expression_model_cell.h>
#include <escher/stack_view_controller.h>

#include "function_cell.h"

namespace Graph {

class EditableFunctionCell
    : public TemplatedFunctionCell<Shared::WithEditableExpressionCell> {
 public:
  using State = HighlightCell::State;

  EditableFunctionCell(Escher::Responder* parentResponder,
                       Escher::LayoutFieldDelegate* layoutFieldDelegate,
                       Escher::StackViewController* modelsStackController);

  void updateSubviewsBackgroundAfterChangingState() override {
    m_expressionBackground = backgroundColor();
  }

  bool isEditing() { return expressionCell()->layoutField()->isEditing(); }

  bool buttonShouldBeVisible() {
    return expressionCell()->layoutField()->isEmpty();
  }

  Escher::ButtonCell* buttonCell() { return &m_buttonCell; }

  void setTemplateButtonState(State state);
  State templateButtonState() const { return m_buttonCell.getState(); }

 private:
  int numberOfSubviews() const override {
    return AbstractFunctionCell::numberOfSubviews() + 1;
  }
  void layoutSubviews(bool force = false) override;
  Escher::View* subviewAtIndex(int index) override;

  class ButtonCell : public Escher::ButtonCell {
   public:
    ButtonCell(Escher::LayoutField* parentResponder,
               Escher::Invocation invocation)
        : Escher::ButtonCell(parentResponder, I18n::Message::UseFunctionModel,
                             invocation, Escher::Palette::WallScreen, 0,
                             KDFont::Size::Small) {}
    bool handleEvent(Ion::Events::Event event) override;

    using HighlightCell::getState;
  };

  static constexpr KDCoordinate k_expressionMargin = 5;
  static constexpr KDCoordinate k_templateButtonMargin = 5;
  ButtonCell m_buttonCell;
};

}  // namespace Graph

#endif
