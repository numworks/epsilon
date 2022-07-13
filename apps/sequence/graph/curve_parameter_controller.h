#ifndef SEQUENCE_CURVE_PARAMETER_CONTROLLER_H
#define SEQUENCE_CURVE_PARAMETER_CONTROLLER_H

#include "../../shared/function_curve_parameter_controller.h"
#include "go_to_parameter_controller.h"
#include "cobweb_controller.h"

namespace Sequence {

class GraphController;

class CurveParameterController : public Shared::FunctionCurveParameterController {
public:
  CurveParameterController(Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, GraphController * graphController, CobwebController * cobwebController, Shared::InteractiveCurveViewRange * graphRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  bool hasCobweb() const;
  int numberOfRows() const override;
  int reusableCellCount() const override { return k_totalNumberOfCells - !hasCobweb(); }
  Escher::HighlightCell * reusableCell(int index) override;
private:
  constexpr static int k_totalNumberOfCells = 3;
  GoToParameterController * goToParameterController() override;
  GoToParameterController m_goToParameterController;
  Escher::MessageTableCell m_sumCell;
  Escher::MessageTableCell m_cobwebCell;
  CobwebController * m_cobwebController;
  GraphController * m_graphController;
};

}

#endif
