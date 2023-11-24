#include "range_parameter_controller.h"

#include "poincare_helpers.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

// RangeParameterController

RangeParameterController::RangeParameterController(
    Responder *parentResponder, InteractiveCurveViewRange *interactiveRange)
    : ExplicitSelectableListViewController(parentResponder),
      m_interactiveRange(interactiveRange),
      m_tempInteractiveRange(*interactiveRange),
      m_okButton(
          &m_selectableListView, I18n::Message::Ok,
          Invocation::Builder<RangeParameterController>(
              [](RangeParameterController *parameterController, void *sender) {
                parameterController->buttonAction();
                return true;
              },
              this)),
      m_confirmPopUpController(Invocation::Builder<RangeParameterController>(
          [](RangeParameterController *controller, void *sender) {
            controller->stackController()->pop();
            return true;
          },
          this)),
      m_singleInteractiveCurveViewRangeController(
          parentResponder, &m_tempInteractiveRange, &m_confirmPopUpController) {
  m_normalizeCell.label()->setMessage(I18n::Message::MakeOrthonormal);
  m_xRangeCell.label()->setMessage(I18n::Message::ValuesOfX);
  m_yRangeCell.label()->setMessage(I18n::Message::ValuesOfY);
}

HighlightCell *RangeParameterController::cell(int row) {
  assert(row < numberOfRows());
  HighlightCell *cells[] = {&m_normalizeCell, &m_xRangeCell, &m_yRangeCell,
                            &m_okButton};
  return cells[row];
}

void RangeParameterController::fillRangeCells() {
  constexpr int precision = Preferences::VeryLargeNumberOfSignificantDigits;
  constexpr int bufferSize =
      2 * PrintFloat::charSizeForFloatsWithPrecision(precision) + 4;
  char buffer[bufferSize];
  for (int i = 0; i < 2; ++i) {
    bool isAuto = i == 0 ? m_tempInteractiveRange.xAuto()
                         : m_tempInteractiveRange.yAuto();
    if (isAuto) {
      strlcpy(buffer, I18n::translate(I18n::Message::DefaultSetting),
              bufferSize);
    } else {
      float min = i == 0 ? m_tempInteractiveRange.xMin()
                         : m_tempInteractiveRange.yMin();
      float max = i == 0 ? m_tempInteractiveRange.xMax()
                         : m_tempInteractiveRange.yMax();
      int numberOfChars = PoincareHelpers::ConvertFloatToTextWithDisplayMode(
          min, buffer, bufferSize, precision,
          Preferences::PrintFloatMode::Decimal);
      buffer[numberOfChars++] = ' ';
      buffer[numberOfChars++] = ';';
      buffer[numberOfChars++] = ' ';
      numberOfChars += PoincareHelpers::ConvertFloatToTextWithDisplayMode(
          max, buffer + numberOfChars, bufferSize - numberOfChars, precision,
          Preferences::PrintFloatMode::Decimal);
      buffer[numberOfChars++] = '\0';
    }
    RangeCell *cell = i == 0 ? &m_xRangeCell : &m_yRangeCell;
    cell->subLabel()->setText(buffer);
  }
}

KDCoordinate RangeParameterController::separatorBeforeRow(int row) {
  HighlightCell *cell = this->cell(row);
  return cell == &m_xRangeCell || cell == &m_okButton ? k_defaultRowSeparator
                                                      : 0;
}

void RangeParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_normalizeCell.setVisible(!m_tempInteractiveRange.zoomNormalize());
  fillRangeCells();
  if (selectedRow() == -1) {
    selectRow(0);
  } else {
    /* If the table has not been deselected, it means we come from the
     * SingleRangeController. */
    int row = m_singleInteractiveCurveViewRangeController.editXRange() ? 1 : 2;
    selectRow(row);
  }
  m_selectableListView.reloadData();
}

void RangeParameterController::viewDidDisappear() {
  if (!stackController()) {
    m_selectableListView.deselectTable();
  }
}

bool RangeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back &&
      (m_interactiveRange->rangeChecksum() !=
           m_tempInteractiveRange.rangeChecksum() ||
       m_interactiveRange->xAuto() != m_tempInteractiveRange.xAuto() ||
       m_interactiveRange->yAuto() != m_tempInteractiveRange.yAuto())) {
    // Open pop-up to confirm discarding values
    m_confirmPopUpController.presentModally();
    return true;
  }
  HighlightCell *cell = this->cell(selectedRow());
  if (cell == &m_normalizeCell &&
      m_normalizeCell.canBeActivatedByEvent(event)) {
    m_normalizeCell.setHighlighted(false);
    m_tempInteractiveRange.normalize();
    buttonAction();
    return true;
  }
  if ((cell == &m_xRangeCell || cell == &m_yRangeCell) &&
      static_cast<RangeCell *>(cell)->canBeActivatedByEvent(event)) {
    m_singleInteractiveCurveViewRangeController.setEditXRange(cell ==
                                                              &m_xRangeCell);
    stackController()->push(&m_singleInteractiveCurveViewRangeController);
    return true;
  }
  return false;
}

void RangeParameterController::setRange(InteractiveCurveViewRange *range) {
  m_interactiveRange = range;
  m_tempInteractiveRange = *range;
}

void RangeParameterController::buttonAction() {
  /* Deselect the table before denormalizing, as it would mess up the index by
   * adding a new row. */
  m_selectableListView.deselectTable();

  /* Use setZoomAuto to refresh the Auto button on the graph. */
  m_interactiveRange->setZoomAuto(m_tempInteractiveRange.zoomAuto());
  m_interactiveRange->setZoomNormalize(m_tempInteractiveRange.zoomNormalize());
  *m_interactiveRange = m_tempInteractiveRange;

  stackController()->pop();
}

}  // namespace Shared
