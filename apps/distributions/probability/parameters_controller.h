#ifndef DISTRIBUTIONS_PROBABILITY_PARAMETERS_CONTROLLER_H
#define DISTRIBUTIONS_PROBABILITY_PARAMETERS_CONTROLLER_H

#include <apps/shared/float_parameter_controller.h>
#include <escher/expression_cell_with_editable_text_with_message.h>
#include <escher/list_view_with_top_and_bottom_views.h>

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
  Escher::HighlightCell* reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  void buttonAction() override;
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  bool isCellEditing(Escher::HighlightCell* cell, int index) override;
  void setTextInCell(Escher::HighlightCell* cell, const char* text,
                     int index) override;

  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 const char* text,
                                 Ion::Events::Event event) override;
  bool hasUndefinedValue(const char* text, double floatValue) const override;

  constexpr static int k_maxNumberOfCells = 3;
  Escher::MessageTextView m_headerView;
  Escher::MessageTextView m_bottomView;
  Escher::ListViewWithTopAndBottomViews m_contentView;
  Escher::ExpressionCellWithEditableTextWithMessage
      m_menuListCell[k_maxNumberOfCells];
  Distribution* m_distribution;
  CalculationController* m_calculationController;
};

}  // namespace Distributions

#endif
