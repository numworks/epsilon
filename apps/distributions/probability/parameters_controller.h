#ifndef DISTRIBUTIONS_PROBABILITY_PARAMETERS_CONTROLLER_H
#define DISTRIBUTIONS_PROBABILITY_PARAMETERS_CONTROLLER_H

#include <escher/expression_cell_with_editable_text_with_message.h>
#include <escher/table_view_with_frozen_header.h>
#include "distributions/models/distribution/distribution.h"
#include <apps/shared/float_parameter_controller.h>
#include "calculation_controller.h"

namespace Distributions {

class ParametersController : public Shared::FloatParameterController<double> {
public:
  ParametersController(Escher::StackViewController * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Distribution * m_distribution, CalculationController * calculationController);
  const char * title() override;
  ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastTitle; }
  Escher::View * view() override { return &m_contentView; }
  bool handleEvent(Ion::Events::Event event) override;
  void reinitCalculation();
  void viewWillAppear() override;
  void viewDidDisappear() override;
  TELEMETRY_ID("Parameters");
  int numberOfRows() const override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

private:
  Escher::HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  void buttonAction() override;
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  bool isCellEditing(Escher::HighlightCell * cell, int index) override;
  void setTextInCell(Escher::HighlightCell * cell, const char * text, int index) override;

  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) override;
  bool hasUndefinedValue(const char * text, double floatValue) const override;

  constexpr static int k_maxNumberOfCells = 3;
  Escher::TableViewWithFrozenHeader m_contentView;
  Escher::ExpressionCellWithEditableTextWithMessage m_menuListCell[k_maxNumberOfCells];
  Distribution * m_distribution;
  CalculationController * m_calculationController;
};

}

#endif
