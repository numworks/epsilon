#ifndef DISTRIBUTIONS_PROBABILITY_PARAMETERS_CONTROLLER_H
#define DISTRIBUTIONS_PROBABILITY_PARAMETERS_CONTROLLER_H

#include <apps/shared/float_parameter_controller.h>
#include <escher/expression_view.h>
#include <escher/list_view_with_top_and_bottom_views.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/message_text_view.h>

#include "calculation_controller.h"
#include "distributions/models/distribution/distribution.h"

namespace Distributions {

class ParametersController : public Shared::FloatParameterController<double> {
 public:
  ParametersController(
      Escher::StackViewController* parentResponder,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
      Distribution* m_distribution,
      CalculationController* calculationController);
  const char* title() override;
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  Escher::View* view() override { return &m_contentView; }
  bool handleEvent(Ion::Events::Event event) override;
  void reinitCalculation();
  void viewWillAppear() override;
  TELEMETRY_ID("Parameters");
  int numberOfRows() const override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;

 private:
  constexpr static KDGlyph::Format k_format = {
      {.glyphColor = Escher::Palette::GrayDark,
       .backgroundColor = Escher::Palette::WallScreen,
       .font = KDFont::Size::Small},
      .horizontalAlignment = KDGlyph::k_alignCenter};
  int reusableParameterCellCount(int type) override;
  Escher::HighlightCell* reusableParameterCell(int index, int type) override;
  Escher::TextField* textFieldOfCellAtIndex(Escher::HighlightCell* cell,
                                            int index) override;
  void buttonAction() override;
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;

  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 const char* text,
                                 Ion::Events::Event event) override;
  bool hasUndefinedValue(const char* text, double floatValue) const override;

  constexpr static int k_maxNumberOfCells = 3;
  Escher::MessageTextView m_headerView;
  Escher::MessageTextView m_bottomView;
  Escher::ListViewWithTopAndBottomViews m_contentView;
  Escher::MenuCellWithEditableText<Escher::ExpressionView,
                                   Escher::MessageTextView>
      m_menuListCell[k_maxNumberOfCells];
  Distribution* m_distribution;
  CalculationController* m_calculationController;
};

}  // namespace Distributions

#endif
