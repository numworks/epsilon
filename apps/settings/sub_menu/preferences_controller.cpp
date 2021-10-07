#include "preferences_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
#include <assert.h>
#include <cmath>
#include <poincare/layout_helper.h>
#include <poincare/code_point_layout.h>
#include <poincare/fraction_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/nth_root_layout.h>
#include <algorithm>

using namespace Poincare;

namespace Settings {

PreferencesController::PreferencesController(Responder * parentResponder) :
  GenericSubController(parentResponder)
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
  }
}

void PreferencesController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool PreferencesController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    /* Generic behaviour of preference menu*/
    assert(m_messageTreeModel->label() != I18n::Message::DisplayMode || selectedRow() != numberOfRows()-1); // In that case, events OK and EXE are handled by the cell
    setPreferenceWithValueIndex(m_messageTreeModel->label(), selectedRow());
    AppsContainer::sharedAppsContainer()->refreshPreferences();
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

Layout PreferencesController::layoutForPreferences(I18n::Message message) {
  switch (message) {
    // Angle Unit
    case I18n::Message::Degrees:
    {
      const char * degEx = "90°";
      return LayoutHelper::String(degEx, strlen(degEx), k_layoutFont);
    }
    case I18n::Message::Radian:
      return FractionLayout::Builder(
          CodePointLayout::Builder(UCodePointGreekSmallLetterPi, k_layoutFont),
          CodePointLayout::Builder('2', k_layoutFont));
    case I18n::Message::Gradians:
    {
      const char * degEx = "100 gon";
      return LayoutHelper::String(degEx, strlen(degEx), k_layoutFont);
    }
    // Display Mode format
    case I18n::Message::Decimal:
      return LayoutHelper::String("0.1234", 6, k_layoutFont);
    case I18n::Message::Scientific:
    {
      const char * text = "1.234ᴇ-1";
      return LayoutHelper::String(text, strlen(text), k_layoutFont);
    }
    case I18n::Message::Engineering:
    {
      const char * text = "123.4ᴇ-3";
      return LayoutHelper::String(text, strlen(text), k_layoutFont);
    }

    // Edition mode
    case I18n::Message::Edition2D:
      return HorizontalLayout::Builder(
          LayoutHelper::String("1+", 2, k_layoutFont),
          FractionLayout::Builder(LayoutHelper::String("2", 1, k_layoutFont), LayoutHelper::String("3", 1, k_layoutFont))
        );
    case I18n::Message::EditionLinear:
      return LayoutHelper::String("1+2/3", 5, k_layoutFont);

    // Complex format
    case I18n::Message::Real:
      return CodePointLayout::Builder('x', k_layoutFont);
    case I18n::Message::Cartesian:
    {
      const char * text = "a+𝐢b";
      return LayoutHelper::String(text, strlen(text), k_layoutFont);
    }
    case I18n::Message::Polar:
    {
      const char * base = "rℯ";
      const char * superscript = "𝐢θ";
      return HorizontalLayout::Builder(
          LayoutHelper::String(base, strlen(base), k_layoutFont),
          VerticalOffsetLayout::Builder(LayoutHelper::String(superscript, strlen(superscript), k_layoutFont), VerticalOffsetLayoutNode::Position::Superscript)
        );
    }

    // Exam mode modes
    case I18n::Message::ExamModeModeStandard:
    {
      const char * text = " ";
      return LayoutHelper::String(text, strlen(text), k_layoutFont);
    }
    case I18n::Message::ExamModeModeNoSym:
    {
      const char * text = " ";
      return LayoutHelper::String(text, strlen(text), k_layoutFont);
    }
    case I18n::Message::ExamModeModeNoSymNoText:
    {
      const char * text = " ";
      return LayoutHelper::String(text, strlen(text), k_layoutFont);
    }
    case I18n::Message::ExamModeModeDutch:
    {
      const char * text = " ";
      return LayoutHelper::String(text, strlen(text), k_layoutFont);
    }


    // Symbol controller
    case I18n::Message::SymbolMultiplicationCross: // × and · aren't single characters, so they cannot be constructed into codepoints..?
    {
      const char * text = "×";
      return LayoutHelper::String(text, strlen(text), k_layoutFont);
    }
    case I18n::Message::SymbolMultiplicationMiddleDot:
    {
      const char * text = "·";
      return LayoutHelper::String(text, strlen(text), k_layoutFont);
    }
    case I18n::Message::SymbolMultiplicationStar:
      return CodePointLayout::Builder('*', k_layoutFont);
    case I18n::Message::SymbolMultiplicationAutoSymbol:
      return CodePointLayout::Builder(' ', k_layoutFont);


    // Symbol function
    case I18n::Message::SymbolDefaultFunction:
    {
      return NthRootLayout::Builder(CodePointLayout::Builder('x'));
    }
    case I18n::Message::SymbolArgDefaultFunction:
    {
      return NthRootLayout::Builder(CodePointLayout::Builder('x'), CodePointLayout::Builder('2'));
    }
    case I18n::Message::SymbolArgFunction:
    {
      return NthRootLayout::Builder(CodePointLayout::Builder('x'), CodePointLayout::Builder('y'));
    }

    // Font size
    case I18n::Message::LargeFont:
    case I18n::Message::SmallFont:
    {
      const char * text = "abc";
      const KDFont * font = message == I18n::Message::LargeFont ? KDFont::LargeFont : KDFont::SmallFont;
      return LayoutHelper::String(text, strlen(text), font);
    }

    default:
      assert(false);
      return Layout();
  }
}

void PreferencesController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);
  MessageTableCellWithExpression * myCell = (MessageTableCellWithExpression *)cell;
  myCell->setLayout(layoutForPreferences(m_messageTreeModel->childAtIndex(index)->label()));
}

KDCoordinate PreferencesController::rowHeight(int j) {
  /* We cheat for the Writing format subcontroller, because the Edition2D layout
   * needs more vertical space. */
  return GenericSubController::rowHeight(j) + (m_messageTreeModel->label() == I18n::Message::EditionMode ? 2 : 0);
}

void PreferencesController::setPreferenceWithValueIndex(I18n::Message message, int valueIndex) {
  Preferences * preferences = Preferences::sharedPreferences();
  if (message == I18n::Message::AngleUnit) {
    preferences->setAngleUnit((Preferences::AngleUnit)valueIndex);
  } else if (message == I18n::Message::DisplayMode) {
    Preferences::PrintFloatMode mode = (Preferences::PrintFloatMode)valueIndex;
    preferences->setDisplayMode(mode);
    if (mode == Preferences::PrintFloatMode::Engineering) {
      /* In Engineering mode, the number of significant digits cannot be lower
       * than 3, because we need to be able to display 100 for instance. */
      // TODO: Add warning about signifiant digits change ?
      preferences->setNumberOfSignificantDigits(std::max<int>(preferences->numberOfSignificantDigits(), 3));
    }
  } else if (message == I18n::Message::EditionMode) {
    preferences->setEditionMode((Preferences::EditionMode)valueIndex);
  } else if (message == I18n::Message::ComplexFormat) {
    preferences->setComplexFormat((Preferences::ComplexFormat)valueIndex);
  } else if (message == I18n::Message::ExamModeMode) {
    GlobalPreferences::sharedGlobalPreferences()->setTempExamMode((GlobalPreferences::ExamMode)((uint8_t)valueIndex + 1));
  } else if (message == I18n::Message::SymbolMultiplication) {
    preferences->setSymbolMultiplication((Preferences::SymbolMultiplication)valueIndex);
  } else if (message == I18n::Message::SymbolFunction) {
    preferences->setSymbolOfFunction((Preferences::SymbolFunction)valueIndex);
  } else if (message == I18n::Message::FontSizes) {
    GlobalPreferences::sharedGlobalPreferences()->setFont(valueIndex == 0 ? KDFont::LargeFont : KDFont::SmallFont);
  }
}

int PreferencesController::valueIndexForPreference(I18n::Message message) const {
  Preferences * preferences = Preferences::sharedPreferences();
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
  if (message == I18n::Message::SymbolMultiplication) {
    return (int)preferences->symbolOfMultiplication();
  }
  if (message == I18n::Message::SymbolFunction) {
    return (int)preferences->symbolOfFunction();
  }
  if (message == I18n::Message::FontSizes) {
    return GlobalPreferences::sharedGlobalPreferences()->font() == KDFont::LargeFont ? 0 : 1;
  }
  return 0;
}

}
