#include "main_controller.h"
#include "../global_preferences.h"
#include "../../poincare/src/layout/baseline_relative_layout.h"
#include "../../poincare/src/layout/string_layout.h"
#include <assert.h>
#include <poincare.h>

using namespace Poincare;

namespace Settings {

const SettingsNode angleChildren[2] = {SettingsNode(I18n::Message::Degres), SettingsNode(I18n::Message::Radian)};
const SettingsNode FloatDisplayModeChildren[2] = {SettingsNode(I18n::Message::Auto), SettingsNode(I18n::Message::Scientific)};
const SettingsNode complexFormatChildren[2] = {SettingsNode(I18n::Message::Default), SettingsNode(I18n::Message::Default)};
const SettingsNode languageChildren[3] = {SettingsNode(I18n::Message::French), SettingsNode(I18n::Message::English), SettingsNode(I18n::Message::Spanish)};

const SettingsNode menu[4] = {SettingsNode(I18n::Message::AngleUnit, angleChildren, 2), SettingsNode(I18n::Message::DisplayMode, FloatDisplayModeChildren, 2), SettingsNode(I18n::Message::ComplexFormat, complexFormatChildren, 2),
  SettingsNode(I18n::Message::Language, languageChildren, 3)};
const SettingsNode model = SettingsNode(I18n::Message::SettingsApp, menu, 4);

MainController::MainController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_cells{MessageTableCellWithChevronAndMessage(KDText::FontSize::Large, KDText::FontSize::Small),
    MessageTableCellWithChevronAndMessage(KDText::FontSize::Large, KDText::FontSize::Small), MessageTableCellWithChevronAndMessage(KDText::FontSize::Large, KDText::FontSize::Small)},
  m_complexFormatCell(MessageTableCellWithChevronAndExpression(I18n::Message::Default, KDText::FontSize::Large)),
  m_selectableTableView(SelectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
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
const char * MainController::title() {
  return I18n::translate(m_nodeModel->label());
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
  MessageTableCell * myCell = (MessageTableCell *)cell;
  myCell->setMessage(m_nodeModel->children(index)->label());

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
    MessageTableCellWithChevronAndExpression * myExpCell = (MessageTableCellWithChevronAndExpression *)cell;
    myExpCell->setExpression(m_complexFormatLayout);
    return;
  }
  MessageTableCellWithChevronAndMessage * myTextCell = (MessageTableCellWithChevronAndMessage *)cell;
  switch (index) {
    case 0:
      myTextCell->setSubtitle(m_nodeModel->children(index)->children((int)Preferences::sharedPreferences()->angleUnit())->label());
      break;
    case 1:
      myTextCell->setSubtitle(m_nodeModel->children(index)->children((int)Preferences::sharedPreferences()->displayMode())->label());
      break;
    case 3:
      myTextCell->setSubtitle(m_nodeModel->children(index)->children((int)GlobalPreferences::sharedGlobalPreferences()->language()-1)->label());
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
