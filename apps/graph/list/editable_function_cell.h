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

  bool templateButtonShouldBeVisible() {
    return expressionCell()->layoutField()->isEmpty();
  }

  void setTemplateButtonVisible(bool visible);
  void setTemplateButtonHighlighted(bool highlighted);

  bool isTemplateButtonVisible() const { return m_templateButton.isVisible(); }
  bool isTemplateButtonHighlighted() const {
    return m_templateButton.isHighlighted();
  }

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
                             invocation, Style::EmbossedGray,
                             KDFont::Size::Small,
                             Escher::Palette::PurpleBright) {}
    bool handleEvent(Ion::Events::Event event) override;
  };

  static constexpr KDCoordinate k_expressionMargin = 5;
  static constexpr KDCoordinate k_templateButtonMargin = 5;
  ButtonCell m_templateButton;
};

}  // namespace Graph

#endif
