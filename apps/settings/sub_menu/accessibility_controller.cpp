#include "accessibility_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
#include <assert.h>

using namespace Shared;

namespace Settings {

AccessibilityController::AccessibilityController(Responder * parentResponder) :
  GenericSubController(parentResponder)
{
  for (int i = 0; i < k_totalNumberOfSwitchCells; i++) {
    m_switchCells[i].setMessageFont(KDFont::LargeFont);
  }
  for (int i = 0; i < k_totalNumberOfGaugeCells; i++) {
    m_gaugeCells[i].setMessageFont(KDFont::LargeFont);
  }
}

bool AccessibilityController::handleEvent(Ion::Events::Event event) {
  bool invertEnabled = KDIonContext::sharedContext()->invertEnabled;
  bool zoomEnabled = KDIonContext::sharedContext()->zoomEnabled;
  bool gammaEnabled = KDIonContext::sharedContext()->gammaEnabled;
  int redGamma, greenGamma, blueGamma;
  KDIonContext::sharedContext()->gamma.gamma(redGamma, greenGamma, blueGamma);

  if ((event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) && (selectedRow() <= 2)) {
    if (selectedRow() == 0) {
      invertEnabled = !invertEnabled;
    }
    else if (selectedRow() == 1) {
      zoomEnabled = !zoomEnabled;
    }
    else if (selectedRow() == 2) {
      gammaEnabled = !gammaEnabled;
    }
    else {
      GenericSubController::handleEvent(event);
    }
  }
  else if (event == Ion::Events::Right || event == Ion::Events::Left || event == Ion::Events::Plus || event == Ion::Events::Minus) {
    int direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? 1 : -1;
    if (selectedRow() == 3) {
      redGamma += direction;
    }
    else if (selectedRow() == 4) {
      greenGamma += direction;
    }
    else if (selectedRow() == 5) {
      blueGamma += direction;
    }
    else {
      return GenericSubController::handleEvent(event);
    }
  }
  else {
    return GenericSubController::handleEvent(event);
  }
  KDIonContext::sharedContext()->invertEnabled = invertEnabled;
  KDIonContext::sharedContext()->zoomEnabled = zoomEnabled;
  KDIonContext::sharedContext()->gammaEnabled = gammaEnabled;
  KDIonContext::sharedContext()->gamma.setGamma(redGamma, greenGamma, blueGamma);
  KDIonContext::sharedContext()->updatePostProcessingEffects();
  m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  AppsContainer::sharedAppsContainer()->redrawWindow(true);
  return true;
}

HighlightCell * AccessibilityController::reusableCell(int index, int type) {
  assert(type == 1 || type == 2);
  if (type == 2) {
    assert(index >= 0 && index < k_totalNumberOfSwitchCells);
    return &m_switchCells[index];
  }
  else if (type == 1) {
    assert(index >= 0 && index < k_totalNumberOfGaugeCells);
    return &m_gaugeCells[index];
  }
  return nullptr;
}

int AccessibilityController::reusableCellCount(int type) {
  assert(type == 1 || type == 2);
  if (type == 2) {
    return k_totalNumberOfSwitchCells;
  }
  else if (type == 1) {
    return k_totalNumberOfGaugeCells;
  }
  return 0;
}

void AccessibilityController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);

  MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;

  if (index == 0) {
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(KDIonContext::sharedContext()->invertEnabled);
  }
  else if (index == 1) {
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(KDIonContext::sharedContext()->zoomEnabled);
  }
  else if (index == 2) {
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(KDIonContext::sharedContext()->gammaEnabled);
  }
  else {
    MessageTableCellWithGauge * myGaugeCell = (MessageTableCellWithGauge *)cell;
    GaugeView * myGauge = (GaugeView *)myGaugeCell->accessoryView();

    float redGamma, greenGamma, blueGamma, level;
    KDIonContext::sharedContext()->gamma.gamma(redGamma, greenGamma, blueGamma);
    if (index == 3) {
      level = redGamma;
    }
    else if (index == 4) {
      level = greenGamma;
    }
    else {
      level = blueGamma;
    }
    myGauge->setLevel(level);
  }
}

int AccessibilityController::typeAtLocation(int i, int j) {
  switch (j) {
    case 0:
    case 1:
    case 2:
      return 2;
    default:
      return 1;
  }
}

}
