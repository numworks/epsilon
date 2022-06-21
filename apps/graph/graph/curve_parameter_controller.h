#ifndef GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H

#include <escher/message_table_cell_with_chevron.h>
#include "../../shared/function_curve_parameter_controller.h"
#include "calculation_parameter_controller.h"
#include "banner_view.h"

namespace Graph {

class GraphController;

class CurveParameterController : public Shared::FunctionCurveParameterController {
public:
  CurveParameterController(Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Shared::InteractiveCurveViewRange * graphRange, BannerView * bannerView, Shared::CurveViewCursor * cursor, GraphView * graphView, GraphController * graphController);
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  int reusableCellCount() const override { return numberOfRows(); }
  Escher::HighlightCell * reusableCell(int index) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  void viewWillAppear() override;
private:
  bool shouldDisplayCalculation() const;
  int cellIndex(int visibleCellIndex) const;
  Shared::FunctionGoToParameterController * goToParameterController() override;
  Shared::FunctionGoToParameterController m_goToParameterController;
  Escher::MessageTableCellWithChevron m_calculationCell;
  CalculationParameterController m_calculationParameterController;
};

}

#endif
