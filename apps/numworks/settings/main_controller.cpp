#include "main_controller.h"
#include "apps/global_preferences.h"
#include "apps/i18n.h"
#include "poincare/src/layout/baseline_relative_layout.h"
#include "poincare/src/layout/string_layout.h"
#include <assert.h>
#include <poincare.h>

using namespace Poincare;

namespace Settings {

const SettingsNode angleChildren[2] = {SettingsNode(&I18n::Common::Degres), SettingsNode(&I18n::Common::Radian)};
const SettingsNode FloatDisplayModeChildren[2] = {SettingsNode(&I18n::Common::Auto), SettingsNode(&I18n::Common::Scientific)};
const SettingsNode complexFormatChildren[2] = {SettingsNode(&I18n::Common::Default), SettingsNode(&I18n::Common::Default)};
const SettingsNode languageChildren[I18n::NumberOfLanguages] = {SettingsNode(&I18n::Common::English), SettingsNode(&I18n::Common::French), SettingsNode(&I18n::Common::Spanish), SettingsNode(&I18n::Common::German), SettingsNode(&I18n::Common::Portuguese)};
const SettingsNode examChildren[1] = {SettingsNode(&I18n::Common::ActivateExamMode)};
const SettingsNode aboutChildren[3] = {SettingsNode(&I18n::Common::SoftwareVersion), SettingsNode(&I18n::Common::SerialNumber), SettingsNode(&I18n::Common::FccId)};

#if OS_WITH_SOFTWARE_UPDATE_PROMPT
const SettingsNode menu[8] =
#else
const SettingsNode menu[7] =
#endif
  {SettingsNode(&I18n::Common::AngleUnit, angleChildren, 2), SettingsNode(&I18n::Common::DisplayMode, FloatDisplayModeChildren, 2), SettingsNode(&I18n::Common::ComplexFormat, complexFormatChildren, 2),
  SettingsNode(&I18n::Common::Brightness), SettingsNode(&I18n::Common::Language, languageChildren, I18n::NumberOfLanguages), SettingsNode(&I18n::Common::ExamMode, examChildren, 1),
#if OS_WITH_SOFTWARE_UPDATE_PROMPT
  SettingsNode(&I18n::Common::UpdatePopUp),
#endif
  SettingsNode(&I18n::Common::About, aboutChildren, 3)};
#if OS_WITH_SOFTWARE_UPDATE_PROMPT
const SettingsNode model = SettingsNode(&I18n::Common::SettingsApp, menu, 8);
#else
const SettingsNode model = SettingsNode(&I18n::Common::SettingsApp, menu, 7);
#endif

MainController::MainController(Responder * parentResponder) :
  ViewController(parentResponder),
#if OS_WITH_SOFTWARE_UPDATE_PROMPT
  m_updateCell(&I18n::Common::Default, KDText::FontSize::Large),
#endif
  m_complexFormatCell(&I18n::Common::Default, KDText::FontSize::Large),
  m_brightnessCell(&I18n::Common::Default, KDText::FontSize::Large),
  m_complexFormatLayout(nullptr),
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this),
  m_nodeModel((Node *)&model),
  m_subController(this)
{
  for (int i = 0; i < k_numberOfSimpleChevronCells; i++) {
    m_cells[i].setMessageFontSize(KDText::FontSize::Large);
  }
}

MainController::~MainController() {
  if (m_complexFormatLayout) {
    delete m_complexFormatLayout;
    m_complexFormatLayout = nullptr;
  }
}
const char * MainController::title() {
  return I18n::translate(m_nodeModel->label());
}

View * MainController::view() {
  return &m_selectableTableView;
}

void MainController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool MainController::handleEvent(Ion::Events::Event event) {
  if (m_nodeModel->children(selectedRow())->numberOfChildren() == 0) {
#if OS_WITH_SOFTWARE_UPDATE_PROMPT
    if (m_nodeModel->children(selectedRow())->label() == &I18n::Common::UpdatePopUp) {
      if (event == Ion::Events::OK || event == Ion::Events::EXE) {
        GlobalPreferences::sharedGlobalPreferences()->setShowUpdatePopUp(!GlobalPreferences::sharedGlobalPreferences()->showUpdatePopUp());
        m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
        return true;
      }
      return false;
    }
#endif
    if (m_nodeModel->children(selectedRow())->label() == &I18n::Common::Brightness) {
      if (event == Ion::Events::Right || event == Ion::Events::Left || event == Ion::Events::Plus || event == Ion::Events::Minus) {
        int delta = Ion::Backlight::MaxBrightness/GlobalPreferences::NumberOfBrightnessStates;
        int direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? delta : -delta;
        GlobalPreferences::sharedGlobalPreferences()->setBrightnessLevel(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel()+direction);
        m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
        return true;
      }
      return false;
    }
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    m_subController.setNodeModel(m_nodeModel->children(selectedRow()));
    StackViewController * stack = stackController();
    stack->push(&m_subController);
    return true;
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
    assert(index < k_numberOfSimpleChevronCells);
    return &m_cells[index];
  }
  assert(index == 0);
  if (type == 2) {
    return &m_brightnessCell;
  }
#if OS_WITH_SOFTWARE_UPDATE_PROMPT
  if (type == 3) {
    return &m_updateCell;
  }
#endif
  return &m_complexFormatCell;
}

int MainController::reusableCellCount(int type) {
  if (type == 0) {
    return k_numberOfSimpleChevronCells;
  }
  return 1;
}

int MainController::typeAtLocation(int i, int j) {
  if (j == 2) {
    return 1;
  }
  if (j == 3) {
    return 2;
  }
#if OS_WITH_SOFTWARE_UPDATE_PROMPT
  if (j == 6) {
    return 3;
  }
#endif
  return 0;
}

void MainController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  myCell->setMessage(m_nodeModel->children(index)->label());

  if (index == 2) {
    if (m_complexFormatLayout) {
      delete m_complexFormatLayout;
      m_complexFormatLayout = nullptr;
    }
    if (Preferences::sharedPreferences()->complexFormat() == Expression::ComplexFormat::Cartesian) {
      const char text[] = {'a','+', Ion::Charset::IComplex, 'b', ' '};
      m_complexFormatLayout = new StringLayout(text, sizeof(text), KDText::FontSize::Small);
    } else {
      const char base[] = {'r', Ion::Charset::Exponential};
      const char superscript[] = {Ion::Charset::IComplex, Ion::Charset::SmallTheta, ' '};
      m_complexFormatLayout = new BaselineRelativeLayout(new StringLayout(base, sizeof(base), KDText::FontSize::Small), new StringLayout(superscript, sizeof(superscript), KDText::FontSize::Small), BaselineRelativeLayout::Type::Superscript);
    }
    MessageTableCellWithChevronAndExpression * myExpCell = (MessageTableCellWithChevronAndExpression *)cell;
    myExpCell->setExpression(m_complexFormatLayout);
    return;
  }
  if (index == 3) {
    MessageTableCellWithGauge * myGaugeCell = (MessageTableCellWithGauge *)cell;
    GaugeView * myGauge = (GaugeView *)myGaugeCell->accessoryView();
    myGauge->setLevel((float)GlobalPreferences::sharedGlobalPreferences()->brightnessLevel()/(float)Ion::Backlight::MaxBrightness);
    return;
  }
#if OS_WITH_SOFTWARE_UPDATE_PROMPT
  if (index == 6) {
    MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(GlobalPreferences::sharedGlobalPreferences()->showUpdatePopUp());
    return;
  }
#endif
  MessageTableCellWithChevronAndMessage * myTextCell = (MessageTableCellWithChevronAndMessage *)cell;
  switch (index) {
    case 0:
      myTextCell->setSubtitle(m_nodeModel->children(index)->children((int)Preferences::sharedPreferences()->angleUnit())->label());
      break;
    case 1:
      myTextCell->setSubtitle(m_nodeModel->children(index)->children((int)Preferences::sharedPreferences()->displayMode())->label());
      break;
    case 4:
      myTextCell->setSubtitle(m_nodeModel->children(index)->children((int)GlobalPreferences::sharedGlobalPreferences()->language()-1)->label());
      break;
    default:
      myTextCell->setSubtitle(&I18n::Common::Default);
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
