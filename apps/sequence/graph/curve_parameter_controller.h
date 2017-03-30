#ifndef SEQUENCE_CURVE_PARAMETER_CONTROLLER_H
#define SEQUENCE_CURVE_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../../shared/function_curve_parameter_controller.h"
#include "go_to_parameter_controller.h"

namespace Sequence {

class GraphController;

class CurveParameterController : public Shared::FunctionCurveParameterController {
public:
  CurveParameterController(GraphController * graphController, Shared::InteractiveCurveViewRange * graphRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  constexpr static int k_totalNumberOfCells = 2;
  GoToParameterController * goToParameterController() override;
  GoToParameterController m_goToParameterController;
  MessageTableCell m_sumCell;
  GraphController * m_graphController;
};

}

#endif
