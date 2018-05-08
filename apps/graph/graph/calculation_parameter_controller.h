#ifndef GRAPH_CALCULATION_PARAMETER_CONTROLLER_H
#define GRAPH_CALCULATION_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../cartesian_function.h"
#include "tangent_graph_controller.h"
#include "extremum_graph_controller.h"
#include "integral_graph_controller.h"
#include "intersection_graph_controller.h"
#include "root_graph_controller.h"
#include "graph_view.h"
#include "banner_view.h"
#include "../../i18n.h"

namespace Graph {

class CalculationParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  CalculationParameterController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * range, Shared::CurveViewCursor * cursor, CartesianFunctionStore * functionStore);
  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void setFunction(CartesianFunction * function);
private:
  constexpr static int k_totalNumberOfCells = 6;
  MessageTableCell m_cells[k_totalNumberOfCells];
  SelectableTableView m_selectableTableView;
  CartesianFunction * m_function;
  TangentGraphController m_tangentGraphController;
  IntegralGraphController m_integralGraphController;
  MinimumGraphController m_minimumGraphController;
  MaximumGraphController m_maximumGraphController;
  RootGraphController m_rootGraphController;
  IntersectionGraphController m_intersectionGraphController;
};

}

#endif

