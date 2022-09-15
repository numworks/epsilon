#ifndef GRAPH_AREA_BETWEEN_CURVES_PARAMETER_CONTROLLER
#define GRAPH_AREA_BETWEEN_CURVES_PARAMETER_CONTROLLER

#include <escher/selectable_list_view_controller.h>
#include <apps/shared/curve_selection_controller.h>
#include "area_between_curves_graph_controller.h"

namespace Graph {

class AreaBetweenCurvesParameterController : public Escher::SelectableListViewController<Escher::MemoizedListViewDataSource> {
public:
  static Ion::Storage::Record DerivableActiveFunctionAtIndex(int index, Ion::Storage::Record excludedRecord);
  AreaBetweenCurvesParameterController(Escher::Responder * parentResponder, AreaBetweenCurvesGraphController * areaGraphController);

  const char * title() override;
  void setRecord(Ion::Storage::Record record) { m_mainRecord = record; }

  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

  int numberOfRows() const override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  int typeAtIndex(int index) override { return 0; }
  Shared::CurveSelectionCell * reusableCell(int index, int type) override { assert(index >= 0 && index < reusableCellCount(type)); return m_cells + index; }
  int reusableCellCount(int type) override { return k_maxNumberOfRows; }

  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

private:
  constexpr static int k_maxNumberOfRows = 7;

  Ion::Storage::Record m_mainRecord;
  AreaBetweenCurvesGraphController * m_areaGraphController;
  Shared::CurveSelectionCell m_cells[k_maxNumberOfRows];
};

}

#endif
