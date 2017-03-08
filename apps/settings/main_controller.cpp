#include "main_controller.h"
#include "../global_preferences.h"
#include "../../poincare/src/layout/baseline_relative_layout.h"
#include "../../poincare/src/layout/string_layout.h"
#include <assert.h>
#include <poincare.h>

using namespace Poincare;

namespace Settings {

const SettingsNode angleChildren[2] = {SettingsNode("Degres "), SettingsNode("Radians ")};
const SettingsNode FloatDisplayModeChildren[2] = {SettingsNode("Auto "), SettingsNode("Scientifique ")};
const SettingsNode complexFormatChildren[2] = {SettingsNode("a+ib "), SettingsNode("eitheta ")};
const SettingsNode languageChildren[3] = {SettingsNode("Anglais "), SettingsNode("Francais "), SettingsNode("Espagnol ")};

const SettingsNode menu[4] = {SettingsNode("Unite d'angles", angleChildren, 2), SettingsNode("Format resultat", FloatDisplayModeChildren, 2), SettingsNode("Format complexe", complexFormatChildren, 2),
  SettingsNode("Langue", languageChildren, 3)};
const SettingsNode model = SettingsNode("Parametres", menu, 4);

MainController::MainController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_cells{PointerTableCellWithChevronAndPointer(KDText::FontSize::Large, KDText::FontSize::Small),
    PointerTableCellWithChevronAndPointer(KDText::FontSize::Large, KDText::FontSize::Small), PointerTableCellWithChevronAndPointer(KDText::FontSize::Large, KDText::FontSize::Small)},
  m_complexFormatCell(PointerTableCellWithChevronAndExpression(nullptr, KDText::FontSize::Large)),
  m_selectableTableView(SelectableTableView(this, this, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin)),
  m_nodeModel((Node *)&model),
  m_subController(this)
{
}

MainController::~MainController() {
  if (m_complexFormatLayout) {
    delete m_complexFormatLayout;
    m_complexFormatLayout = nullptr;
  }
}
const char * MainController::title() const {
  return m_nodeModel->label();
}

View * MainController::view() {
  return &m_selectableTableView;
}

void MainController::didBecomeFirstResponder() {
  if (m_selectableTableView.selectedRow() < 0) {
    m_selectableTableView.selectCellAtLocation(0, 0);
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool MainController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    m_subController.setNodeModel(m_nodeModel->children(m_selectableTableView.selectedRow()), m_selectableTableView.selectedRow());
    StackViewController * stack = stackController();
    stack->push(&m_subController);
  }
  return false;
}

int MainController::numberOfRows() {
  return m_nodeModel->numberOfChildren();
};

KDCoordinate MainController::rowHeight(int j) {
  return Metric::ParameterCellHeight;
}

KDCoordinate MainController::cumulatedHeightFromIndex(int j) {
  return j*rowHeight(0);
}

int MainController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return offsetY/rowHeight(0);
}

HighlightCell * MainController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == 0) {
    assert(index < k_totalNumberOfCell-1);
    return &m_cells[index];
  }
  assert(index == 0);
  return &m_complexFormatCell;
}

int MainController::reusableCellCount(int type) {
  if (type == 0) {
    return k_totalNumberOfCell-1;
  }
  return 1;
}

int MainController::typeAtLocation(int i, int j) {
  if (j == 2) {
    return 1;
  }
  return 0;
}

void MainController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  PointerTableCell * myCell = (PointerTableCell *)cell;
  myCell->setText(m_nodeModel->children(index)->label());

  if (index == 2) {
    if (m_complexFormatLayout) {
      delete m_complexFormatLayout;
      m_complexFormatLayout = nullptr;
    }
    if (Preferences::sharedPreferences()->complexFormat() == Expression::ComplexFormat::Algebric) {
      const char text[6] = {'a','+', Ion::Charset::IComplex, 'b', ' ', 0};
      m_complexFormatLayout = new StringLayout(text, 6, KDText::FontSize::Small);
    } else {
      const char base[3] = {'r', Ion::Charset::Exponential, 0};
      const char superscrit[4] = {Ion::Charset::IComplex, Ion::Charset::SmallTheta, ' ', 0};
      m_complexFormatLayout = new BaselineRelativeLayout(new StringLayout(base, 4, KDText::FontSize::Small), new StringLayout(superscrit, 3, KDText::FontSize::Small), BaselineRelativeLayout::Type::Superscript);
    }
    PointerTableCellWithChevronAndExpression * myExpCell = (PointerTableCellWithChevronAndExpression *)cell;
    myExpCell->setExpression(m_complexFormatLayout);
    return;
  }
  PointerTableCellWithChevronAndPointer * myTextCell = (PointerTableCellWithChevronAndPointer *)cell;
  switch (index) {
    case 0:
      myTextCell->setSubtitle(m_nodeModel->children(index)->children((int)Preferences::sharedPreferences()->angleUnit())->label());
      break;
    case 1:
      myTextCell->setSubtitle(m_nodeModel->children(index)->children((int)Preferences::sharedPreferences()->displayMode())->label());
      break;
    case 3:
      myTextCell->setSubtitle(m_nodeModel->children(index)->children((int)GlobalPreferences::sharedGlobalPreferences()->language())->label());
      break;
  }
}

void MainController::viewWillAppear() {
  m_selectableTableView.reloadData();
}

StackViewController * MainController::stackController() const {
  return (StackViewController *)parentResponder();
}

}
