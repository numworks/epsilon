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
#include "probability/models/data.h"
#include "probability/models/distribution/binomial_distribution.h"
#include "probability/models/distribution/chi_squared_distribution.h"
#include "probability/models/distribution/exponential_distribution.h"
#include "probability/models/distribution/fisher_distribution.h"
#include "probability/models/distribution/geometric_distribution.h"
#include "probability/models/distribution/normal_distribution.h"
#include "probability/models/distribution/poisson_distribution.h"
#include "probability/models/distribution/student_distribution.h"
#include "probability/models/distribution/uniform_distribution.h"

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
  App::app()->setPage(Data::Page::Distribution);
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
    stack->push(m_parametersController, Escher::StackViewController::Style::GrayGradation, 0);
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
  m_distribution->~Distribution();
  switch (index) {
    case 0:
      new (m_distribution) BinomialDistribution();
      break;
    case 1:
      new (m_distribution) UniformDistribution();
      break;
    case 2:
      new (m_distribution) ExponentialDistribution();
      break;
    case 3:
      new (m_distribution) NormalDistribution();
      break;
    case 4:
      new (m_distribution) ChiSquaredDistribution();
      break;
    case 5:
      new (m_distribution) StudentDistribution();
      break;
    case 6:
      new (m_distribution) GeometricDistribution();
      break;
    case 7:
      new (m_distribution) PoissonDistribution();
      break;
    case 8:
      new (m_distribution) FisherDistribution();
      break;
    default:
      return;
  }
  m_parametersController->reinitCalculation();
}

}  // namespace Probability
