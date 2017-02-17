#include "main_controller.h"
#include "../global_preferences.h"
#include <assert.h>
#include <poincare.h>

using namespace Poincare;

namespace Settings {

const SettingsNode angleChildren[2] = {SettingsNode("Degre"), SettingsNode("Radian")};
const SettingsNode FloatDisplayModeChildren[2] = {SettingsNode("Auto"), SettingsNode("Scientifique")};
const SettingsNode numberTypeChildren[2] = {SettingsNode("Reel"), SettingsNode("Complexe")};
const SettingsNode complexFormatChildren[2] = {SettingsNode("Algebrique"), SettingsNode("Polaire")};
const SettingsNode languageChildren[3] = {SettingsNode("Anglais"), SettingsNode("Francais"), SettingsNode("Espagnol")};

const SettingsNode menu[5] = {SettingsNode("Unite d'angles", angleChildren, 2), SettingsNode("Format resultat", FloatDisplayModeChildren, 2),
  SettingsNode("Reel ou complexe", numberTypeChildren, 2), SettingsNode("Format complexe", complexFormatChildren, 2),
  SettingsNode("Langue", languageChildren, 3)};
const SettingsNode model = SettingsNode("Parametres", menu, 5);

MainController::MainController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_cells{PointerTableCellWithChevronAndPointer(KDText::FontSize::Large), PointerTableCellWithChevronAndPointer(KDText::FontSize::Large), PointerTableCellWithChevronAndPointer(KDText::FontSize::Large),
    PointerTableCellWithChevronAndPointer(KDText::FontSize::Large), PointerTableCellWithChevronAndPointer(KDText::FontSize::Large)},
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_nodeModel((Node *)&model),
  m_subController(this)
{
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

HighlightCell * MainController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  return &m_cells[index];
}

int MainController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate MainController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void MainController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  PointerTableCellWithChevronAndPointer * myCell = (PointerTableCellWithChevronAndPointer *)cell;
  myCell->setText(m_nodeModel->children(index)->label());
  switch (index) {
    case 0:
      myCell->setSubtitle(m_nodeModel->children(index)->children((int)Preferences::sharedPreferences()->angleUnit())->label());
      break;
    case 1:
      myCell->setSubtitle(m_nodeModel->children(index)->children((int)Preferences::sharedPreferences()->displayMode())->label());
      break;
    case 2:
      myCell->setSubtitle(m_nodeModel->children(index)->children((int)Preferences::sharedPreferences()->numberType())->label());
      break;
    case 3:
      myCell->setSubtitle(m_nodeModel->children(index)->children((int)Preferences::sharedPreferences()->complexFormat())->label());
      break;
    case 4:
      myCell->setSubtitle(m_nodeModel->children(index)->children((int)GlobalPreferences::sharedGlobalPreferences()->language())->label());
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
