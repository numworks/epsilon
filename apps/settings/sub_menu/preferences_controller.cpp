#include "preferences_controller.h"

#include <apps/apps_container.h>
#include <apps/global_preferences.h>
#include <assert.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

#include <algorithm>
#include <cmath>

using namespace Poincare;
using namespace Escher;

namespace Settings {

PreferencesController::PreferencesController(Responder *parentResponder)
    : GenericSubController(parentResponder) {}

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

int PreferencesController::reusableCellCount(int type) const {
  return k_totalNumberOfCell;
}

Layout PreferencesController::layoutForPreferences(I18n::Message message) {
  switch (message) {
    // Angle Unit
    case I18n::Message::Degrees:
      return "90°"_l;
    case I18n::Message::Radian:
      return KRackL(KFracL("π"_l, "2"_l));
    case I18n::Message::Gradians:
      return "100 gon"_l;

    // Display Mode format
    case I18n::Message::Decimal:
      return "0.1234"_l;
    case I18n::Message::Scientific:
      // TODO handle small E in rack litterals
      // return "1.234ᴇ-1"_l;
      return "1.234"_l ^ KCodePointL<UCodePointLatinLetterSmallCapitalE>() ^
             "-1"_l;
    case I18n::Message::Engineering:
      return "123.4"_l ^ KCodePointL<UCodePointLatinLetterSmallCapitalE>() ^
             "-3"_l;

    // Edition mode
    case I18n::Message::Edition2D:
      return "1+"_l ^ KFracL("2"_l, "3"_l);
    case I18n::Message::EditionLinear:
      return "1+2/3"_l;

    // Complex format
    case I18n::Message::Real:
      return "x"_l;
    case I18n::Message::Algebraic:
      return "a+ib"_l;
    case I18n::Message::Exponential:
      return "re"_l ^ KSuperscriptL("iθ"_l);

    // Font size
    case I18n::Message::LargeFont:
    case I18n::Message::SmallFont:
      return "abc"_l;

    default:
      assert(false);
      return Layout();
  }
}

void PreferencesController::fillCellForRow(HighlightCell *cell, int row) {
  GenericSubController::fillCellForRow(cell, row);
  MenuCell<MessageTextView, LayoutView> *myCell =
      static_cast<MenuCell<MessageTextView, LayoutView> *>(cell);
  I18n::Message message = m_messageTreeModel->childAtIndex(row)->label();
  myCell->subLabel()->setLayout(layoutForPreferences(message));
  myCell->subLabel()->setFont(
      message == I18n::Message::SmallFont ? k_layoutFont : KDFont::Size::Large);
}

KDCoordinate PreferencesController::nonMemoizedRowHeight(int row) {
  MenuCell<MessageTextView, LayoutView> tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, row);
}

void PreferencesController::setPreferenceWithValueIndex(I18n::Message message,
                                                        int valueIndex) {
  Preferences *preferences = Preferences::SharedPreferences();
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
    GlobalPreferences::SharedGlobalPreferences()->setFont(
        valueIndex == 0 ? KDFont::Size::Large : KDFont::Size::Small);
  }
}

int PreferencesController::valueIndexForPreference(
    I18n::Message message) const {
  Preferences *preferences = Preferences::SharedPreferences();
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
    return GlobalPreferences::SharedGlobalPreferences()->font() ==
                   KDFont::Size::Large
               ? 0
               : 1;
  }
  return 0;
}

}  // namespace Settings
