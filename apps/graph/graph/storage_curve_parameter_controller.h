#ifndef GRAPH_GRAPH_STORAGE_CURVE_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_STORAGE_CURVE_PARAMETER_CONTROLLER_H

#include "../../shared/function_curve_parameter_controller.h"
#include "calculation_parameter_controller.h"
#include "banner_view.h"
#include "../storage_cartesian_function_store.h"
#include "storage_graph_view.h"

namespace Graph {

class GraphController;

class StorageCurveParameterController : public Shared::FunctionCurveParameterController {
public:
  StorageCurveParameterController(Shared::InteractiveCurveViewRange * graphRange, BannerView * bannerView, Shared::CurveViewCursor * cursor, StorageGraphView * graphView, GraphController * graphController, StorageCartesianFunctionStore * functionStore);
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  Shared::FunctionGoToParameterController * goToParameterController() override;
  Shared::FunctionGoToParameterController m_goToParameterController;
  GraphController * m_graphController;
  constexpr static int k_totalNumberOfCells = 3;
  MessageTableCellWithChevron m_calculationCell;
  MessageTableCellWithSwitch m_derivativeCell;
  CalculationParameterController m_calculationParameterController;
};

}

#endif
