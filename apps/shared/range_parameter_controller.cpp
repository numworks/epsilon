#include "range_parameter_controller.h"

#include <omg/utf8_helper.h>

#include "poincare_helpers.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

// RangeParameterController

RangeParameterController::RangeParameterController(
    Responder* parentResponder, InteractiveCurveViewRange* interactiveRange)
    : ExplicitSelectableListViewController(parentResponder),
      m_interactiveRange(interactiveRange),
      m_tempInteractiveRange(*interactiveRange),
      m_okButton(
          &m_selectableListView, I18n::Message::Ok,
          Invocation::Builder<RangeParameterController>(
              [](RangeParameterController* parameterController, void* sender) {
                parameterController->buttonAction();
                return true;
              },
              this),
          ButtonCell::Style::EmbossedLight),
      m_confirmPopUpController(Invocation::Builder<RangeParameterController>(
          [](RangeParameterController* controller, void* sender) {
            controller->stackController()->pop();
            return true;
          },
          this)),
      m_singleInteractiveCurveViewRangeController(
          parentResponder, &m_tempInteractiveRange, &m_confirmPopUpController),
      m_gridTypeController(parentResponder, &m_tempInteractiveRange) {
  m_normalizeCell.label()->setMessage(I18n::Message::MakeOrthonormal);
  m_xRangeCell.label()->setMessage(I18n::Message::ValuesOfX);
  m_yRangeCell.label()->setMessage(I18n::Message::ValuesOfY);
  m_gridTypeCell.label()->setMessage(I18n::Message::GridType);
  m_gridTypeCell.setVisible(false);
}

const HighlightCell* RangeParameterController::cell(int row) const {
  assert(row < numberOfRows());
  const HighlightCell* cells[] = {&m_normalizeCell, &m_xRangeCell,
                                  &m_yRangeCell, &m_gridTypeCell, &m_okButton};
  return cells[row];
}

HighlightCell* RangeParameterController::cell(int row) {
  return const_cast<Escher::HighlightCell*>(
      const_cast<const RangeParameterController*>(this)->cell(row));
}

void RangeParameterController::fillCells() {
  constexpr int precision = Preferences::VeryLargeNumberOfSignificantDigits;
  constexpr size_t bufferSize =
      2 * PrintFloat::charSizeForFloatsWithPrecision(precision) + 4;
  char buffer[bufferSize];
  for (int i = 0; i < 2; ++i) {
    OMG::Axis axis = i == 0 ? OMG::Axis::Horizontal : OMG::Axis::Vertical;
    bool isAuto = m_tempInteractiveRange.zoomAuto(axis);

    if (isAuto) {
      strlcpy(buffer, I18n::translate(I18n::Message::DefaultSetting),
              bufferSize);
    } else {
      float min = axis == OMG::Axis::Horizontal ? m_tempInteractiveRange.xMin()
                                                : m_tempInteractiveRange.yMin();
      float max = axis == OMG::Axis::Horizontal ? m_tempInteractiveRange.xMax()
                                                : m_tempInteractiveRange.yMax();
      int numberOfChars = PoincareHelpers::ConvertFloatToTextWithDisplayMode(
          min, buffer, bufferSize, precision,
          Preferences::PrintFloatMode::Decimal);
      numberOfChars += UTF8Helper::WriteCodePoint(
          buffer + numberOfChars, bufferSize - numberOfChars, ' ');
      numberOfChars += UTF8Helper::WriteCodePoint(
          buffer + numberOfChars, bufferSize - numberOfChars, ';');
      numberOfChars += UTF8Helper::WriteCodePoint(
          buffer + numberOfChars, bufferSize - numberOfChars, ' ');
      numberOfChars += PoincareHelpers::ConvertFloatToTextWithDisplayMode(
          max, buffer + numberOfChars, bufferSize - numberOfChars, precision,
          Preferences::PrintFloatMode::Decimal);
      buffer[numberOfChars++] = 0;
    }
    MenuCell* cell =
        axis == OMG::Axis::Horizontal ? &m_xRangeCell : &m_yRangeCell;
    cell->subLabel()->setText(buffer);
  }

  I18n::Message gridTypeCellSubMessage =
      m_tempInteractiveRange.gridType() ==
              GridTypeController::GridType::Cartesian
          ? I18n::Message::Cartesian
          : I18n::Message::Polar;

  m_gridTypeCell.subLabel()->setText(I18n::translate(gridTypeCellSubMessage));
}

KDCoordinate RangeParameterController::separatorBeforeRow(int row) {
  HighlightCell* cell = this->cell(row);
  return cell == &m_xRangeCell || cell == &m_okButton ? k_defaultRowSeparator
                                                      : 0;
}

void RangeParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_normalizeCell.setVisible(!m_tempInteractiveRange.zoomNormalize());
  fillCells();
  if (selectedRow() == -1) {
    selectRow(0);
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
       m_interactiveRange->zoomAuto(OMG::Axis::Horizontal) !=
           m_tempInteractiveRange.zoomAuto(OMG::Axis::Horizontal) ||
       m_interactiveRange->zoomAuto(OMG::Axis::Vertical) !=
           m_tempInteractiveRange.zoomAuto(OMG::Axis::Vertical) ||
       m_interactiveRange->gridType() != m_tempInteractiveRange.gridType())) {
    // Open pop-up to confirm discarding values
    m_confirmPopUpController.presentModally();
    return true;
  }
  HighlightCell* cell = this->cell(selectedRow());
  if (cell == &m_normalizeCell &&
      m_normalizeCell.canBeActivatedByEvent(event)) {
    m_normalizeCell.setHighlighted(false);
    m_tempInteractiveRange.normalize();
    buttonAction();
    return true;
  }
  if ((cell == &m_xRangeCell || cell == &m_yRangeCell) &&
      static_cast<MenuCell*>(cell)->canBeActivatedByEvent(event)) {
    m_singleInteractiveCurveViewRangeController.setAxis(
        cell == &m_xRangeCell ? OMG::Axis::Horizontal : OMG::Axis::Vertical);
    stackController()->push(&m_singleInteractiveCurveViewRangeController);
    return true;
  }
  if (cell == &m_gridTypeCell &&
      static_cast<MenuCell*>(cell)->canBeActivatedByEvent(event)) {
    stackController()->push(&m_gridTypeController);
    return true;
  }
  return false;
}

void RangeParameterController::setRange(InteractiveCurveViewRange* range) {
  m_interactiveRange = range;
  m_tempInteractiveRange = *range;
}

void RangeParameterController::buttonAction() {
  /* Deselect the table before denormalizing, as it would mess up the index by
   * adding a new row. */
  m_selectableListView.deselectTable();

  /* Use setZoomAuto to refresh the Auto button on the graph. */
  m_interactiveRange->setZoomAuto(m_tempInteractiveRange.zoomAuto());
  m_interactiveRange->setUserGridUnit(m_tempInteractiveRange.userGridUnit());
  m_interactiveRange->setZoomNormalize(m_tempInteractiveRange.zoomNormalize());
  *m_interactiveRange = m_tempInteractiveRange;

  stackController()->pop();
}

}  // namespace Shared
