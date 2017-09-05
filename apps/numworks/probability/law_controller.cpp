#include "law_controller.h"
#include <assert.h>
#include <new>
#include "app.h"
#include "law/binomial_law.h"
#include "law/exponential_law.h"
#include "law/normal_law.h"
#include "law/poisson_law.h"
#include "law/uniform_law.h"
#include "images/binomial_icon.h"
#include "images/exponential_icon.h"
#include "images/normal_icon.h"
#include "images/poisson_icon.h"
#include "images/uniform_icon.h"
#include "images/focused_binomial_icon.h"
#include "images/focused_exponential_icon.h"
#include "images/focused_normal_icon.h"
#include "images/focused_poisson_icon.h"
#include "images/focused_uniform_icon.h"

namespace Probability {

LawController::ContentView::ContentView(SelectableTableView * selectableTableView) :
  m_titleView(KDText::FontSize::Small, I18n::Message::ChooseLaw, 0.5f, 0.5f, Palette::GreyDark, Palette::WallScreen),
  m_selectableTableView(selectableTableView)
{
}

int LawController::ContentView::numberOfSubviews() const {
  return 2;
}

View * LawController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return &m_titleView;
  }
  return m_selectableTableView;
}

void LawController::ContentView::layoutSubviews() {
  KDCoordinate titleHeight = KDText::charSize(KDText::FontSize::Small).height()+k_titleMargin;
  m_titleView.setFrame(KDRect(0, 0, bounds().width(), titleHeight));
  m_selectableTableView->setFrame(KDRect(0, titleHeight, bounds().width(),  bounds().height()-titleHeight));
}

static I18n::Message sMessages[] = {
  I18n::Message::Binomial,
  I18n::Message::Uniforme,
  I18n::Message::Exponential,
  I18n::Message::Normal,
  I18n::Message::Poisson
};

LawController::LawController(Responder * parentResponder, Law * law, ParametersController * parametersController) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin-ContentView::k_titleMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this),
  m_contentView(&m_selectableTableView),
  m_law(law),
  m_parametersController(parametersController)
{
  m_messages = sMessages;
  assert(m_law != nullptr);
}

View * LawController::view() {
  return &m_contentView;
}

void Probability::LawController::viewWillAppear() {
  selectRow((int)m_law->type());
}

void Probability::LawController::didBecomeFirstResponder() {
  App::Snapshot * snapshot = (App::Snapshot *)app()->snapshot();
  snapshot->setActivePage(App::Snapshot::Page::Law);
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool Probability::LawController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    StackViewController * stack = (StackViewController *)parentResponder();
    setLawAccordingToIndex(selectedRow());
    stack->push(m_parametersController, KDColorWhite, Palette::PurpleBright, Palette::PurpleBright);
    return true;
  }
  return false;
}

int Probability::LawController::numberOfRows() {
  return k_totalNumberOfModels;
};

HighlightCell * Probability::LawController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfModels);
  return &m_cells[index];
}

int Probability::LawController::reusableCellCount() {
  return k_totalNumberOfModels;
}

void Probability::LawController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  Cell * myCell = (Cell *)cell;
  myCell->setLabel(m_messages[index]);
  const Image * images[5] = {ImageStore::BinomialIcon, ImageStore::UniformIcon, ImageStore::ExponentialIcon,
    ImageStore::NormalIcon, ImageStore::PoissonIcon};
  const Image * focusedImages[5] = {ImageStore::FocusedBinomialIcon, ImageStore::FocusedUniformIcon, ImageStore::FocusedExponentialIcon,
    ImageStore::FocusedNormalIcon, ImageStore::FocusedPoissonIcon};
  myCell->setImage(images[index], focusedImages[index]);
  myCell->reloadCell();
}

KDCoordinate Probability::LawController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void Probability::LawController::setLawAccordingToIndex(int index) {
  if ((int)m_law->type() == index) {
    return;
  }
  m_law->~Law();
  switch (index) {
    case 0:
      new(m_law) BinomialLaw();
      break;
    case 1:
      new(m_law) UniformLaw();
      break;
    case 2:
      new(m_law) ExponentialLaw();
      break;
    case 3:
      new(m_law) NormalLaw();
      break;
    case 4:
      new(m_law) PoissonLaw();
      break;
    default:
     return;
  }
  m_parametersController->reinitCalculation();
}

}
