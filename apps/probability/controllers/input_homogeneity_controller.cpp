#include "input_homogeneity_controller.h"

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <kandinsky/color.h>
#include <kandinsky/font.h>
#include <string.h>

#include "probability/app.h"
#include "probability/models/data.h"
#include "probability/text_helpers.h"

using namespace Probability;

InputHomogeneityController::InputHomogeneityController(
    StackViewController * parent,
    ResultsHomogeneityController * homogeneityResultsController,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    HomogeneityStatistic * statistic) :
    InputCategoricalController(parent,
                               homogeneityResultsController,
                               statistic,
                               inputEventHandlerDelegate),
    m_tableController(&m_contentView, statistic, inputEventHandlerDelegate, &m_contentView, this) {
  m_contentView.setTableView(&m_tableController);
}

void Probability::InputHomogeneityController::didBecomeFirstResponder() {
  App::app()->setPage(Data::Page::InputHomogeneity);
  InputCategoricalController::didBecomeFirstResponder();
}
