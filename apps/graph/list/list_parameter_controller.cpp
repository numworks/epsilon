#include "list_parameter_controller.h"
#include "list_controller.h"
#include "type_helper.h"
#include "../../shared/poincare_helpers.h"
#include "../app.h"
#include <escher/metric.h>
#include <assert.h>

using namespace Shared;
using namespace Poincare;

namespace Graph {

ListParameterController::ListParameterController(ListController * listController, Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  Shared::ListParameterController(parentResponder, functionColorMessage, deleteFunctionMessage),
  m_listController(listController),
  m_typeCell(),
  m_typeParameterController(this),
  m_domainParameterController(nullptr, inputEventHandlerDelegate),
  m_renameCell(I18n::Message::Rename)
{
  m_selectableTableView.setMargins(Metric::CommonTopMargin, Metric::CommonTopMargin, Metric::CommonBottomMargin, Metric::CommonTopMargin); // Reduce the margins to make te text fit
}

HighlightCell * ListParameterController::reusableCell(int index, int type) {
  switch (type) {
  case 0:
    return &m_typeCell;
  case 1:
    return &m_functionDomain;
  case 2:
    return &m_renameCell;
  default:
    return Shared::ListParameterController::reusableCell(index, type - 3);
  }
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  if (Shared::ListParameterController::handleEvent(event)) {
    return true;
  }
  if (event == Ion::Events::Right) {
    int selectedR = selectedRow();
    if (selectedR == 0 || selectedR == 1 || selectedR == 3) {
      // Go in the submenu
      return handleEnterOnRow(selectedR);
    }
  }
  return false;
}

char intervalBracket(double value, bool opening) {
  return std::isinf(value) == opening ? ']' : '[';
}

int writeInterval(char * buffer, int bufferSize, double min, double max, int numberOfSignificantDigits, Preferences::PrintFloatMode mode) {
  int numberOfChar = 0;
  assert(bufferSize-1 > numberOfChar);
  buffer[numberOfChar++] = intervalBracket(min, true);
  int glyphLengthRequiredForFloat = PrintFloat::glyphLengthForFloatWithPrecision(numberOfSignificantDigits);
  PrintFloat::TextLengths minLengths = PrintFloat::ConvertFloatToText<double>(min, buffer+numberOfChar, bufferSize - numberOfChar, glyphLengthRequiredForFloat, numberOfSignificantDigits, mode);
  numberOfChar += minLengths.CharLength;
  assert(bufferSize > numberOfChar);
  numberOfChar += strlcpy(buffer+numberOfChar, ",", bufferSize-numberOfChar);
  PrintFloat::TextLengths maxLengths = PrintFloat::ConvertFloatToText<double>(max, buffer+numberOfChar, bufferSize - numberOfChar, glyphLengthRequiredForFloat, numberOfSignificantDigits, mode);
  numberOfChar += maxLengths.CharLength;
  assert(bufferSize-1 > numberOfChar);
  buffer[numberOfChar++] = intervalBracket(max, false);
  assert(bufferSize > numberOfChar);
  strlcpy(buffer+numberOfChar, " ", bufferSize-numberOfChar);
  return minLengths.GlyphLength + maxLengths. GlyphLength + 3 + 1; // Count "[,] " glyphs
}

void ListParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  Shared::ListParameterController::willDisplayCellForIndex(cell, index);
  if ((cell == &m_typeCell || cell == &m_functionDomain) && !m_record.isNull()) {
    App * myApp = App::app();
    assert(!m_record.isNull());
    Shared::ExpiringPointer<Shared::ContinuousFunction> function = myApp->functionStore()->modelForRecord(m_record);
    if (cell == &m_typeCell) {
      m_typeCell.setMessage(I18n::Message::CurveType);
      int row = static_cast<int>(function->plotType());
      m_typeCell.setSubtitle(PlotTypeHelper::Message(row));
    } else {
      assert(cell == &m_functionDomain);
      m_functionDomain.setMessage(I18n::Message::FunctionDomain);
      double min = function->tMin();
      double max = function->tMax();
      constexpr int bufferSize = BufferTextView::k_maxNumberOfChar;
      char buffer[bufferSize];
      int glyphLength = writeInterval(buffer, bufferSize, min, max, Preferences::VeryShortNumberOfSignificantDigits, Preferences::sharedPreferences()->displayMode());
      int numberOfAvailableGlyphs = (m_functionDomain.bounds().width() - m_functionDomain.labelView()->bounds().width() - m_functionDomain.accessoryView()->bounds().width() - 2*Metric::TableCellHorizontalMargin)/KDFont::SmallFont->glyphSize().width();
      if (glyphLength > numberOfAvailableGlyphs) {
        writeInterval(buffer, bufferSize, min, max, Preferences::VeryShortNumberOfSignificantDigits-1, Preferences::PrintFloatMode::Scientific);
      }
      m_functionDomain.setAccessoryText(buffer);
    }
  }
}

bool ListParameterController::handleEnterOnRow(int rowIndex) {
  StackViewController * stack = (StackViewController *)(parentResponder());
  switch (rowIndex) {
  case 0:
    m_typeParameterController.setRecord(m_record);
    stack->push(&m_typeParameterController);
    return true;
  case 1:
    m_domainParameterController.setRecord(m_record);
    stack->push(&m_domainParameterController);
    return true;
  case 2:
    renameFunction();
    return true;
  default:
    return Shared::ListParameterController::handleEnterOnRow(rowIndex - 3);
  }
}

void ListParameterController::renameFunction() {
  // Set editing true on function title
  StackViewController * stack = (StackViewController *)(parentResponder());
  stack->pop();
  m_listController->renameSelectedFunction();
}

}
