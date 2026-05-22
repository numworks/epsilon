#include "distribution_controller.h"

#include <apps/global_preferences.h>
#include <assert.h>

#include <new>

#include "../app.h"
#include "binomial_dark_icon.h"
#include "binomial_light_icon.h"
#include "chi_squared_dark_icon.h"
#include "chi_squared_light_icon.h"
#include "exponential_dark_icon.h"
#include "exponential_light_icon.h"
#include "fisher_dark_icon.h"
#include "fisher_light_icon.h"
#include "geometric_dark_icon.h"
#include "geometric_light_icon.h"
#include "hypergeometric_dark_icon.h"
#include "hypergeometric_light_icon.h"
#include "normal_dark_icon.h"
#include "normal_light_icon.h"
#include "poisson_dark_icon.h"
#include "poisson_light_icon.h"
#include "student_dark_icon.h"
#include "student_light_icon.h"
#include "uniform_dark_icon.h"
#include "uniform_light_icon.h"

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
  const bool dark = SharedGlobalPrefs().theme() == Escher::Theme::Dark;
  const Image* images[k_totalNumberOfModels] = {
      dark ? ImageStore::BinomialDarkIcon : ImageStore::BinomialLightIcon,
      dark ? ImageStore::UniformDarkIcon : ImageStore::UniformLightIcon,
      dark ? ImageStore::ExponentialDarkIcon : ImageStore::ExponentialLightIcon,
      dark ? ImageStore::NormalDarkIcon : ImageStore::NormalLightIcon,
      dark ? ImageStore::ChiSquaredDarkIcon : ImageStore::ChiSquaredLightIcon,
      dark ? ImageStore::StudentDarkIcon : ImageStore::StudentLightIcon,
      dark ? ImageStore::GeometricDarkIcon : ImageStore::GeometricLightIcon,
      dark ? ImageStore::HypergeometricDarkIcon
           : ImageStore::HypergeometricLightIcon,
      dark ? ImageStore::PoissonDarkIcon : ImageStore::PoissonLightIcon,
      dark ? ImageStore::FisherDarkIcon : ImageStore::FisherLightIcon,
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
