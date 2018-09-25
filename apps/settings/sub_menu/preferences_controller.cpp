#include "preferences_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
#include <assert.h>
#include <cmath>

using namespace Poincare;

namespace Settings {

PreferencesController::PreferencesController(Responder * parentResponder) :
  GenericSubController(parentResponder)
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_cells[i].setMessageFontSize(KDText::FontSize::Large);
  }
}

void PreferencesController::didBecomeFirstResponder() {
  selectCellAtLocation(0, valueIndexForPreference(m_messageTreeModel->label()));
  app()->setFirstResponder(&m_selectableTableView);
}

bool PreferencesController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    /* Generic behaviour of preference menu*/
    assert(m_messageTreeModel->label() != I18n::Message::DisplayMode || selectedRow() != numberOfRows()-1); // In that case, events OK and EXE are handled by the cell
    setPreferenceWithValueIndex(m_messageTreeModel->label(), selectedRow());
    AppsContainer * myContainer = (AppsContainer * )app()->container();
    myContainer->refreshPreferences();
    StackViewController * stack = stackController();
    stack->pop();
    return true;
  }
  return GenericSubController::handleEvent(event);
}

HighlightCell * PreferencesController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0 && index < k_totalNumberOfCell);
  return &m_cells[index];
}

int PreferencesController::reusableCellCount(int type) {
  return k_totalNumberOfCell;
}

Layout layoutForPreferences(I18n::Message message, int rowIndex) {
  switch (message) {
    case I18n::Message::AngleUnit:
      return Layout();
    case I18n::Message::DisplayMode:
    {
      if (rowIndex == 0) {
        return LayoutHelper::String("12.34", 5, KDText::FontSize::Small);
      }
      const char text[] = {'1','.', '2', '3', '4', Ion::Charset::Exponent, '1'};
      return LayoutHelper::String(text, sizeof(text), KDText::FontSize::Small);
    }
    case I18n::Message::EditionMode:
    {
      if (rowIndex == 0) {
        return LayoutHelper::String("1+2/3", 5, KDText::FontSize::Small);
      }
      return HorizontalLayout(
          LayoutHelper::String("1+", 2, KDText::FontSize::Small),
          FractionLayout(LayoutHelper::String("2", 1, KDText::FontSize::Small), LayoutHelper::String("3", 1, KDText::FontSize::Small))
          );

    }
    case I18n::Message::ComplexFormat:
    {
      if (rowIndex == 0) {
        const char text[] = {'a','+', Ion::Charset::IComplex, 'b'};
        return LayoutHelper::String(text, sizeof(text), KDText::FontSize::Small);
      }
      const char base[] = {'r', Ion::Charset::Exponential};
      const char superscript[] = {Ion::Charset::IComplex, Ion::Charset::SmallTheta};
      return HorizontalLayout(
          LayoutHelper::String(base, sizeof(base), KDText::FontSize::Small),
          VerticalOffsetLayout(LayoutHelper::String(superscript, sizeof(superscript), KDText::FontSize::Small), VerticalOffsetLayoutNode::Type::Superscript)
          );
    }
    default:
      assert(false);
      return Layout();
  }
}

void PreferencesController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);
  MessageTableCellWithExpression * myCell = (MessageTableCellWithExpression *)cell;
  myCell->setLayout(layoutForPreferences(m_messageTreeModel->label(), index));
}

void PreferencesController::setPreferenceWithValueIndex(I18n::Message message, int valueIndex) {
  if (message == I18n::Message::AngleUnit) {
    Preferences::sharedPreferences()->setAngleUnit((Preferences::AngleUnit)valueIndex);
  }
  if (message == I18n::Message::DisplayMode) {
    Preferences::sharedPreferences()->setDisplayMode((Preferences::PrintFloatMode)valueIndex);
  }
  if (message == I18n::Message::EditionMode) {
    Preferences::sharedPreferences()->setEditionMode((Preferences::EditionMode)valueIndex);
  }
  if (message == I18n::Message::ComplexFormat) {
    Preferences::sharedPreferences()->setComplexFormat((Preferences::ComplexFormat)valueIndex);
  }
}

int PreferencesController::valueIndexForPreference(I18n::Message message) {
  if (message == I18n::Message::AngleUnit) {
    return (int)Preferences::sharedPreferences()->angleUnit();
  }
  if (message == I18n::Message::DisplayMode) {
    return (int)Preferences::sharedPreferences()->displayMode();
  }
  if (message == I18n::Message::EditionMode) {
    return (int)Preferences::sharedPreferences()->editionMode();
  }
  if (message == I18n::Message::ComplexFormat) {
    return (int)Preferences::sharedPreferences()->complexFormat();
  }
  return 0;
}

}
