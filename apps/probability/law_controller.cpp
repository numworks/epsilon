#include "law_controller.h"
#include <assert.h>
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
  KDCoordinate titleHeight = KDText::stringSize("", KDText::FontSize::Small).height()+k_titleMargin;
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

LawController::LawController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin-ContentView::k_titleMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this),
  m_contentView(&m_selectableTableView),
  m_law(nullptr),
  m_parametersController(nullptr)
{
  m_messages = sMessages;
}

LawController::~LawController() {
  if (m_law) {
    delete m_law;
    m_law = nullptr;
  }
}
View * LawController::view() {
  return &m_contentView;
}

void Probability::LawController::didBecomeFirstResponder() {
  if (m_law != nullptr) {
    delete m_law;
    m_law = nullptr;
    m_parametersController.setLaw(m_law);
  }
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool Probability::LawController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    StackViewController * stack = (StackViewController *)parentResponder();
    setLawAccordingToIndex(selectedRow());
    stack->push(&m_parametersController, KDColorWhite, Palette::PurpleBright, Palette::PurpleBright);
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
  if (m_law != nullptr) {
    delete m_law;
    m_law = nullptr;
  }
  switch (index) {
    case 0:
      m_law = new BinomialLaw();
      break;
    case 1:
      m_law = new UniformLaw();
      break;
    case 2:
      m_law = new ExponentialLaw();
      break;
    case 3:
      m_law = new NormalLaw();
      break;
    case 4:
      m_law = new PoissonLaw();
      break;
    default:
     return;
  }
  m_parametersController.setLaw(m_law);
}

}
