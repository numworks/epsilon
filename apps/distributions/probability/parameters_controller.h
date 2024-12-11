#ifndef DISTRIBUTIONS_PROBABILITY_PARAMETERS_CONTROLLER_H
#define DISTRIBUTIONS_PROBABILITY_PARAMETERS_CONTROLLER_H

#include <apps/shared/float_parameter_controller.h>
#include <escher/layout_view.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/message_text_view.h>

#include "calculation_controller.h"
#include "distributions/models/distribution/distribution.h"

namespace Distributions {

class ParametersController : public Shared::FloatParameterController<double> {
 public:
  ParametersController(Escher::StackViewController* parentResponder,
                       Distribution* m_distribution,
                       CalculationController* calculationController);
  const char* title() const override;
  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  bool handleEvent(Ion::Events::Event event) override;
  void reinitCalculation();
  void viewWillAppear() override;
  int numberOfRows() const override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  KDCoordinate nonMemoizedRowHeight(int row) override;

 private:
  int reusableParameterCellCount(int type) const override;
  Escher::HighlightCell* reusableParameterCell(int index, int type) override;
  Escher::TextField* textFieldOfCellAtIndex(Escher::HighlightCell* cell,
                                            int index) override;
  void buttonAction() override;
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;

  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;
  bool hasUndefinedValue(const char* text, double floatValue,
                         int row) const override;

  constexpr static int k_maxNumberOfCells = 3;
  Escher::MessageTextView m_headerView;
  Escher::MessageTextView m_bottomView;
  Escher::MenuCellWithEditableText<Escher::LayoutView, Escher::MessageTextView>
      m_menuListCell[k_maxNumberOfCells];
  Distribution* m_distribution;
  CalculationController* m_calculationController;
};

}  // namespace Distributions

#endif
