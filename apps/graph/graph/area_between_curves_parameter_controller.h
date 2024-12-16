#ifndef GRAPH_AREA_BETWEEN_CURVES_PARAMETER_CONTROLLER
#define GRAPH_AREA_BETWEEN_CURVES_PARAMETER_CONTROLLER

#include <apps/shared/curve_selection_controller.h>
#include <escher/list_with_top_and_bottom_controller.h>

#include "area_between_curves_graph_controller.h"

namespace Graph {

class AreaBetweenCurvesParameterController
    : public Escher::ListWithTopAndBottomController {
 public:
  static Ion::Storage::Record AreaCompatibleFunctionAtIndex(
      int index, Ion::Storage::Record excludedRecord);
  AreaBetweenCurvesParameterController(
      Escher::Responder* parentResponder,
      AreaBetweenCurvesGraphController* areaGraphController);

  const char* title() const override;
  void setRecord(Ion::Storage::Record record) { m_mainRecord = record; }

  bool handleEvent(Ion::Events::Event event) override;
  Escher::ViewController::TitlesDisplay titlesDisplay() const override {
    return Escher::ViewController::TitlesDisplay::DisplayLastThreeTitles;
  }

  int numberOfRows() const override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  int typeAtRow(int row) const override { return 0; }
  Shared::CurveSelectionCell* reusableCell(int index, int type) override {
    assert(index >= 0 && index < reusableCellCount(type));
    return m_cells + index;
  }
  int reusableCellCount(int type) const override {
    return k_maxNumberOfDisplayableRows;
  }

  void fillCellForRow(Escher::HighlightCell* cell, int row) override;

 private:
  constexpr static int k_maxNumberOfDisplayableRows =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          Escher::AbstractMenuCell::k_minimalLargeFontCellHeight,
          Escher::Metric::TabHeight + 3 * Escher::Metric::StackTitleHeight);
  // There should always be 3 titles displayed.

  Ion::Storage::Record m_mainRecord;
  AreaBetweenCurvesGraphController* m_areaGraphController;
  Escher::MessageTextView m_topView;
  Shared::CurveSelectionCell m_cells[k_maxNumberOfDisplayableRows];
};

}  // namespace Graph

#endif
