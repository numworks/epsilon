#ifndef APPS_PROBABILITY_CONTROLLERS_RESULTS_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_RESULTS_CONTROLLER_H

#include <apps/shared/button_with_separator.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/message_table_cell_with_message.h>
#include <escher/responder.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>

#include "statistic_graph_controller.h"
#include "probability/abstract/button_delegate.h"
#include "probability/gui/page_controller.h"
#include "probability/gui/results_data_source.h"
#include "probability/models/statistic/statistic.h"

namespace Probability {

class ResultsController : public Page, public ButtonDelegate, public SelectableTableViewDataSource {
public:
  ResultsController(Escher::StackViewController * parent, Statistic * results,
                    StatisticGraphController * statisticGraphController, Escher::InputEventHandlerDelegate * handler,
                    Escher::TextFieldDelegate * textFieldDelegate);
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  void didBecomeFirstResponder() override;
  void buttonAction() override { openPage(m_statisticGraphController); }
  void openPage(ViewController * nextPage, KDColor backgroundColor = Escher::Palette::GrayMiddle,
                KDColor separatorColor = Escher::Palette::GrayMiddle,
                KDColor textColor = KDColorWhite) {
    Page::openPage(nextPage, backgroundColor, separatorColor, textColor);
  }
  Escher::View * view() override { return &m_tableView; }

protected:
  Escher::SelectableTableView m_tableView;

  ResultsDataSource m_resultsDataSource;

  StatisticGraphController * m_statisticGraphController;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_RESULTS_CONTROLLER_H */
