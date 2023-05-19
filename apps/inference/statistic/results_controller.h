#ifndef INFERENCE_STATISTIC_RESULTS_CONTROLLER_H
#define INFERENCE_STATISTIC_RESULTS_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <escher/button_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
#include <escher/layout_view.h>
#include <escher/list_view_with_top_and_bottom_views.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/palette.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>

#include "inference/models/statistic/statistic.h"
#include "inference/shared/dynamic_cells_data_source.h"
#include "interval/interval_graph_controller.h"
#include "test/test_graph_controller.h"

namespace Inference {

using ResultCell = Escher::MenuCell<Escher::LayoutView, Escher::MessageTextView,
                                    Escher::FloatBufferTextView<>>;

class ResultsController
    : public Escher::SelectableListViewController<
          Escher::StandardMemoizedListViewDataSource>,
      public DynamicCellsDataSource<ResultCell, k_maxNumberOfResultCells>,
      public DynamicCellsDataSourceDelegate<ResultCell> {
 public:
  ResultsController(Escher::StackViewController* parent, Statistic* statistic,
                    TestGraphController* testGraphController,
                    IntervalGraphController* intervalGraphController,
                    Escher::InputEventHandlerDelegate* handler,
                    Escher::TextFieldDelegate* textFieldDelegate);

  static bool ButtonAction(ResultsController* controller, void* s);

  // ViewController
  ViewController::TitlesDisplay titlesDisplay() override;
  const char* title() override;
  void didBecomeFirstResponder() override;
  Escher::View* view() override { return &m_contentView; }

  // StandardMemoizedListViewDataSource
  int numberOfRows() const override;
  KDCoordinate defaultColumnWidth() override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int i) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtIndex(int index) const override;
  KDCoordinate separatorBeforeRow(int index) override {
    return typeAtIndex(index) == k_buttonCellType ? k_defaultRowSeparator : 0;
  }

  // DynamicCellsDataSourceDelegate
  Escher::SelectableListView* tableView() override {
    return &m_selectableListView;
  }

 protected:
  Escher::MessageTextView m_title;
  Escher::ListViewWithTopAndBottomViews m_contentView;

  Statistic* m_statistic;

  TestGraphController* m_testGraphController;
  IntervalGraphController* m_intervalGraphController;
  constexpr static int k_titleBufferSize = 50;
  char m_titleBuffer[k_titleBufferSize];

 private:
  constexpr static int k_numberOfReusableCells = 5;
  constexpr static int k_resultCellType = 0;
  constexpr static int k_buttonCellType = 1;
  Escher::ButtonCell m_next;
};

}  // namespace Inference

#endif
