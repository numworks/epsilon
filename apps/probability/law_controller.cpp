#include "law_controller.h"
#include <assert.h>
#include "app.h"
#include "binomial_icon.h"
#include "exponential_icon.h"
#include "normal_icon.h"
#include "poisson_icon.h"
#include "uniform_icon.h"
#include "focused_binomial_icon.h"
#include "focused_exponential_icon.h"
#include "focused_normal_icon.h"
#include "focused_poisson_icon.h"
#include "focused_uniform_icon.h"

namespace Probability {

static const char * sMessages[] = {
  "Binomiale",
  "Uniforme",
  "Exponentielle",
  "Normale",
  "Poisson"
};

LawController::LawController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_parametersController(ParametersController(nullptr, &m_law))
{
  m_messages = sMessages;
}

View * LawController::view() {
  return &m_selectableTableView;
}

const char * LawController::title() const {
  switch (m_law.type()) {
    case Law::Type::Binomial:
      return "Loi binomiale";
    case Law::Type::Uniform:
      return "Loi uniforme";
    case Law::Type::Exponential:
      return "Loi exponentielle";
    case Law::Type::Normal:
      return "Loi normale";
    case Law::Type::Poisson:
      return "Loi Poisson";
    default:
      return "Choisir le type de Loi";
  }
}

void Probability::LawController::didBecomeFirstResponder() {
  m_law.setType(Law::Type::NoType);
  StackViewController * stack = (StackViewController *)parentResponder();
  stack->updateTitle();
  if (m_selectableTableView.selectedRow() == -1) {
    m_selectableTableView.selectCellAtLocation(0, 0);
  } else {
    m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool Probability::LawController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    StackViewController * stack = (StackViewController *)parentResponder();
    m_law.setType(typeAtIndex(m_selectableTableView.selectedRow()));
    stack->updateTitle();
    stack->push(&m_parametersController);
    return true;
  }
  return false;
}

int Probability::LawController::numberOfRows() {
  return k_totalNumberOfModels;
};

TableViewCell * Probability::LawController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfModels);
  return &m_cells[index];
}

int Probability::LawController::reusableCellCount() {
  return k_totalNumberOfModels;
}

void Probability::LawController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  Cell * myCell = (Cell *)cell;
  myCell->setLabel(m_messages[index]);
  switch (typeAtIndex(index)) {
    case Law::Type::Binomial:
      myCell->setImage(ImageStore::BinomialIcon, ImageStore::FocusedBinomialIcon);
      break;
    case Law::Type::Uniform:
      myCell->setImage(ImageStore::UniformIcon, ImageStore::FocusedUniformIcon);
      break;
    case Law::Type::Exponential:
      myCell->setImage(ImageStore::ExponentialIcon, ImageStore::FocusedExponentialIcon);
      break;
    case Law::Type::Normal:
      myCell->setImage(ImageStore::NormalIcon, ImageStore::FocusedNormalIcon);
      break;
    case Law::Type::Poisson:
      myCell->setImage(ImageStore::PoissonIcon, ImageStore::FocusedPoissonIcon);
      break;
    default:
      break;
  }
  myCell->reloadCell();
}

KDCoordinate Probability::LawController::cellHeight() {
  return 35;
}

Law::Type Probability::LawController::typeAtIndex(int index) {
  switch (index) {
    case 0:
     return Law::Type::Binomial;
    case 1:
     return Law::Type::Uniform;
    case 2:
     return Law::Type::Exponential;
    case 3:
     return Law::Type::Normal;
    case 4:
     return Law::Type::Poisson;
    default:
     return Law::Type::NoType;
  }
}

}
