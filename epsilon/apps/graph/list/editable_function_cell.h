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

  bool isEditing() { return layoutField()->isEditing(); }
  bool templateButtonShouldBeVisible() { return layoutField()->isEmpty(); }
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
  Escher::LayoutField* layoutField() { return expressionCell()->layoutField(); }
  bool displayEllipsis() const override { return false; }

  class TemplateButtonCell : public Escher::ButtonCell {
   public:
    TemplateButtonCell(Escher::LayoutField* parentResponder,
                       Escher::Invocation invocation)
        : Escher::ButtonCell(parentResponder, I18n::Message::FunctionTemplates,
                             invocation, Style::EmbossedGray,
                             KDFont::Size::Small,
                             Escher::Palette::PurpleBright) {}
    bool handleEvent(Ion::Events::Event event) override;
  };

  constexpr static KDCoordinate k_expressionMargin = 5;
  constexpr static KDCoordinate k_templateButtonMargin = 5;
  TemplateButtonCell m_templateButton;
};

}  // namespace Graph

#endif
