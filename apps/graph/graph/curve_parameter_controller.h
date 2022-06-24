#ifndef GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H

#include "../../shared/function_curve_parameter_controller.h"
#include "calculation_parameter_controller.h"
#include "banner_view.h"

namespace Graph {

class GraphController;

class CurveParameterController : public Shared::FunctionCurveParameterController {
public:
  CurveParameterController(InputEventHandlerDelegate * inputEventHandlerDelegate, Shared::InteractiveCurveViewRange * graphRange, BannerView * bannerView, Shared::CurveViewCursor * cursor, GraphView * graphView, GraphController * graphController);
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void viewWillAppear() override;
private:
  bool shouldDisplayCalculationAndDerivative() const;
  int cellIndex(int visibleCellIndex) const;
  Shared::FunctionGoToParameterController * goToParameterController() override;
  Shared::FunctionGoToParameterController m_goToParameterController;
  GraphController * m_graphController;
  MessageTableCellWithChevron<> m_calculationCell;
  MessageTableCellWithSwitch m_derivativeCell;
  CalculationParameterController m_calculationParameterController;
};

}

#endif
