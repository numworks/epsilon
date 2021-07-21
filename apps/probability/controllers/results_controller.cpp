#include "results_controller.h"

#include <apps/i18n.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>

#include "probability/app.h"
#include "probability/helpers.h"
#include "probability/models/data.h"

using namespace Probability;

ResultsController::ResultsController(Escher::StackViewController * parent,
                                     Statistic * results,
                                     StatisticGraphController * statisticGraphController,
                                     Escher::InputEventHandlerDelegate * handler,
                                     Escher::TextFieldDelegate * textFieldDelegate) :
    Page(parent),
    m_tableView(this, &m_resultsDataSource, this, nullptr),
    m_contentView(&m_tableView),
    m_resultsDataSource(&m_tableView, results, this),
    m_statisticGraphController(statisticGraphController) {
}

void ResultsController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::Results);
  // TODO factor out
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(),
                         clipped(selectedRow(), m_resultsDataSource.numberOfRows() - 1));
  }
  Escher::Container::activeApp()->setFirstResponder(&m_tableView);
  m_resultsDataSource.resetMemoization();
  m_tableView.reloadData();
}

ViewController::TitlesDisplay Probability::ResultsController::titlesDisplay() {
  if (App::app()->categoricalType() == Data::CategoricalType::None) {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  return ViewController::TitlesDisplay::DisplayLastTitle;
}

Probability::ResultsController::ContentView::ContentView(Escher::SelectableTableView * table,
                                                         I18n::Message titleMessage) :
    m_spacer(Palette::WallScreen, 0, k_spacerHeight),
    m_title(KDFont::SmallFont, titleMessage, 0.5f, 0.5f, Palette::GrayDark, Palette::WallScreen),
    m_table(table) {
      m_table->setMargins(k_spacerHeight, Metric::CommonRightMargin, 0, Metric::CommonLeftMargin);
}

Escher::View * Probability::ResultsController::ContentView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  if (i == 0) {
    return &m_spacer;
  } else if (i == 1) {
    return &m_title;
  }
  return m_table;
}
