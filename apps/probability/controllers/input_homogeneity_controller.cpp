#include "input_homogeneity_controller.h"

#include "probability/app.h"

using namespace Probability;

InputHomogeneityController::InputHomogeneityController(
    StackViewController * parent,
    ResultsHomogeneityController * homogeneityResultsController,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    HomogeneityTest * statistic) :
      InputCategoricalController(parent,
                                 homogeneityResultsController,
                                 statistic,
                                 inputEventHandlerDelegate),
      m_tableController(&m_contentView,
                        statistic,
                        &m_contentView,
                        this),
      m_contentView(this, Escher::Invocation(&InputCategoricalController::ButtonAction, this), nullptr, inputEventHandlerDelegate, this) {
  m_contentView.setTableView(&m_tableController);
}
