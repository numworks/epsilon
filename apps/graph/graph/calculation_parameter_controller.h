#ifndef GRAPH_CALCULATION_PARAMETER_CONTROLLER_H
#define GRAPH_CALCULATION_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../cartesian_function_store.h"
#include "tangent_graph_controller.h"
#include "extremum_graph_controller.h"
#include "integral_graph_controller.h"
#include "intersection_graph_controller.h"
#include "root_graph_controller.h"
#include "graph_view.h"
#include "banner_view.h"
#include <apps/i18n.h>

namespace Graph {

class CalculationParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  CalculationParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * range, Shared::CurveViewCursor * cursor);
  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void setRecord(Ion::Storage::Record record);
private:
  constexpr static int k_totalNumberOfCells = 6;
  MessageTableCell m_cells[k_totalNumberOfCells];
  SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
  TangentGraphController m_tangentGraphController;
  IntegralGraphController m_integralGraphController;
  MinimumGraphController m_minimumGraphController;
  MaximumGraphController m_maximumGraphController;
  RootGraphController m_rootGraphController;
  IntersectionGraphController m_intersectionGraphController;
};

}

#endif

