#ifndef GRAPH_AREA_BETWEEN_CURVES_PARAMETER_CONTROLLER
#define GRAPH_AREA_BETWEEN_CURVES_PARAMETER_CONTROLLER

#include <apps/shared/curve_selection_controller.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/table_view_with_top_and_bottom_views.h>

#include "area_between_curves_graph_controller.h"

namespace Graph {

class AreaBetweenCurvesParameterController
    : public Escher::SelectableListViewController<
          Escher::MemoizedListViewDataSource> {
 public:
  static Ion::Storage::Record DerivableActiveFunctionAtIndex(
      int index, Ion::Storage::Record excludedRecord);
  AreaBetweenCurvesParameterController(
      Escher::Responder* parentResponder,
      AreaBetweenCurvesGraphController* areaGraphController);

  const char* title() override;
  void setRecord(Ion::Storage::Record record) { m_mainRecord = record; }

  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  Escher::ViewController::TitlesDisplay titlesDisplay() override {
    return Escher::ViewController::TitlesDisplay::DisplayLastThreeTitles;
  }

  int numberOfRows() const override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  int typeAtIndex(int index) const override { return 0; }
  Shared::CurveSelectionCell* reusableCell(int index, int type) override {
    assert(index >= 0 && index < reusableCellCount(type));
    return m_cells + index;
  }
  int reusableCellCount(int type) override {
    return k_maxNumberOfDisplayableRows;
  }

  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;
  Escher::View* view() override { return &m_contentView; }

 private:
  constexpr static int k_maxNumberOfDisplayableRows =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          Escher::TableCell::k_minimalLargeFontCellHeight,
          Escher::Metric::TabHeight + 3 * Escher::Metric::StackTitleHeight);
  // There should always be 3 titles displayed.

  Ion::Storage::Record m_mainRecord;
  AreaBetweenCurvesGraphController* m_areaGraphController;
  Escher::TableViewWithTopAndBottomViews m_contentView;
  Escher::MessageTextView m_topView;
  Shared::CurveSelectionCell m_cells[k_maxNumberOfDisplayableRows];
};

}  // namespace Graph

#endif
