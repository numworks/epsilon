#include "main_controller.h"
#include <assert.h>

namespace Settings {

const SettingsNode angleChildren[2] = {SettingsNode("Degre"), SettingsNode("Radian")};
const SettingsNode displayModeChildren[2] = {SettingsNode("Auto"), SettingsNode("Scientifique")};
const SettingsNode numberTypeChildren[2] = {SettingsNode("Reel"), SettingsNode("Complexe")};
const SettingsNode complexFormatChildren[2] = {SettingsNode("Cartesien"), SettingsNode("Polaire")};
const SettingsNode languageChildren[2] = {SettingsNode("Anglais"), SettingsNode("Francais")};

const SettingsNode menu[5] = {SettingsNode("Unite d'angles", angleChildren, 2), SettingsNode("Format resultat", displayModeChildren, 2),
  SettingsNode("Reel ou complexe", numberTypeChildren, 2), SettingsNode("Format complexe", complexFormatChildren, 2),
  SettingsNode("Langue", languageChildren, 2)};
const SettingsNode model = SettingsNode("Parametres", menu, 5);

MainController::MainController(Responder * parentResponder, Preferences * preferences) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_nodeModel((Node *)&model),
  m_preferences(preferences),
  m_subController(this, m_preferences)
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

TableViewCell * MainController::reusableCell(int index) {
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

void MainController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  MenuCell * myCell = (MenuCell *)cell;
  myCell->setText(m_nodeModel->children(index)->label());
  switch (index) {
    case 0:
      myCell->setSubtitle(m_nodeModel->children(index)->children((int)m_preferences->angleUnit())->label());
      break;
    case 1:
      myCell->setSubtitle(m_nodeModel->children(index)->children((int)m_preferences->displayMode())->label());
      break;
    case 2:
      myCell->setSubtitle(m_nodeModel->children(index)->children((int)m_preferences->numberType())->label());
      break;
    case 3:
      myCell->setSubtitle(m_nodeModel->children(index)->children((int)m_preferences->complexFormat())->label());
      break;
    case 4:
      myCell->setSubtitle(m_nodeModel->children(index)->children((int)m_preferences->language())->label());
      break;
  }
}

StackViewController * MainController::stackController() const {
  return (StackViewController *)parentResponder();
}

}
