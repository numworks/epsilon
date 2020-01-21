#include "history_view_cell.h"
#include "app.h"
#include "../constant.h"
#include "selectable_table_view.h"
#include <poincare/exception_checkpoint.h>
#include <assert.h>
#include <string.h>

namespace Calculation {

static inline KDCoordinate minCoordinate(KDCoordinate x, KDCoordinate y) { return x < y ? x : y; }
static inline KDCoordinate maxCoordinate(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

/* HistoryViewCellDataSource */

HistoryViewCellDataSource::HistoryViewCellDataSource() :
  m_selectedSubviewType(SubviewType::Output) {}

void HistoryViewCellDataSource::setSelectedSubviewType(SubviewType subviewType, int previousSelectedCellX, int previousSelectedCellY) {
  m_selectedSubviewType = subviewType;
  HistoryViewCell * selectedCell = nullptr;
  HistoryViewCell * previouslySelectedCell = nullptr;
  historyViewCellDidChangeSelection(&selectedCell, &previouslySelectedCell, previousSelectedCellX, previousSelectedCellY);
  if (selectedCell) {
    selectedCell->setHighlighted(selectedCell->isHighlighted());
    selectedCell->cellDidSelectSubview(subviewType);
  }
  if (previouslySelectedCell) {
    previouslySelectedCell->cellDidSelectSubview(SubviewType::Input);
  }
}

/* HistoryViewCell */

HistoryViewCell::HistoryViewCell(Responder * parentResponder) :
  Responder(parentResponder),
  m_calculationDisplayOutput(Calculation::DisplayOutput::Unknown),
  m_calculationAdditionInformation(Calculation::AdditionalInformationType::None),
  m_calculationExpanded(false),
  m_inputView(this, Metric::CommonLargeMargin, Metric::CommonSmallMargin),
  m_scrollableOutputView(this)
{
  m_calculationCRC32 = 0;
}

Shared::ScrollableTwoExpressionsView * HistoryViewCell::outputView() {
  return &m_scrollableOutputView;
}

void HistoryViewCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_inputView.setBackgroundColor(backgroundColor());
  m_scrollableOutputView.setBackgroundColor(backgroundColor());
  m_scrollableOutputView.evenOddCell()->setEven(even);
  m_ellipsis.setEven(even);
}

void HistoryViewCell::setHighlighted(bool highlight) {
  assert(m_dataSource);
  m_highlighted = highlight;
  m_inputView.setExpressionBackgroundColor(backgroundColor());
  m_scrollableOutputView.evenOddCell()->setHighlighted(false);
  m_ellipsis.setHighlighted(false);
  if (isHighlighted()) {
    if (m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) {
      m_inputView.setExpressionBackgroundColor(Palette::Select);
    } else {
      m_scrollableOutputView.evenOddCell()->setHighlighted(true);
    }
  }
}

Poincare::Layout HistoryViewCell::layout() const {
  assert(m_dataSource);
  if (m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) {
    return m_inputView.layout();
  } else {
    return m_scrollableOutputView.layout();
  }
}

void HistoryViewCell::reloadScroll() {
  m_inputView.reloadScroll();
  m_scrollableOutputView.reloadScroll();
}

void HistoryViewCell::reloadOutputSelection() {
  /* Select the right output according to the calculation display output. This
   * will reload the scroll to display the selected output. */
  if (m_calculationDisplayOutput == Calculation::DisplayOutput::ExactAndApproximate) {
    m_scrollableOutputView.setSelectedSubviewPosition(Shared::ScrollableTwoExpressionsView::SubviewPosition::Center);
  } else {
    assert((m_calculationDisplayOutput == Calculation::DisplayOutput::ApproximateOnly)
        || (m_calculationDisplayOutput == Calculation::DisplayOutput::ExactAndApproximateToggle)
        || (m_calculationDisplayOutput == Calculation::DisplayOutput::ExactOnly));
    m_scrollableOutputView.setSelectedSubviewPosition(Shared::ScrollableTwoExpressionsView::SubviewPosition::Right);
  }
}

void HistoryViewCell::cellDidSelectSubview(HistoryViewCellDataSource::SubviewType type) {
  // Init output selection
  if (type == HistoryViewCellDataSource::SubviewType::Output) {
    reloadOutputSelection();
  }

  // Update m_calculationExpanded
  m_calculationExpanded = (type == HistoryViewCellDataSource::SubviewType::Output && m_calculationDisplayOutput == Calculation::DisplayOutput::ExactAndApproximateToggle);
  /* The selected subview has changed. The displayed outputs might have changed.
   * For example, for the calculation 1.2+2 --> 3.2, selecting the output would
   * display 1.2+2 --> 16/5 = 3.2. */
  m_scrollableOutputView.setDisplayCenter(m_calculationDisplayOutput == Calculation::DisplayOutput::ExactAndApproximate || m_calculationExpanded);

  /* The displayed outputs have changed. We need to re-layout the cell
   * and re-initialize the scroll. */
  layoutSubviews();
  reloadScroll();
}

KDColor HistoryViewCell::backgroundColor() const {
  KDColor background = m_even ? KDColorWhite : Palette::WallScreen;
  return background;
}

int HistoryViewCell::numberOfSubviews() const {
  return 2 + displayedEllipsis();
}

View * HistoryViewCell::subviewAtIndex(int index) {
  /* The order of the subviews should not matter here as they don't overlap.
   * However, the order determines the order of redrawing as well. For several
   * reasons listed after, changing subview selection often redraws the entire
   * m_scrollableOutputView even if it seems unecessary:
   * - Before feeding new Layouts to ExpressionViews, we reset the hold layouts
   *   in order to empty the Poincare pool and have more space to compute new
   *   layouts.
   * - Even if we did not do that, ExpressionView::setLayout doesn't avoid
   *   redrawing when the previous expression is identical (for reasons
   *   explained in expression_view.cpp)
   * - Because of the toggling burger view, ExpressionViews often have the same
   *   absolute frame but a different relative frame which leads to redrawing
   *   them anyway.
   * All these reasons cause a blinking which can be avoided if we redraw the
   * output view before the input view (starting with redrawing the more
   * complex view enables to redraw it before the vblank thereby preventing
   * blinking).
   * TODO: this is a dirty hack which should be fixed! */
  View * views[3] = {&m_scrollableOutputView, &m_inputView, &m_ellipsis};
  return views[index];
}

void HistoryViewCell::layoutSubviews(bool force) {
  KDCoordinate maxFrameWidth = bounds().width();
  if (displayedEllipsis()) {
    m_ellipsis.setFrame(KDRect(maxFrameWidth - Metric::EllipsisCellWidth, 0, Metric::EllipsisCellWidth, bounds().height()), force);
    maxFrameWidth -= Metric::EllipsisCellWidth;
  } else {
    m_ellipsis.setFrame(KDRectZero, force); // Required to mark previous rect as dirty
  }
  KDSize inputSize = m_inputView.minimalSizeForOptimalDisplay();
  m_inputView.setFrame(KDRect(
    0, 0,
    minCoordinate(maxFrameWidth, inputSize.width()),
    inputSize.height()),
  force);
  KDSize outputSize = m_scrollableOutputView.minimalSizeForOptimalDisplay();
  m_scrollableOutputView.setFrame(KDRect(
    maxCoordinate(0, maxFrameWidth - outputSize.width()),
    inputSize.height(),
    minCoordinate(maxFrameWidth, outputSize.width()),
    outputSize.height()),
  force);
}

void HistoryViewCell::setCalculation(Calculation * calculation, bool expanded) {
  uint32_t newCalculationCRC = Ion::crc32Byte((const uint8_t *)calculation, ((char *)calculation->next()) - ((char *) calculation));
  if (newCalculationCRC == m_calculationCRC32 && m_calculationExpanded == expanded) {
    return;
  }
  Poincare::Context * context = App::app()->localContext();

  // Clean the layouts to make room in the pool
  // TODO: maybe do this only when the layout won't change to avoid blinking
  m_inputView.setLayout(Poincare::Layout());
  m_scrollableOutputView.setLayouts(Poincare::Layout(), Poincare::Layout(), Poincare::Layout());

  // Memoization
  m_calculationCRC32 = newCalculationCRC;
  m_calculationExpanded = expanded && calculation->displayOutput(context) == ::Calculation::Calculation::DisplayOutput::ExactAndApproximateToggle;
  m_calculationAdditionInformation = calculation->additionalInformationType(context);
  m_inputView.setLayout(calculation->createInputLayout());

  /* All expressions have to be updated at the same time. Otherwise,
   * when updating one layout, if the second one still points to a deleted
   * layout, calling to layoutSubviews() would fail. */

  // Create the exact output layout
  Poincare::Layout exactOutputLayout = Poincare::Layout();
  if (Calculation::DisplaysExact(calculation->displayOutput(context))) {
    bool couldNotCreateExactLayout = false;
    exactOutputLayout = calculation->createExactOutputLayout(&couldNotCreateExactLayout);
    if (couldNotCreateExactLayout) {
      if (calculation->displayOutput(context) != ::Calculation::Calculation::DisplayOutput::ExactOnly) {
        calculation->forceDisplayOutput(::Calculation::Calculation::DisplayOutput::ApproximateOnly);
      } else {
        /* We should only display the exact result, but we cannot create it
         * -> raise an exception. */
        Poincare::ExceptionCheckpoint::Raise();
      }
    }
  }

  // Create the approximate output layout
  Poincare::Layout approximateOutputLayout;
  if (calculation->displayOutput(context) == ::Calculation::Calculation::DisplayOutput::ExactOnly) {
    approximateOutputLayout = exactOutputLayout;
  } else {
    bool couldNotCreateApproximateLayout = false;
    approximateOutputLayout = calculation->createApproximateOutputLayout(context, &couldNotCreateApproximateLayout);
    if (couldNotCreateApproximateLayout) {
      if (calculation->displayOutput(context) == ::Calculation::Calculation::DisplayOutput::ApproximateOnly) {
        Poincare::ExceptionCheckpoint::Raise();
      } else {
        /* Set the display output to ApproximateOnly, make room in the pool by
         * erasing the exact layout, and retry to create the approximate layout */
        calculation->forceDisplayOutput(::Calculation::Calculation::DisplayOutput::ApproximateOnly);
        exactOutputLayout = Poincare::Layout();
        couldNotCreateApproximateLayout = false;
        approximateOutputLayout = calculation->createApproximateOutputLayout(context, &couldNotCreateApproximateLayout);
        if (couldNotCreateApproximateLayout) {
          Poincare::ExceptionCheckpoint::Raise();
        }
      }
    }
  }
  m_calculationDisplayOutput = calculation->displayOutput(context);

  // We must set which subviews are displayed before setLayouts to mark the right rectangle as dirty
  m_scrollableOutputView.setDisplayCenter(m_calculationDisplayOutput == Calculation::DisplayOutput::ExactAndApproximate || m_calculationExpanded);
  m_scrollableOutputView.setLayouts(Poincare::Layout(), exactOutputLayout, approximateOutputLayout);
  I18n::Message equalMessage = calculation->exactAndApproximateDisplayedOutputsAreEqual(context) == Calculation::EqualSign::Equal ? I18n::Message::Equal : I18n::Message::AlmostEqual;
  m_scrollableOutputView.setEqualMessage(equalMessage);

  /* The displayed input and outputs have changed. We need to re-layout the cell
   * and re-initialize the scroll. */
  layoutSubviews();
  reloadScroll();
}

void HistoryViewCell::didBecomeFirstResponder() {
  assert(m_dataSource);
  if (m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) {
    Container::activeApp()->setFirstResponder(&m_inputView);
  } else {
    Container::activeApp()->setFirstResponder(&m_scrollableOutputView);
  }
}

bool HistoryViewCell::handleEvent(Ion::Events::Event event) {
  assert(m_dataSource);
  if ((event == Ion::Events::Down && m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) ||
    (event == Ion::Events::Up && m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Output)) {
    HistoryViewCellDataSource::SubviewType otherSubviewType = m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input ? HistoryViewCellDataSource::SubviewType::Output : HistoryViewCellDataSource::SubviewType::Input;
    m_dataSource->setSelectedSubviewType(otherSubviewType);
    CalculationSelectableTableView * tableView = (CalculationSelectableTableView *)parentResponder();
    tableView->scrollToSubviewOfTypeOfCellAtLocation(otherSubviewType, tableView->selectedColumn(), tableView->selectedRow());
    Container::activeApp()->setFirstResponder(this);
    return true;
  }
  return false;
}

bool HistoryViewCell::displayedEllipsis() const {
  return m_highlighted && m_calculationAdditionInformation != Calculation::AdditionalInformationType::None;
}

}
