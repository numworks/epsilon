#include "distribution_controller.h"

#include <assert.h>

#include <new>

#include "../images/binomial_icon.h"
#include "../images/chi_squared_icon.h"
#include "../images/exponential_icon.h"
#include "../images/fisher_icon.h"
#include "../images/geometric_icon.h"
#include "../images/normal_icon.h"
#include "../images/poisson_icon.h"
#include "../images/student_icon.h"
#include "../images/uniform_icon.h"
#include "inference/app.h"
#include "inference/models/models_buffer.h"
#include "inference/models/probability/distribution/binomial_distribution.h"
#include "inference/models/probability/distribution/chi_squared_distribution.h"
#include "inference/models/probability/distribution/exponential_distribution.h"
#include "inference/models/probability/distribution/fisher_distribution.h"
#include "inference/models/probability/distribution/geometric_distribution.h"
#include "inference/models/probability/distribution/normal_distribution.h"
#include "inference/models/probability/distribution/poisson_distribution.h"
#include "inference/models/probability/distribution/student_distribution.h"
#include "inference/models/probability/distribution/uniform_distribution.h"

using namespace Escher;

namespace Inference {

DistributionController::DistributionController(Escher::StackViewController * parentResponder,
                                               Distribution * distribution,
                                               ParametersController * parametersController) :
      Escher::SelectableListViewController<Escher::SimpleListViewDataSource>(parentResponder),
      m_distribution(distribution),
      m_parametersController(parametersController) {
  assert(m_distribution != nullptr);

  // Init selection
  selectRow(0);
}

void DistributionController::stackOpenPage(Escher::ViewController * nextPage) {
  selectRow(static_cast<int>(m_distribution->type()));
  ViewController::stackOpenPage(nextPage);
}

void DistributionController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool DistributionController::handleEvent(Ion::Events::Event event) {
  StackViewController * stack = (StackViewController *)parentResponder();
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    setDistributionAccordingToIndex(selectedRow());
    stack->push(m_parametersController);
    return true;
  } else if (event == Ion::Events::Left) {
    stack->pop();
    return true;
  }
  return false;
}

HighlightCell * DistributionController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < reusableCellCount());
  return &m_cells[index];
}

constexpr I18n::Message sMessages[] = {I18n::Message::Binomial,
                                       I18n::Message::Uniforme,
                                       I18n::Message::Exponential,
                                       I18n::Message::Normal,
                                       I18n::Message::ChiSquare,
                                       I18n::Message::Student,
                                       I18n::Message::Geometric,
                                       I18n::Message::Poisson,
                                       I18n::Message::Fisher};

void DistributionController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  DistributionCell * myCell = static_cast<DistributionCell *>(cell);
  myCell->setLabel(sMessages[index]);
  const Image * images[k_totalNumberOfModels] = {ImageStore::BinomialIcon,
                                                 ImageStore::UniformIcon,
                                                 ImageStore::ExponentialIcon,
                                                 ImageStore::NormalIcon,
                                                 ImageStore::ChiSquaredIcon,
                                                 ImageStore::StudentIcon,
                                                 ImageStore::GeometricIcon,
                                                 ImageStore::PoissonIcon,
                                                 ImageStore::FisherIcon};
  myCell->setImage(images[index]);
  myCell->reloadCell();
}

void DistributionController::setDistributionAccordingToIndex(int index) {
  if (Distribution::Initialize(m_distribution, static_cast<Poincare::Distribution::Type>(index))) {
    m_parametersController->reinitCalculation();
  }
}

}  // namespace Inference
