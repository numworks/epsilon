#include "distribution_controller.h"

#include <assert.h>

#include <new>

#include "../app.h"
#include "../images/binomial_light_icon.h"
#include "../images/chi_squared_light_icon.h"
#include "../images/exponential_light_icon.h"
#include "../images/fisher_light_icon.h"
#include "../images/geometric_light_icon.h"
#include "../images/hypergeometric_light_icon.h"
#include "../images/normal_light_icon.h"
#include "../images/poisson_light_icon.h"
#include "../images/student_light_icon.h"
#include "../images/uniform_light_icon.h"

using namespace Escher;

namespace Distributions {

DistributionController::DistributionController(
    Escher::StackViewController* parentResponder, Distribution* distribution,
    ParametersController* parametersController)
    : Escher::SelectableListViewController<Escher::SimpleListViewDataSource>(
          parentResponder),
      m_contentView(&m_selectableListView, I18n::Message::ChooseDistribution),
      m_distribution(distribution),
      m_parametersController(parametersController) {
  assert(m_distribution != nullptr);
  for (int i = 0; i < k_numberOfCells; ++i) {
    m_cells[i].subLabel()->setGlyphFormat(
        GlyphsView::FormatForWidgetType(CellWidget::Type::Label));
  }
  // Init selection
  selectRow(0);
}

void DistributionController::stackOpenPage(Escher::ViewController* nextPage) {
  selectRow(static_cast<int>(m_distribution->type()));
  ViewController::stackOpenPage(nextPage);
}

bool DistributionController::handleEvent(Ion::Events::Event event) {
  StackViewController* stack = (StackViewController*)parentResponder();
  // canBeActivatedByEvent can be called on any cell with chevron
  if (m_cells[0].canBeActivatedByEvent(event)) {
    setDistributionAccordingToIndex(selectedRow());
    stack->push(m_parametersController);
    return true;
  }
  return false;
}

HighlightCell* DistributionController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < reusableCellCount());
  return &m_cells[index];
}

constexpr I18n::Message sMessages[] = {
    I18n::Message::Binomial,    I18n::Message::Uniform,
    I18n::Message::Exponential, I18n::Message::Normal,
    I18n::Message::ChiSquare,   I18n::Message::Student,
    I18n::Message::Geometric,   I18n::Message::Hypergeometric,
    I18n::Message::Poisson,     I18n::Message::Fisher,
};

void DistributionController::fillCellForRow(HighlightCell* cell, int row) {
  DistributionCell* myCell = static_cast<DistributionCell*>(cell);
  myCell->subLabel()->setMessage(sMessages[row]);
  const Image* images[k_totalNumberOfModels] = {
      ImageStore::BinomialLightIcon,    ImageStore::UniformLightIcon,
      ImageStore::ExponentialLightIcon, ImageStore::NormalLightIcon,
      ImageStore::ChiSquaredLightIcon,  ImageStore::StudentLightIcon,
      ImageStore::GeometricLightIcon,   ImageStore::HypergeometricLightIcon,
      ImageStore::PoissonLightIcon,     ImageStore::FisherLightIcon,
  };
  myCell->label()->setImage(images[row]);
  myCell->reloadCell();
}

KDCoordinate DistributionController::defaultRowHeight() {
  DistributionCell tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, 0);
}

void DistributionController::setDistributionAccordingToIndex(int index) {
  if (Distribution::Initialize(
          m_distribution, static_cast<Poincare::Distribution::Type>(index))) {
    m_parametersController->reinitCalculation();
  }
}

}  // namespace Distributions
