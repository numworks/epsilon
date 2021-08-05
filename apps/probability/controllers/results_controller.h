#ifndef APPS_PROBABILITY_CONTROLLERS_RESULTS_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_RESULTS_CONTROLLER_H

#include <apps/i18n.h>
#include <apps/shared/button_with_separator.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/message_table_cell_with_message.h>
#include <escher/responder.h>
#include <escher/scroll_view.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>

#include "probability/abstract/button_delegate.h"
#include "probability/gui/horizontal_or_vertical_layout.h"
#include "probability/gui/page_controller.h"
#include "probability/models/results_data_source.h"
#include "probability/gui/spacer_view.h"
#include "probability/models/statistic/statistic.h"
#include "statistic_graph_controller.h"

namespace Probability {

class ResultsView : public ScrollView, public Escher::SelectableTableViewDelegate {
public:
  ResultsView(Escher::SelectableTableView * table,
              Escher::TableViewDataSource * tableDataSource,
              I18n::Message titleMessage = I18n::Message::CalculatedValues);

  KDPoint tableOrigin();
  void tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t,
                                               int previousSelectedCellX,
                                               int previousSelectedCellY,
                                               bool withinTemporarySelection = false) override;

  void reload();

private:
  /* Lays out a SpacerView, a title and a table */
  class ContentView : public VerticalLayout {
  public:
    ContentView(Escher::SelectableTableView * table, I18n::Message titleMessage);
    int numberOfSubviews() const override { return 2; }
    Escher::View * subviewAtIndex(int i) override;
    void relayout();

  private:
    friend ResultsView;
    constexpr static int k_marginAroundTitle = 5;
    Escher::MessageTextView m_title;
    Escher::SelectableTableView * m_table;
  };

  ContentView m_contentView;
  ScrollViewDataSource m_scrollDataSource;
  Escher::TableViewDataSource * m_tableDataSource;
};

class ResultsController : public Page, public ButtonDelegate, public SelectableTableViewDataSource {
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
  void openPage(ViewController * nextPage,
                KDColor backgroundColor = s_titleColor,
                KDColor separatorColor = s_titleColor,
                KDColor textColor = KDColorWhite) {
    Page::openPage(nextPage, backgroundColor, separatorColor, textColor);
  }
  Escher::View * view() override { return &m_contentView; }

protected:
  Escher::SelectableTableView m_tableView;
  ResultsView m_contentView;

  ResultsDataSource m_resultsDataSource;
  Statistic * m_statistic;

  StatisticGraphController * m_statisticGraphController;
  constexpr static int k_titleBufferSize = 40;
  char m_titleBuffer[k_titleBufferSize];

private:
  static KDColor s_titleColor;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_RESULTS_CONTROLLER_H */
