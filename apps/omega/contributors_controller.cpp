#include "contributors_controller.h"
#include <assert.h>
#include "apps/i18n.h"

// using namespace Shared;

namespace Omega {

// constexpr SettingsMessageTree s_contributorsChildren[17] = {
//   SettingsMessageTree(I18n::Message::Developers),
//   SettingsMessageTree(I18n::Message::QuentinGuidee),
//   SettingsMessageTree(I18n::Message::SandraSimmons),
//   SettingsMessageTree(I18n::Message::JoachimLeFournis),
//   SettingsMessageTree(I18n::Message::JeanBaptisteBoric),
//   SettingsMessageTree(I18n::Message::MaximeFriess),
//   SettingsMessageTree(I18n::Message::David),
//   SettingsMessageTree(I18n::Message::DamienNicolet),
//   SettingsMessageTree(I18n::Message::EvannDreumont),
//   SettingsMessageTree(I18n::Message::SzaboLevente),
//   SettingsMessageTree(I18n::Message::VenceslasDuet),
//   SettingsMessageTree(I18n::Message::CharlotteThomas),
//   SettingsMessageTree(I18n::Message::BetaTesters),
//   SettingsMessageTree(I18n::Message::CyprienMejat),
//   SettingsMessageTree(I18n::Message::TimeoArnouts),
//   SettingsMessageTree(I18n::Message::LouisC),
//   SettingsMessageTree(I18n::Message::LelahelHideux)
// };

ContributorsController::ContributorsController(Responder * parentResponder) :
  ViewController(parentResponder),
  SelectableTableViewDataSource(),
  m_contributorsView(&m_selectableTableView),
  m_contributorCell(),
  m_selectableTableView(this)
{
}

View * ContributorsController::view() {
  return &m_contributorsView;
}

const char * ContributorsController::title() {
  return "Contributors";
}

void ContributorsController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void ContributorsController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_selectableTableView.reloadData();
}

bool ContributorsController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    stackController()->pop();
    return true;
  }
  return false;
}

int ContributorsController::numberOfRows() const {
  return 1;
}

KDCoordinate ContributorsController::rowHeight(int j) {
  return Metric::ParameterCellHeight;
}

KDCoordinate ContributorsController::cumulatedHeightFromIndex(int j) {
  return rowHeight(0) * j;
}

int ContributorsController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  KDCoordinate height = rowHeight(0);
  if (height == 0) {
    return 0;
  }
  return (offsetY - 1) / height;
}

HighlightCell * ContributorsController::reusableCell(int index, int type) {
  assert(index >= 0 && index <= k_numberOfCells);
  return &m_contributorCell;
}

int ContributorsController::reusableCellCount(int type) {
  return 1;
}

int ContributorsController::typeAtLocation(int i, int j) {
  return 0;
}

void ContributorsController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  //I18n::Message titles[1] = {I18n::Message::OmegaContributors};
  //myCell->setMessage(titles[index]);
  myCell->setMessage(I18n::Message::QuentinGuidee);
}

StackViewController * ContributorsController::stackController() const {
    return (StackViewController *)parentResponder();
}

}
