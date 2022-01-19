#ifndef PROBABILITY_CONTROLLERS_RESULTS_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_RESULTS_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/palette.h>
#include <escher/scroll_view.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>

#include <escher/button_delegate.h>
#include <escher/horizontal_or_vertical_layout.h>
#include <apps/shared/page_controller.h>
#include "probability/models/results_data_source.h"
#include "probability/models/statistic/statistic.h"
#include "statistic_graph_controller.h"

namespace Probability {

class ResultsView : public Escher::ScrollView, public Escher::SelectableTableViewDelegate {
public:
  ResultsView(Escher::SelectableTableView * table,
              Escher::TableViewDataSource * tableDataSource,
              I18n::Message titleMessage = I18n::Message::CalculatedValues);

  KDPoint tableOrigin();
  void tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t,
                                               int previousSelectedCellX,
                                               int previousSelectedCellY,
                                               bool withinTemporarySelection = false) override;

  void reload();

private:
  /* Lays out a title and a table */
  class ContentView : public Escher::VerticalLayout {
  public:
    ContentView(Escher::SelectableTableView * table, I18n::Message titleMessage);
    int numberOfSubviews() const override { return 2; }
    Escher::View * subviewAtIndex(int i) override;

  private:
    friend ResultsView;
    constexpr static int k_marginAroundTitle = 5;
    Escher::MessageTextView m_title;
    Escher::SelectableTableView * m_table;
  };

  ContentView m_contentView;
  Escher::ScrollViewDataSource m_scrollDataSource;
  Escher::TableViewDataSource * m_tableDataSource;
};

class ResultsController : public Shared::Page,
                          public Escher::ButtonDelegate,
                          public Escher::SelectableTableViewDataSource,
                          public DynamicCellsDataSourceDelegate<ExpressionCellWithBufferWithMessage> {
public:
  ResultsController(Escher::StackViewController * parent,
                    Statistic * statistic,
                    StatisticGraphController * statisticGraphController,
                    Escher::InputEventHandlerDelegate * handler,
                    Escher::TextFieldDelegate * textFieldDelegate);
  ViewController::TitlesDisplay titlesDisplay() override;
  const char * title() override;
  void didBecomeFirstResponder() override;
  bool buttonAction() override;
  Escher::View * view() override { return &m_contentView; }

  void initCell(ExpressionCellWithBufferWithMessage, void * cell, int index) override;
  Escher::SelectableTableView * tableView() override { return &m_tableView; }

protected:
  Escher::SelectableTableView m_tableView;
  ResultsView m_contentView;

  ResultsDataSource m_resultsDataSource;
  Statistic * m_statistic;

  StatisticGraphController * m_statisticGraphController;
  constexpr static int k_titleBufferSize = 50;
  char m_titleBuffer[k_titleBufferSize];

private:
  int stackTitleStyleStep() const override;
};

}  // namespace Probability

#endif /* PROBABILITY_CONTROLLERS_RESULTS_CONTROLLER_H */
