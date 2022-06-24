#ifndef GRAPH_CALCULATION_PARAMETER_CONTROLLER_H
#define GRAPH_CALCULATION_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "preimage_parameter_controller.h"
#include "tangent_graph_controller.h"
#include "extremum_graph_controller.h"
#include "integral_graph_controller.h"
#include "intersection_graph_controller.h"
#include "root_graph_controller.h"
#include "graph_view.h"
#include "banner_view.h"
#include <apps/i18n.h>

namespace Graph {

class CalculationParameterController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  CalculationParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * range, Shared::CurveViewCursor * cursor);
  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  TELEMETRY_ID("CalculationParameter");
  int numberOfRows() const override;
  KDCoordinate rowHeight(int j) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void setRecord(Ion::Storage::Record record);
private:
  bool shouldDisplayIntersection() const;
  MessageTableCellWithChevron<> m_preimageCell;
  constexpr static int k_totalNumberOfReusableCells = 6;
  MessageTableCell<> m_cells[k_totalNumberOfReusableCells];
  SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
  PreimageParameterController m_preimageParameterController;
  PreimageGraphController m_preimageGraphController;
  TangentGraphController m_tangentGraphController;
  IntegralGraphController m_integralGraphController;
  MinimumGraphController m_minimumGraphController;
  MaximumGraphController m_maximumGraphController;
  RootGraphController m_rootGraphController;
  IntersectionGraphController m_intersectionGraphController;
};

}

#endif

