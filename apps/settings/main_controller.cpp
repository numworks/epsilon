#include "main_controller.h"
#include "../global_preferences.h"
#include "../i18n.h"
#include "../../poincare/src/layout/baseline_relative_layout.h"
#include "../../poincare/src/layout/string_layout.h"
#include <assert.h>
#include <poincare.h>

using namespace Poincare;

namespace Settings {

const SettingsMessageTree angleChildren[2] = {SettingsMessageTree(I18n::Message::Degres), SettingsMessageTree(I18n::Message::Radian)};
const SettingsMessageTree FloatDisplayModeChildren[3] = {SettingsMessageTree(I18n::Message::Auto), SettingsMessageTree(I18n::Message::Scientific), SettingsMessageTree(I18n::Message::SignificantFigures)};
const SettingsMessageTree complexFormatChildren[2] = {SettingsMessageTree(I18n::Message::Default), SettingsMessageTree(I18n::Message::Default)};
const SettingsMessageTree examChildren[1] = {SettingsMessageTree(I18n::Message::ActivateExamMode)};
const SettingsMessageTree aboutChildren[3] = {SettingsMessageTree(I18n::Message::SoftwareVersion), SettingsMessageTree(I18n::Message::SerialNumber), SettingsMessageTree(I18n::Message::FccId)};

#if EPSILON_SOFTWARE_UPDATE_PROMPT
const SettingsMessageTree menu[8] =
#else
const SettingsMessageTree menu[7] =
#endif
  {SettingsMessageTree(I18n::Message::AngleUnit, angleChildren, 2), SettingsMessageTree(I18n::Message::DisplayMode, FloatDisplayModeChildren, 3), SettingsMessageTree(I18n::Message::ComplexFormat, complexFormatChildren, 2),
  SettingsMessageTree(I18n::Message::Brightness), SettingsMessageTree(I18n::Message::Language), SettingsMessageTree(I18n::Message::ExamMode, examChildren, 1),
#if EPSILON_SOFTWARE_UPDATE_PROMPT
  SettingsMessageTree(I18n::Message::UpdatePopUp),
#endif
  SettingsMessageTree(I18n::Message::About, aboutChildren, 3)};
#if EPSILON_SOFTWARE_UPDATE_PROMPT
const SettingsMessageTree model = SettingsMessageTree(I18n::Message::SettingsApp, menu, 8);
#else
const SettingsMessageTree model = SettingsMessageTree(I18n::Message::SettingsApp, menu, 7);
#endif

MainController::MainController(Responder * parentResponder) :
  ViewController(parentResponder),
#if EPSILON_SOFTWARE_UPDATE_PROMPT
  m_updateCell(I18n::Message::Default, KDText::FontSize::Large),
#endif
  m_complexFormatCell(I18n::Message::Default, KDText::FontSize::Large),
  m_brightnessCell(I18n::Message::Default, KDText::FontSize::Large),
  m_complexFormatLayout(nullptr),
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this),
  m_messageTreeModel((MessageTree *)&model),
  m_subController(this),
  m_languageController(this, 13)
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
  if (m_messageTreeModel->children(selectedRow())->numberOfChildren() == 0) {
#if EPSILON_SOFTWARE_UPDATE_PROMPT
    if (m_messageTreeModel->children(selectedRow())->label() == I18n::Message::UpdatePopUp) {
      if (event == Ion::Events::OK || event == Ion::Events::EXE) {
        GlobalPreferences::sharedGlobalPreferences()->setShowUpdatePopUp(!GlobalPreferences::sharedGlobalPreferences()->showUpdatePopUp());
        m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
        return true;
      }
      return false;
    }
#endif
    if (m_messageTreeModel->children(selectedRow())->label() == I18n::Message::Brightness) {
      if (event == Ion::Events::Right || event == Ion::Events::Left || event == Ion::Events::Plus || event == Ion::Events::Minus) {
        int delta = Ion::Backlight::MaxBrightness/GlobalPreferences::NumberOfBrightnessStates;
        int direction = (event == Ion::Events::Right || event == Ion::Events::Plus) ? delta : -delta;
        GlobalPreferences::sharedGlobalPreferences()->setBrightnessLevel(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel()+direction);
        m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
        return true;
      }
      return false;
    }
    if (m_messageTreeModel->children(selectedRow())->label() == I18n::Message::Language) {
      if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
        stackController()->push(&m_languageController);
        return true;
      }
      return false;
    }
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    m_subController.setMessageTreeModel(m_messageTreeModel->children(selectedRow()));
    StackViewController * stack = stackController();
    stack->push(&m_subController);
    return true;
  }
  return false;
}

int MainController::numberOfRows() {
  return m_messageTreeModel->numberOfChildren();
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
#if EPSILON_SOFTWARE_UPDATE_PROMPT
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
#if EPSILON_SOFTWARE_UPDATE_PROMPT
  if (j == 6) {
    return 3;
  }
#endif
  return 0;
}

void MainController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  myCell->setMessage(m_messageTreeModel->children(index)->label());

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
  if (index == 4) {
    int index = (int)GlobalPreferences::sharedGlobalPreferences()->language()-1;
    static_cast<MessageTableCellWithChevronAndMessage *>(cell)->setSubtitle(I18n::LanguageNames[index]);
    return;
  }
#if EPSILON_SOFTWARE_UPDATE_PROMPT
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
      myTextCell->setSubtitle(m_messageTreeModel->children(index)->children((int)Preferences::sharedPreferences()->angleUnit())->label());
      break;
    case 1:
      myTextCell->setSubtitle(m_messageTreeModel->children(index)->children((int)Preferences::sharedPreferences()->displayMode())->label());
      break;
    default:
      myTextCell->setSubtitle(I18n::Message::Default);
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
