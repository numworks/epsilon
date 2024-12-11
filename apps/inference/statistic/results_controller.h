#ifndef INFERENCE_STATISTIC_RESULTS_CONTROLLER_H
#define INFERENCE_STATISTIC_RESULTS_CONTROLLER_H

#include <escher/buffer_text_view.h>
#include <escher/button_cell.h>
#include <escher/layout_view.h>
#include <escher/list_with_top_and_bottom_controller.h>
#include <escher/menu_cell.h>
#include <escher/stack_view_controller.h>

#include "inference/models/statistic/statistic.h"
#include "inference/shared/dynamic_cells_data_source.h"
#include "interval/interval_graph_controller.h"
#include "test/test_graph_controller.h"

namespace Inference {

using ResultCell = Escher::MenuCell<Escher::LayoutView, Escher::MessageTextView,
                                    Escher::FloatBufferTextView<>>;

class ResultsController
    : public Escher::ListWithTopAndBottomController,
      public DynamicCellsDataSource<ResultCell, k_maxNumberOfResultCells>,
      public DynamicCellsDataSourceDelegate<ResultCell> {
 public:
  ResultsController(Escher::Responder* parent, Statistic* statistic,
                    TestGraphController* testGraphController,
                    IntervalGraphController* intervalGraphController,
                    bool enableHeadline = true);

  static bool ButtonAction(ResultsController* controller, void* s);

  // ViewController
  ViewController::TitlesDisplay titlesDisplay() const override;
  const char* title() const override;
  void initView() override;

  // StandardMemoizedListViewDataSource
  int numberOfRows() const override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override;
  int typeAtRow(int row) const override;
  KDCoordinate separatorBeforeRow(int row) override {
    return row == m_statistic->secondResultSectionStart() ||
                   typeAtRow(row) == k_buttonCellType
               ? k_defaultRowSeparator
               : 0;
  }
  KDCoordinate nonMemoizedRowHeight(int row) override;

  // DynamicCellsDataSourceDelegate
  Escher::SelectableListView* tableView() override {
    return &m_selectableListView;
  }

 protected:
  Escher::MessageTextView m_title;
  Statistic* m_statistic;

  TestGraphController* m_testGraphController;
  IntervalGraphController* m_intervalGraphController;
  constexpr static int k_titleBufferSize = 50;
  /* m_titleBuffer is declared as mutable so that ViewController::title() can
   * remain const-qualified in the generic case. */
  mutable char m_titleBuffer[k_titleBufferSize];

 private:
  constexpr static int k_numberOfReusableCells = 6;
  constexpr static int k_resultCellType = 0;
  constexpr static int k_buttonCellType = 1;
  Escher::ButtonCell m_next;
};

}  // namespace Inference

#endif
