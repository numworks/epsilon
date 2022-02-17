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
#include "probability/app.h"
#include "probability/models/models_buffer.h"
#include "probability/models/probability/distribution/binomial_distribution.h"
#include "probability/models/probability/distribution/chi_squared_distribution.h"
#include "probability/models/probability/distribution/exponential_distribution.h"
#include "probability/models/probability/distribution/fisher_distribution.h"
#include "probability/models/probability/distribution/geometric_distribution.h"
#include "probability/models/probability/distribution/normal_distribution.h"
#include "probability/models/probability/distribution/poisson_distribution.h"
#include "probability/models/probability/distribution/student_distribution.h"
#include "probability/models/probability/distribution/uniform_distribution.h"

using namespace Escher;

namespace Probability {

DistributionController::DistributionController(Escher::StackViewController * parentResponder,
                                               Distribution * distribution,
                                               ParametersController * parametersController) :
      Escher::SelectableListViewController(parentResponder),
      m_distribution(distribution),
      m_parametersController(parametersController) {
  assert(m_distribution != nullptr);
}

void Probability::DistributionController::viewWillAppear() {
  ViewController::viewWillAppear();
  selectRow((int)m_distribution->type());
}

void Probability::DistributionController::didBecomeFirstResponder() {
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool Probability::DistributionController::handleEvent(Ion::Events::Event event) {
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

KDCoordinate Probability::DistributionController::nonMemoizedRowHeight(int j) {
  DistributionCell tempCell;
  return heightForCellAtIndex(&tempCell, j);
}

HighlightCell * Probability::DistributionController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < reusableCellCount(type));
  return &m_cells[index];
}

constexpr I18n::Message sMessages[] = {I18n::Message::Binomial,
                                       I18n::Message::Uniforme,
                                       I18n::Message::Exponential,
                                       I18n::Message::Normal,
                                       I18n::Message::ChiSquared,
                                       I18n::Message::Student,
                                       I18n::Message::Geometric,
                                       I18n::Message::Poisson,
                                       I18n::Message::Fisher};

void Probability::DistributionController::willDisplayCellForIndex(HighlightCell * cell, int index) {
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

void Probability::DistributionController::setDistributionAccordingToIndex(int index) {
  if ((int)m_distribution->type() == index) {
    return;
  }
  Distribution::Initialize(m_distribution, static_cast<Distribution::Type>(index));
  m_parametersController->reinitCalculation();
}

}  // namespace Probability
