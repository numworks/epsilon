#include "preferences_controller.h"

#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/fraction_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/vertical_offset_layout.h>

#include <algorithm>
#include <cmath>

#include "../../apps_container.h"
#include "../../global_preferences.h"

using namespace Poincare;
using namespace Escher;

namespace Settings {

PreferencesController::PreferencesController(Responder *parentResponder)
    : GenericSubController(parentResponder) {}

void PreferencesController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableListView);
}

bool PreferencesController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    /* Generic behaviour of preference menu*/
    // In that case, events OK and EXE are handled by the cell
    assert(m_messageTreeModel->label() != I18n::Message::DisplayMode ||
           selectedRow() != numberOfRows() - 1);
    setPreferenceWithValueIndex(m_messageTreeModel->label(), selectedRow());
    AppsContainer::sharedAppsContainer()->refreshPreferences();
    StackViewController *stack = stackController();
    stack->pop();
    return true;
  }
  return GenericSubController::handleEvent(event);
}

HighlightCell *PreferencesController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0 && index < k_totalNumberOfCell);
  return &m_cells[index];
}

int PreferencesController::reusableCellCount(int type) {
  return k_totalNumberOfCell;
}

Layout PreferencesController::layoutForPreferences(I18n::Message message) {
  switch (message) {
    // Angle Unit
    case I18n::Message::Degrees: {
      const char *degEx = "90°";
      return LayoutHelper::String(degEx, strlen(degEx));
    }
    case I18n::Message::Radian:
      return FractionLayout::Builder(
          CodePointLayout::Builder(UCodePointGreekSmallLetterPi),
          CodePointLayout::Builder('2'));
    case I18n::Message::Gradians: {
      const char *degEx = "100 gon";
      return LayoutHelper::String(degEx, strlen(degEx));
    }
    // Display Mode format
    case I18n::Message::Decimal:
      return LayoutHelper::String("0.1234", 6);
    case I18n::Message::Scientific: {
      const char *text = "1.234ᴇ-1";
      return LayoutHelper::String(text, strlen(text));
    }
    case I18n::Message::Engineering: {
      const char *text = "123.4ᴇ-3";
      return LayoutHelper::String(text, strlen(text));
    }

    // Edition mode
    case I18n::Message::Edition2D:
      return HorizontalLayout::Builder(
          LayoutHelper::String("1+", 2),
          FractionLayout::Builder(LayoutHelper::String("2", 1),
                                  LayoutHelper::String("3", 1)));
    case I18n::Message::EditionLinear:
      return LayoutHelper::String("1+2/3", 5);

    // Complex format
    case I18n::Message::Real:
      return CodePointLayout::Builder('x');
    case I18n::Message::Cartesian: {
      const char *text = "a+ib";
      return LayoutHelper::String(text, strlen(text));
    }
    case I18n::Message::Polar: {
      const char *base = "re";
      const char *superscript = "iθ";
      return HorizontalLayout::Builder(
          LayoutHelper::String(base, strlen(base)),
          VerticalOffsetLayout::Builder(
              LayoutHelper::String(superscript, strlen(superscript)),
              VerticalOffsetLayoutNode::VerticalPosition::Superscript));
    }

    // Font size
    case I18n::Message::LargeFont:
    case I18n::Message::SmallFont: {
      const char *text = "abc";
      return LayoutHelper::String(text, strlen(text));
    }

    default:
      assert(false);
      return Layout();
  }
}

void PreferencesController::willDisplayCellForIndex(HighlightCell *cell,
                                                    int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);
  MessageTableCellWithExpression *myCell =
      static_cast<MessageTableCellWithExpression *>(cell);
  I18n::Message message = m_messageTreeModel->childAtIndex(index)->label();
  myCell->setLayout(layoutForPreferences(message));
  myCell->setFont(message == I18n::Message::SmallFont ? k_layoutFont
                                                      : KDFont::Size::Large);
}

KDCoordinate PreferencesController::nonMemoizedRowHeight(int index) {
  MessageTableCellWithExpression tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, index);
}

void PreferencesController::setPreferenceWithValueIndex(I18n::Message message,
                                                        int valueIndex) {
  Preferences *preferences = Preferences::sharedPreferences;
  if (message == I18n::Message::AngleUnit) {
    preferences->setAngleUnit((Preferences::AngleUnit)valueIndex);
  } else if (message == I18n::Message::DisplayMode) {
    Preferences::PrintFloatMode mode = (Preferences::PrintFloatMode)valueIndex;
    preferences->setDisplayMode(mode);
    if (mode == Preferences::PrintFloatMode::Engineering) {
      /* In Engineering mode, the number of significant digits cannot be lower
       * than 3, because we need to be able to display 100 for instance. */
      // TODO: Add warning about signifiant digits change ?
      preferences->setNumberOfSignificantDigits(
          std::max<int>(preferences->numberOfSignificantDigits(), 3));
    }
  } else if (message == I18n::Message::EditionMode) {
    preferences->setEditionMode((Preferences::EditionMode)valueIndex);
  } else if (message == I18n::Message::ComplexFormat) {
    preferences->setComplexFormat((Preferences::ComplexFormat)valueIndex);
  } else if (message == I18n::Message::FontSizes) {
    GlobalPreferences::sharedGlobalPreferences->setFont(
        valueIndex == 0 ? KDFont::Size::Large : KDFont::Size::Small);
  }
}

int PreferencesController::valueIndexForPreference(
    I18n::Message message) const {
  Preferences *preferences = Preferences::sharedPreferences;
  if (message == I18n::Message::AngleUnit) {
    return (int)preferences->angleUnit();
  }
  if (message == I18n::Message::DisplayMode) {
    return (int)preferences->displayMode();
  }
  if (message == I18n::Message::EditionMode) {
    return (int)preferences->editionMode();
  }
  if (message == I18n::Message::ComplexFormat) {
    return (int)preferences->complexFormat();
  }
  if (message == I18n::Message::FontSizes) {
    return GlobalPreferences::sharedGlobalPreferences->font() ==
                   KDFont::Size::Large
               ? 0
               : 1;
  }
  return 0;
}

}  // namespace Settings
