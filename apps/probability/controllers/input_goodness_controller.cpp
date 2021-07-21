#include "input_goodness_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
#include <escher/responder.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/table_view_data_source.h>
#include <escher/text_field_delegate.h>

#include "probability/app.h"
#include "probability/models/data.h"
#include "probability/text_helpers.h"

using namespace Probability;

InputGoodnessController::InputGoodnessController(
    StackViewController * parent,
    ResultsController * resultsController,
    GoodnessStatistic * statistic,
    InputEventHandlerDelegate * inputEventHandlerDelegate) :
    InputCategoricalController(parent, resultsController, statistic, inputEventHandlerDelegate),
    m_tableController(&m_contentView, statistic, inputEventHandlerDelegate, &m_contentView, this) {
  m_contentView.setTableView(&m_tableController);
}

void Probability::InputGoodnessController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::InputGoodness);
  InputCategoricalController::didBecomeFirstResponder();
}
