#include "results_controller.h"

#include <apps/i18n.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>

#include "probability/app.h"
#include "probability/models/data.h"

using namespace Probability;

ResultsController::ResultsController(Escher::StackViewController * parent,
                                     Statistic * results,
                                     GraphController * graphController,
                                     Escher::InputEventHandlerDelegate * handler,
                                     Escher::TextFieldDelegate * textFieldDelegate) :
    Page(parent),
    m_tableView(this, &m_resultsDataSource, this, nullptr),
    m_resultsDataSource(&m_tableView, results, this),
    m_graphController(graphController) {}

void ResultsController::didBecomeFirstResponder() {
  Probability::App::app()->snapshot()->navigation()->setPage(Data::Page::Results);
  // TODO factor out
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  Escher::Container::activeApp()->setFirstResponder(&m_tableView);
  m_resultsDataSource.resetMemoization();
  m_tableView.reloadData();
}