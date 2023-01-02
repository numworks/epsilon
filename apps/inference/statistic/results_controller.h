#ifndef INFERENCE_STATISTIC_RESULTS_CONTROLLER_H
#define INFERENCE_STATISTIC_RESULTS_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/palette.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>
#include <escher/table_view_with_top_and_bottom_views.h>
#include <escher/view_controller.h>

#include "inference/statistic/results_data_source.h"
#include "inference/models/statistic/statistic.h"
#include "interval/interval_graph_controller.h"
#include "test/test_graph_controller.h"

namespace Inference {

class ResultsController : public Escher::ViewController,
                          public Escher::SelectableTableViewDataSource,
                          public DynamicCellsDataSourceDelegate<ExpressionCellWithBufferWithMessage> {
public:
  ResultsController(Escher::StackViewController * parent,
                    Statistic * statistic,
                    TestGraphController * testGraphController,
                    IntervalGraphController * intervalGraphController,
                    Escher::InputEventHandlerDelegate * handler,
                    Escher::TextFieldDelegate * textFieldDelegate);
  ViewController::TitlesDisplay titlesDisplay() override;
  const char * title() override;
  void didBecomeFirstResponder() override;
  static bool ButtonAction(ResultsController * controller, void * s);
  Escher::View * view() override { return &m_contentView; }

  void initCell(ExpressionCellWithBufferWithMessage, void * cell, int index) override;
  Escher::SelectableTableView * tableView() override { return &m_tableView; }

protected:
  Escher::SelectableTableView m_tableView;
  Escher::MessageTextView m_title;
  Escher::TableViewWithTopAndBottomViews m_contentView;

  ResultsDataSource m_resultsDataSource;
  Statistic * m_statistic;

  TestGraphController * m_testGraphController;
  IntervalGraphController * m_intervalGraphController;
  constexpr static int k_titleBufferSize = 50;
  char m_titleBuffer[k_titleBufferSize];
};

}

#endif
