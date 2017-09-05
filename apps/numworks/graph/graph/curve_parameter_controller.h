#ifndef GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H

#include "apps/shared/function_curve_parameter_controller.h"
#include "banner_view.h"

namespace Graph {

class CurveParameterController : public Shared::FunctionCurveParameterController {
public:
  CurveParameterController(Shared::InteractiveCurveViewRange * graphRange, BannerView * bannerView, Shared::CurveViewCursor * cursor);
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  Shared::FunctionGoToParameterController * goToParameterController() override;
  Shared::FunctionGoToParameterController m_goToParameterController;
  BannerView * m_bannerView;
#if FUNCTION_CALCULATE_MENU
  constexpr static int k_totalNumberOfCells = 3;
  MessageTableCellWithChevron m_calculationCell;
#else
  constexpr static int k_totalNumberOfCells = 2;
#endif
  MessageTableCellWithSwitch m_derivativeCell;
};

}

#endif
