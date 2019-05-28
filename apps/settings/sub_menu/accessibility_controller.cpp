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
  GlobalPreferences *globalPreferences = GlobalPreferences::sharedGlobalPreferences();
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (selectedRow() == 0) {
      bool newState = !globalPreferences->accessibilityInvertColors();
      globalPreferences->setAccessibilityInvertColors(newState);
    }
    else if (selectedRow() == 1) {
      bool newState = !globalPreferences->accessibilityMagnify();
      globalPreferences->setAccessibilityMagnify(newState);
    }
    else if (selectedRow() == 2) {
      bool newState = !globalPreferences->accessibilityGamma();
      globalPreferences->setAccessibilityGamma(newState);
    }
    else {
      GenericSubController::handleEvent(event);
    }
  }
  else if (event == Ion::Events::Right || event == Ion::Events::Left || event == Ion::Events::Plus || event == Ion::Events::Minus) {
    int direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? 1 : -1;
    if (selectedRow() == 3) {
      globalPreferences->setAccessibilityGammaRed(globalPreferences->accessibilityGammaRed()+direction);
    }
    else if (selectedRow() == 4) {
      globalPreferences->setAccessibilityGammaGreen(globalPreferences->accessibilityGammaGreen()+direction);
    }
    else if (selectedRow() == 5) {
      globalPreferences->setAccessibilityGammaBlue(globalPreferences->accessibilityGammaBlue()+direction);
    }
    else {
      return GenericSubController::handleEvent(event);
    }
  }
  else {
    return GenericSubController::handleEvent(event);
  }
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
  GlobalPreferences *globalPreferences = GlobalPreferences::sharedGlobalPreferences();

  if (index == 0) {
    MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(globalPreferences->accessibilityInvertColors());
  }
  else if (index == 1) {
    MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(globalPreferences->accessibilityMagnify());
  }
  else if (index == 2) {
    MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(globalPreferences->accessibilityGamma());
  }
  else {
    MessageTableCellWithGauge * myGaugeCell = (MessageTableCellWithGauge *)cell;
    GaugeView * myGauge = (GaugeView *)myGaugeCell->accessoryView();
    int level;
    if (index == 3) {
      level = globalPreferences->accessibilityGammaRed();
    }
    else if (index == 4) {
      level = globalPreferences->accessibilityGammaGreen();
    }
    else {
      level = globalPreferences->accessibilityGammaBlue();
    }
    myGauge->setLevel(level/(float)GlobalPreferences::NumberOfGammaStates);
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
