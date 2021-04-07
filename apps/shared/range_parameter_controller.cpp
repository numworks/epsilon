#include "range_parameter_controller.h"
#include "poincare_helpers.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

// RangeParameterController

RangeParameterController::RangeParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * interactiveRange) :
  SelectableListViewController(parentResponder),
  m_interactiveRange(interactiveRange),
  m_tempInteractiveRange(*interactiveRange),
  m_rangeCells{},
  m_okButton(&m_selectableTableView, I18n::Message::Ok, Invocation([](void * context, void * sender) {
    RangeParameterController * parameterController = static_cast<RangeParameterController *>(context);
    parameterController->buttonAction();
    return true;
  }, this)),
  m_confirmPopUpController(Invocation([](void * context, void * sender) {
    Container::activeApp()->dismissModalViewController();
    ((RangeParameterController *)context)->stackController()->pop();
    return true;
  }, this)),
  m_singleRangeController(parentResponder, inputEventHandlerDelegate, &m_tempInteractiveRange)
{}

int RangeParameterController::typeAtIndex(int index) {
  /* FIXME: Give names to the types. */
  int types[] = {0, 1, 1, 2};
  return types[index + !displayNormalizeCell()];
}

int RangeParameterController::reusableCellCount(int type) {
  if (type == 1) {
    return k_numberOfRangeCells;
  } else {
    return 1;
  }
}

HighlightCell * RangeParameterController::reusableCell(int index, int type) {
  if (type == 0) {
    assert(index == 0);
    return &m_normalizeCell;
  } else if (type == 1) {
    assert(index < k_numberOfRangeCells);
    return m_rangeCells + index;
  } else {
    assert(type == 2);
    assert(index == 0);
    return &m_okButton;
  }
}

KDCoordinate RangeParameterController::nonMemoizedRowHeight(int j) {
  HighlightCell * cells[] = {&m_normalizeCell, m_rangeCells, m_rangeCells + 1, &m_okButton};
  assert(j < numberOfRows());
  return heightForCellAtIndex(cells[j + !displayNormalizeCell()], j, false);
}

void RangeParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  int i = index - displayNormalizeCell();
  if (i  >= 0 && i < k_numberOfRangeCells) {
    MessageTableCellWithChevronAndBuffer * castedCell = static_cast<MessageTableCellWithChevronAndBuffer *>(cell);
    float min, max;
    if (i == 0) {
      castedCell->setMessage(I18n::Message::ValuesOfX);
      min = m_tempInteractiveRange.xMin();
      max = m_tempInteractiveRange.xMax();
    } else {
      assert(i == 1);
      castedCell->setMessage(I18n::Message::ValuesOfY);
      min = m_tempInteractiveRange.yMin();
      max = m_tempInteractiveRange.yMax();
    }
    constexpr int precision = Preferences::LargeNumberOfSignificantDigits;
    constexpr int bufferSize = 2 * PrintFloat::charSizeForFloatsWithPrecision(precision) + 4;
    char buffer[bufferSize];
    int numberOfChars = PoincareHelpers::ConvertFloatToTextWithDisplayMode(min, buffer, bufferSize, precision, Preferences::PrintFloatMode::Decimal);
    buffer[numberOfChars++] = ' ';
    buffer[numberOfChars++] = ';';
    buffer[numberOfChars++] = ' ';
    numberOfChars += PoincareHelpers::ConvertFloatToTextWithDisplayMode(max, buffer + numberOfChars, bufferSize - numberOfChars, precision, Preferences::PrintFloatMode::Decimal);
    buffer[numberOfChars++] = '\0';
    castedCell->setSubLabelText(buffer);
  }
  SelectableListViewController::willDisplayCellForIndex(cell, index);
}

void RangeParameterController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void RangeParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  resetMemoization();
  m_selectableTableView.reloadData();
}

bool RangeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back && m_interactiveRange->rangeChecksum() != m_tempInteractiveRange.rangeChecksum()) {
    // Open pop-up to confirm discarding values
    Container::activeApp()->displayModalViewController(&m_confirmPopUpController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpRightMargin, Metric::PopUpBottomMargin, Metric::PopUpLeftMargin);
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (displayNormalizeCell() && selectedRow() == 0) {
      m_interactiveRange->normalize();
      stackController()->pop();
      return true;
    }
    int index = selectedRow() - displayNormalizeCell();
    if (index < k_numberOfRangeCells) {
      m_singleRangeController.setEditXRange(index == 0);
      stackController()->push(&m_singleRangeController);
      return true;
    }
  }
  return false;
}

void RangeParameterController::setRange(InteractiveCurveViewRange * range) {
  m_interactiveRange = range;
  m_tempInteractiveRange = *range;
}

void RangeParameterController::buttonAction() {
  /* Deselect the table before denormalizing, as it would mess up the index by
   * adding a new row. */
  m_selectableTableView.deselectTable();

  *m_interactiveRange = m_tempInteractiveRange;
  m_interactiveRange->setZoomAuto(false);
  m_interactiveRange->setZoomNormalize(m_interactiveRange->isOrthonormal());

  stackController()->pop();
}

// RangeParameterController::NormalizeCell

KDSize RangeParameterController::NormalizeCell::minimalSizeForOptimalDisplay() const {
  m_cell.setSize(bounds().size());
  KDSize cellSize = m_cell.minimalSizeForOptimalDisplay();
  // An additional border is required after separator (and will be overlapped)
  return KDSize(cellSize.width(), cellSize.height() + k_margin + k_lineThickness);
}

// RangeParameterController::SingleRangeController

RangeParameterController::SingleRangeController::SingleRangeController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * interactiveRange) :
  FloatParameterController<float>(parentResponder),
  m_boundsCells{},
  m_range(interactiveRange)
{
  for (int i = 0; i < k_numberOfTextCells; i++) {
    m_boundsCells[i].setParentResponder(&m_selectableTableView);
    m_boundsCells[i].textField()->setDelegates(inputEventHandlerDelegate, this);
  }
}

void RangeParameterController::SingleRangeController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (index < k_numberOfTextCells) {
    MessageTableCellWithEditableText * castedCell = static_cast<MessageTableCellWithEditableText *>(cell);
    castedCell->setMessage(index == 0 ? I18n::Message::Minimum : I18n::Message::Maximum);
  }
  FloatParameterController<float>::willDisplayCellForIndex(cell, index);
}

float RangeParameterController::SingleRangeController::parameterAtIndex(int index) {
  ParameterGetterPointer getters[] = { &InteractiveCurveViewRange::yMin, &InteractiveCurveViewRange::yMax, &InteractiveCurveViewRange::xMin, &InteractiveCurveViewRange::xMax };
  return (m_range->*getters[index + 2 * m_editXRange])();
}

bool RangeParameterController::SingleRangeController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex < k_numberOfTextCells);
  ParameterSetterPointer setters[] = { &InteractiveCurveViewRange::setYMin, &InteractiveCurveViewRange::setYMax, &InteractiveCurveViewRange::setXMin, &InteractiveCurveViewRange::setXMax };
  (m_range->*setters[parameterIndex + 2 * m_editXRange])(f);
  return true;
}

}
