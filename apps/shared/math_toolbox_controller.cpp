#include "math_toolbox_controller.h"

#include <apps/apps_container.h>
#include <apps/global_preferences.h>
#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/exception_checkpoint.h>
#include <string.h>

#include "global_context.h"
#include "toolbox_helpers.h"

using namespace Poincare;
using namespace Escher;

namespace Shared {

// Actual toolbox content is in math_toolbox_content.cpp
extern const ToolboxMessage listsStatsFork[];
extern const ToolboxMessage arithmeticFork[];
extern const ToolboxMessage binomialFork[];
extern const ToolboxMessage permuteFork[];
extern const ToolboxMessage logFork[];
extern const ToolboxMessage toolboxModel;

MathToolboxController::MathToolboxController()
    : Toolbox(nullptr, rootModel()->label()), m_extraCellsDataSource(nullptr) {}

const ToolboxMessageTree *MathToolboxController::rootModel() const {
  return toolboxModel.toMessageTree();
}

bool MathToolboxController::handleEvent(Ion::Events::Event event) {
  const int rowIndex = selectedRow();
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) &&
      rowIndex < numberOfExtraCellsInCurrentMenu()) {
    return selectExtraCell(rowIndex);
  }
  return Toolbox::handleEvent(event);
}

void MathToolboxController::viewDidDisappear() {
  Toolbox::viewDidDisappear();
  /* NestedMenuController::viewDidDisappear might need cell heights, which would
   * use the MathToolboxController cell heights memoization. We thus reset the
   * MathToolboxController layouts only after calling the parent's
   * viewDidDisappear. */

  // Tidy the layouts displayed in the MathToolboxController to clean Pool
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].label()->setLayout(Layout());
  }
}

int MathToolboxController::numberOfRows() const {
  return Toolbox::numberOfRows() + numberOfExtraCellsInCurrentMenu();
}

int MathToolboxController::typeAtRow(int row) const {
  if (row < numberOfExtraCellsInCurrentMenu()) {
    return k_leafCellType;
  }
  return Escher::Toolbox::typeAtRow(row);
}

KDCoordinate MathToolboxController::nonMemoizedRowHeight(int row) {
  if (typeAtRow(row) == k_leafCellType) {
    LeafCell tempCell;
    return protectedNonMemoizedRowHeight(&tempCell, row);
  }
  return Escher::Toolbox::nonMemoizedRowHeight(row);
}

bool MathToolboxController::isMessageTreeDisabled(
    const ToolboxMessageTree *messageTree) const {
  I18n::Message label = messageTree->label();
  ExamMode examMode = Preferences::SharedPreferences()->examMode();
  return (messageTree->text() == I18n::Message::Sum && examMode.forbidSum()) ||
         (label == I18n::Message::LogCommandWithArg &&
          examMode.forbidBasedLogarithm()) ||
         (label == I18n::Message::NormVectorCommandWithArg &&
          examMode.forbidVectorNorm()) ||
         ((label == I18n::Message::DotCommandWithArg ||
           label == I18n::Message::CrossCommandWithArg) &&
          examMode.forbidVectorProduct()) ||
         (label == I18n::Message::UnitAndConstant && examMode.forbidUnits());
}

bool MathToolboxController::displayMessageTreeDisabledPopUp(
    const Escher::ToolboxMessageTree *messageTree) {
  if (isMessageTreeDisabled(messageTree)) {
    // TODO: It would be better if warning did not dismiss the toolbox
    App::app()->displayWarning(I18n::Message::DisabledFeatureInTestMode);
    return true;
  }
  return false;
}

void MathToolboxController::fillCellForRow(HighlightCell *cell, int row) {
  if (row < numberOfExtraCellsInCurrentMenu()) {
    assert(m_extraCellsDataSource);
    static_cast<LeafCell *>(cell)->label()->setLayout(
        m_extraCellsDataSource->extraCellLayoutAtRow(row));
    static_cast<LeafCell *>(cell)->subLabel()->setMessage(
        I18n::Message::Default);
    cell->reloadCell();
    return;
  }
  const ToolboxMessageTree *messageTree = messageTreeModelAtIndex(row);
  KDColor textColor =
      isMessageTreeDisabled(messageTree) ? Palette::GrayDark : KDColorBlack;
  if (typeAtRow(row) == k_leafCellType) {
    assert(static_cast<void *>(m_nodeCells) > static_cast<void *>(cell) ||
           static_cast<void *>(cell) >=
               static_cast<void *>(m_nodeCells + k_maxNumberOfDisplayedRows));
    // Message is leaf
    LeafCell *myCell = static_cast<LeafCell *>(cell);

    Layout resultLayout = messageTree->layout();

    if (resultLayout.isUninitialized()) {
      const char *text = I18n::translate(messageTree->label());

      if (Poincare::Preferences::SharedPreferences()->editionMode() ==
          Poincare::Preferences::EditionMode::Edition2D) {
        // No context is given so that f(x) is never parsed as f×(x)
        Expression resultExpression = Expression::Parse(text, nullptr);
        if (!resultExpression.isUninitialized()) {
          // The text is parsable, we create its layout an insert it.
          resultLayout = resultExpression.createLayout(
              Poincare::Preferences::SharedPreferences()->displayMode(),
              Poincare::PrintFloat::k_maxNumberOfSignificantDigits,
              App::app()->localContext());
        }
      }
      if (resultLayout.isUninitialized()) {
        // If 2D parsing failed or edition is in 1D, try a simpler layout
        resultLayout = Layout::String(text, strlen(text));
      }
    }

    myCell->label()->setLayout(resultLayout);
    myCell->subLabel()->setMessage(messageTree->text());
    myCell->label()->setTextColor(textColor);
  } else {
    assert(typeAtRow(row) == k_nodeCellType);
    assert(static_cast<void *>(m_leafCells) > static_cast<void *>(cell) ||
           static_cast<void *>(cell) >=
               static_cast<void *>(m_leafCells + k_maxNumberOfDisplayedRows));
    static_cast<NestedMenuController::NodeCell *>(cell)->label()->setTextColor(
        textColor);
    Escher::Toolbox::fillCellForRow(cell, row);
  }
}

bool MathToolboxController::selectSubMenu(int selectedRow) {
  return displayMessageTreeDisabledPopUp(
             messageTreeModelAtIndex(selectedRow)) ||
         Toolbox::selectSubMenu(selectedRow);
}

bool MathToolboxController::selectLeaf(int selectedRow) {
  assert(typeAtRow(selectedRow) == k_leafCellType);
  const ToolboxMessageTree *messageTree = messageTreeModelAtIndex(selectedRow);
  if (displayMessageTreeDisabledPopUp(messageTree)) {
    return true;
  }

  Layout layout = messageTree->layout();
  if (!layout.isUninitialized()) {
    layout = layout.cloneWithoutChildrenRacks();
    App::app()->modalViewController()->dismissModal();
    sender()->handleEventWithLayout(layout);
    return true;
  }

  // Translate the message
  const char *text = I18n::translate(messageTree->insertedText());
  // Has to be in the same scope as handleEventWithText
  char textToInsert[k_maxMessageSize];
  if (messageTree->stripInsertedText()) {
    int maxTextToInsertLength = strlen(text) + 1;
    assert(maxTextToInsertLength <= k_maxMessageSize);
    ToolboxHelpers::TextToInsertForCommandText(text, -1, textToInsert,
                                               maxTextToInsertLength, true);
    text = textToInsert;
  }
  App::app()->modalViewController()->dismissModal();
  sender()->handleEventWithText(text);
  return true;
}

bool MathToolboxController::selectExtraCell(int selectedRow) {
  assert(m_extraCellsDataSource);
  Layout l = m_extraCellsDataSource->extraCellLayoutAtRow(selectedRow);
  char buffer[k_maxSizeOfExtraCellExpression];
  l.serializeForParsing(
      buffer, k_maxSizeOfExtraCellExpression);  // No need of context here
  App::app()->modalViewController()->dismissModal();
  sender()->handleEventWithText(buffer);
  return true;
}

MathToolboxController::LeafCell *MathToolboxController::leafCellAtIndex(
    int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

Escher::NestedMenuController::NodeCell *MathToolboxController::nodeCellAtIndex(
    int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_nodeCells[index];
}

const Escher::ToolboxMessageTree *
MathToolboxController::messageTreeModelAtIndex(int index) const {
  assert(index >= numberOfExtraCellsInCurrentMenu() &&
         index - numberOfExtraCellsInCurrentMenu() <
             m_messageTreeModel->numberOfChildren());
  return Toolbox::messageTreeModelAtIndex(index -
                                          numberOfExtraCellsInCurrentMenu());
}

int MathToolboxController::maxNumberOfDisplayedRows() const {
  return k_maxNumberOfDisplayedRows;
}

int MathToolboxController::controlChecksum() const {
  return static_cast<int>(
             Preferences::SharedPreferences()->examMode().ruleset()) *
             I18n::NumberOfCountries +
         static_cast<int>(
             GlobalPreferences::SharedGlobalPreferences()->country()) +
         +(m_extraCellsDataSource ? m_extraCellsDataSource->numberOfExtraCells()
                                  : 0) *
             static_cast<int>(Ion::ExamMode::Ruleset::NumberOfRulesets) *
             I18n::NumberOfCountries;
}

int MathToolboxController::indexAfterFork(
    const ToolboxMessageTree *forkMessageTree) const {
  if (forkMessageTree->childrenList() == arithmeticFork) {
    if (Poincare::Preferences::SharedPreferences()
            ->mixedFractionsAreEnabled()) {
      return 0;
    }
    return 1;
  }
  if (forkMessageTree->childrenList() == listsStatsFork) {
    if (GlobalPreferences::SharedGlobalPreferences()
            ->listsStatsOrderInToolbox() ==
        CountryPreferences::ListsStatsOrderInToolbox::Alternate) {
      return 1;
    }
    return 0;
  }
  if (forkMessageTree->childrenList() == logFork) {
    if (GlobalPreferences::SharedGlobalPreferences()->logarithmBasePosition() ==
        Preferences::LogarithmBasePosition::TopLeft) {
      return 1;
    }
    return 0;
  }
  if (forkMessageTree->childrenList() == binomialFork ||
      forkMessageTree->childrenList() == permuteFork) {
    if (GlobalPreferences::SharedGlobalPreferences()->combinatoricsSymbols() ==
        Preferences::CombinatoricSymbols::LetterWithSubAndSuperscript) {
      return 1;
    }
    return 0;
  }
  Preferences::UnitFormat unitFormat =
      GlobalPreferences::SharedGlobalPreferences()->unitFormat();
  if (unitFormat == Preferences::UnitFormat::Metric) {
    return 0;
  }
  assert(unitFormat == Preferences::UnitFormat::Imperial);
  return 1;
}

int MathToolboxController::numberOfExtraCellsInCurrentMenu() const {
  return (m_extraCellsDataSource && m_messageTreeModel == rootModel())
             ? m_extraCellsDataSource->numberOfExtraCells()
             : 0;
}

}  // namespace Shared
