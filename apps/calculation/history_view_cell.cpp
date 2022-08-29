#include "history_view_cell.h"
#include "app.h"
#include "../constant.h"
#include "selectable_table_view.h"
#include <poincare/exception_checkpoint.h>
#include <assert.h>
#include <string.h>
#include <algorithm>

using namespace Escher;

namespace Calculation {

/* HistoryViewCellDataSource */

void HistoryViewCellDataSource::setSelectedSubviewType(SubviewType subviewType, bool sameCell, int previousSelectedCellX, int previousSelectedCellY) {
  HistoryViewCell * selectedCell = nullptr;
  HistoryViewCell * previouslySelectedCell = nullptr;
  SubviewType previousSubviewType = m_selectedSubviewType;
  m_selectedSubviewType = subviewType;
  /* We need to notify the whole table that the selection changed if it
   * involves the selection/deselection of an output. Indeed, only them can
   * trigger change in the displayed expressions. */
  historyViewCellDidChangeSelection(&selectedCell, &previouslySelectedCell, previousSelectedCellX, previousSelectedCellY, subviewType, previousSubviewType);

  previousSubviewType = sameCell ? previousSubviewType : SubviewType::None;
  if (selectedCell) {
    selectedCell->reloadSubviewHighlight();
    selectedCell->cellDidSelectSubview(subviewType, previousSubviewType);
    Container::activeApp()->setFirstResponder(selectedCell);
  }
  if (previouslySelectedCell) {
    previouslySelectedCell->cellDidSelectSubview(SubviewType::Input);
  }
}

/* HistoryViewCell */

KDCoordinate HistoryViewCell::Height(Calculation * calculation, Poincare::Context * context, bool expanded) {
  HistoryViewCell cell(nullptr);
  cell.setCalculation(calculation, expanded, context, true);
  KDRect ellipsisFrame = KDRectZero;
  KDRect inputFrame = KDRectZero;
  KDRect outputFrame = KDRectZero;
  cell.computeSubviewFrames(Ion::Display::Width, KDCOORDINATE_MAX, &ellipsisFrame, &inputFrame, &outputFrame);
  return k_margin + inputFrame.unionedWith(outputFrame).height() + k_margin;
}

HistoryViewCell::HistoryViewCell(Responder * parentResponder) :
  Responder(parentResponder),
  m_calculationCRC32(0),
  m_calculationDisplayOutput(Calculation::DisplayOutput::Unknown),
  m_calculationAdditionInformation(Calculation::AdditionalInformationType::None),
  m_inputView(this, k_inputViewHorizontalMargin, k_inputOutputViewsVerticalMargin),
  m_scrollableOutputView(this),
  m_calculationExpanded(false),
  m_calculationSingleLine(false)
{
}

void HistoryViewCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_inputView.setBackgroundColor(backgroundColor());
  m_scrollableOutputView.setBackgroundColor(backgroundColor());
  m_scrollableOutputView.evenOddCell()->setEven(even);
  m_ellipsis.setEven(even);
}

void HistoryViewCell::setHighlighted(bool highlight) {
  if (isHighlighted() == highlight) {
    return;
  }
  setHighlightedWitoutReload(highlight);
  reloadSubviewHighlight();
  // Re-layout as the ellispsis subview might have appear/disappear
  layoutSubviews();
}

void HistoryViewCell::reloadSubviewHighlight() {
  assert(m_dataSource);
  m_inputView.setExpressionBackgroundColor(backgroundColor());
  m_scrollableOutputView.evenOddCell()->setHighlighted(false);
  m_ellipsis.setHighlighted(false);
  if (isHighlighted()) {
    if (m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) {
      m_inputView.setExpressionBackgroundColor(Palette::Select);
    } else if (m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Output) {
      m_scrollableOutputView.evenOddCell()->setHighlighted(true);
    } else {
      assert(m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Ellipsis);
      m_ellipsis.setHighlighted(true);
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

void HistoryViewCell::reloadOutputSelection(HistoryViewCellDataSource::SubviewType previousType) {
  /* Select the right output according to the calculation display output. This
   * will reload the scroll to display the selected output. */
  if (m_calculationDisplayOutput == Calculation::DisplayOutput::ExactAndApproximate) {
    m_scrollableOutputView.setSelectedSubviewPosition(
        previousType == HistoryViewCellDataSource::SubviewType::Ellipsis ?
          Shared::ScrollableTwoExpressionsView::SubviewPosition::Right :
          Shared::ScrollableTwoExpressionsView::SubviewPosition::Center
        );
  } else {
    assert((m_calculationDisplayOutput == Calculation::DisplayOutput::ApproximateOnly)
        || (m_calculationDisplayOutput == Calculation::DisplayOutput::ExactAndApproximateToggle)
        || (m_calculationDisplayOutput == Calculation::DisplayOutput::ExactOnly));
    m_scrollableOutputView.setSelectedSubviewPosition(Shared::ScrollableTwoExpressionsView::SubviewPosition::Right);
  }
}

void HistoryViewCell::cellDidSelectSubview(HistoryViewCellDataSource::SubviewType type, HistoryViewCellDataSource::SubviewType previousType) {
  // Init output selection
  if (type == HistoryViewCellDataSource::SubviewType::Output) {
    reloadOutputSelection(previousType);
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

bool HistoryViewCell::ViewsCanBeSingleLine(KDCoordinate inputViewWidth, KDCoordinate outputViewWidth, bool ellipsis) {
  // k_margin is the separation between the input and output.
  return (inputViewWidth + k_margin + outputViewWidth) < Ion::Display::Width - (ellipsis ? Metric::EllipsisCellWidth : 0);
}

void HistoryViewCell::layoutSubviews(bool force) {
  KDRect frameBounds = bounds();
  if (bounds().width() <= 0 || bounds().height() <= 0) {
    // TODO Make this behaviour in a non-virtual layoutSublviews, and all layout subviews should become privateLayoutSubviews
    return;
  }
  KDRect ellipsisFrame = KDRectZero;
  KDRect inputFrame = KDRectZero;
  KDRect outputFrame = KDRectZero;
  computeSubviewFrames(frameBounds.width(), frameBounds.height(), &ellipsisFrame, &inputFrame, &outputFrame);

  m_ellipsis.setFrame(ellipsisFrame, force); // Required even if ellipsisFrame is KDRectZero, to mark previous rect as dirty
  m_inputView.setFrame(inputFrame,force);
  m_scrollableOutputView.setFrame(outputFrame, force);
}

void HistoryViewCell::computeSubviewFrames(KDCoordinate frameWidth, KDCoordinate frameHeight, KDRect * ellipsisFrame, KDRect * inputFrame, KDRect * outputFrame) {
  assert(ellipsisFrame != nullptr && inputFrame != nullptr && outputFrame != nullptr);

  if (displayedEllipsis()) {
    *ellipsisFrame = KDRect(frameWidth - Metric::EllipsisCellWidth, 0, Metric::EllipsisCellWidth, frameHeight);
    frameWidth -= Metric::EllipsisCellWidth;
  } else {
    *ellipsisFrame = KDRectZero;
  }

  KDSize inputSize = m_inputView.minimalSizeForOptimalDisplay();
  KDSize outputSize = m_scrollableOutputView.minimalSizeForOptimalDisplay();

  /* To compute if the calculation is on a single line, use the expanded width
   * if there is both an exact and an approximate layout. */
  m_calculationSingleLine = ViewsCanBeSingleLine(inputSize.width(), m_scrollableOutputView.minimalSizeForOptimalDisplayFullSize().width(), m_calculationAdditionInformation != Calculation::AdditionalInformationType::None);

  KDCoordinate inputY = k_margin;
  KDCoordinate outputY = k_margin;
  if (m_calculationSingleLine && !m_inputView.layout().isUninitialized()) {
    KDCoordinate inputBaseline = m_inputView.layout().baseline(m_inputView.font());
    KDCoordinate outputBaseline = m_scrollableOutputView.baseline();
    KDCoordinate baselineDifference = outputBaseline - inputBaseline;
    if (baselineDifference > 0) {
      inputY += baselineDifference;
    } else {
      outputY += -baselineDifference;
    }
  } else {
    outputY += inputSize.height();
  }

  *inputFrame = KDRect(
        0,
        inputY,
        std::min(frameWidth, inputSize.width()),
        inputSize.height());
  *outputFrame = KDRect(
        std::max(0, frameWidth - outputSize.width()),
        outputY,
        std::min(frameWidth, outputSize.width()),
        outputSize.height());
}

void HistoryViewCell::resetMemoization() {
  // Clean the layouts to make room in the pool
  // TODO: maybe do this only when the layout won't change to avoid blinking
  m_inputView.setLayout(Poincare::Layout());
  m_scrollableOutputView.setLayouts(Poincare::Layout(), Poincare::Layout(), Poincare::Layout());
  m_calculationCRC32 = 0;
}

void HistoryViewCell::setCalculation(Calculation * calculation, bool expanded, Poincare::Context * context, bool canChangeDisplayOutput) {
  if (m_calculationExpanded != expanded) {
    // Change expanded if needed
    m_calculationExpanded = expanded && calculation->displayOutput(context) == ::Calculation::Calculation::DisplayOutput::ExactAndApproximateToggle;
    m_scrollableOutputView.setDisplayCenter(m_calculationDisplayOutput == Calculation::DisplayOutput::ExactAndApproximate || m_calculationExpanded);
  }

  uint32_t newCalculationCRC = Ion::crc32Byte((const uint8_t *)calculation, ((char *)calculation->next()) - ((char *) calculation));
  if (newCalculationCRC == m_calculationCRC32) {
    return;
  }

  // TODO: maybe do this only when the layout won't change to avoid blinking
  resetMemoization();

  // Memoization
  m_calculationCRC32 = newCalculationCRC;
  m_calculationAdditionInformation = calculation->additionalInformationType();
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
      if (canChangeDisplayOutput && calculation->displayOutput(context) != ::Calculation::Calculation::DisplayOutput::ExactOnly) {
        calculation->forceDisplayOutput(::Calculation::Calculation::DisplayOutput::ApproximateOnly);
      } else {
        /* We should only display the exact result, but we cannot create it
         * -> raise an exception. */
        Poincare::ExceptionCheckpoint::Raise();
      }
    }
    KDCoordinate maxVisibleWidth = Ion::Display::Width - (
        m_scrollableOutputView.leftMargin()
      + m_scrollableOutputView.rightMargin()
      + 2 * KDFont::GlyphWidth(m_scrollableOutputView.font())); // > arrow and = sign
    if (canChangeDisplayOutput
     && calculation->displayOutput(context) == ::Calculation::Calculation::DisplayOutput::ExactAndApproximate
     && exactOutputLayout.layoutSize(m_scrollableOutputView.font()).width() > maxVisibleWidth)
    {
      calculation->forceDisplayOutput(::Calculation::Calculation::DisplayOutput::ExactAndApproximateToggle);
    }
  }

  // Create the approximate output layout
  Poincare::Layout approximateOutputLayout;
  if (calculation->displayOutput(context) == ::Calculation::Calculation::DisplayOutput::ExactOnly) {
    approximateOutputLayout = exactOutputLayout;
  } else {
    bool couldNotCreateApproximateLayout = false;
    approximateOutputLayout = calculation->createApproximateOutputLayout(&couldNotCreateApproximateLayout);
    if (couldNotCreateApproximateLayout) {
      if (canChangeDisplayOutput && calculation->displayOutput(context) != ::Calculation::Calculation::DisplayOutput::ApproximateOnly) {
        /* Set the display output to ApproximateOnly, make room in the pool by
         * erasing the exact layout, and retry to create the approximate layout */
        calculation->forceDisplayOutput(::Calculation::Calculation::DisplayOutput::ApproximateOnly);
        exactOutputLayout = Poincare::Layout();
        couldNotCreateApproximateLayout = false;
        approximateOutputLayout = calculation->createApproximateOutputLayout(&couldNotCreateApproximateLayout);
        if (couldNotCreateApproximateLayout) {
          Poincare::ExceptionCheckpoint::Raise();
        }
      } else {
        Poincare::ExceptionCheckpoint::Raise();
      }
    }
  }
  m_calculationDisplayOutput = calculation->displayOutput(context);

  // We must set which subviews are displayed before setLayouts to mark the right rectangle as dirty
  m_scrollableOutputView.setDisplayableCenter(m_calculationDisplayOutput == Calculation::DisplayOutput::ExactAndApproximate || m_calculationDisplayOutput == Calculation::DisplayOutput::ExactAndApproximateToggle);
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
  } else if (m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Output) {
    Container::activeApp()->setFirstResponder(&m_scrollableOutputView);
  }
}

bool HistoryViewCell::handleEvent(Ion::Events::Event event) {
  assert(m_dataSource != nullptr);
  HistoryViewCellDataSource::SubviewType type = m_dataSource->selectedSubviewType();
  assert(type != HistoryViewCellDataSource::SubviewType::None);
  HistoryViewCellDataSource::SubviewType otherSubviewType = HistoryViewCellDataSource::SubviewType::None;
  if (m_calculationSingleLine) {
    static_assert(
        static_cast<int>(HistoryViewCellDataSource::SubviewType::None) == 0
        && static_cast<int>(HistoryViewCellDataSource::SubviewType::Input) == 1
        && static_cast<int>(HistoryViewCellDataSource::SubviewType::Output) == 2
        && static_cast<int>(HistoryViewCellDataSource::SubviewType::Ellipsis) == 3,
        "The array types is not well-formed anymore");
    HistoryViewCellDataSource::SubviewType types[] = {
      HistoryViewCellDataSource::SubviewType::None,
      HistoryViewCellDataSource::SubviewType::Input,
      HistoryViewCellDataSource::SubviewType::Output,
      displayedEllipsis() ? HistoryViewCellDataSource::SubviewType::Ellipsis : HistoryViewCellDataSource::SubviewType::None,
      HistoryViewCellDataSource::SubviewType::None,
    };
    if (event == Ion::Events::Right || event == Ion::Events::Left) {
      otherSubviewType = types[static_cast<int>(type) + (event == Ion::Events::Right ? 1 : -1)];
    }
  } else if ((event == Ion::Events::Down && type == HistoryViewCellDataSource::SubviewType::Input)
      || (event == Ion::Events::Left && type == HistoryViewCellDataSource::SubviewType::Ellipsis))
  {
    otherSubviewType = HistoryViewCellDataSource::SubviewType::Output;
  } else if (event == Ion::Events::Up && type == HistoryViewCellDataSource::SubviewType::Output) {
    otherSubviewType = HistoryViewCellDataSource::SubviewType::Input;
  } else if (event == Ion::Events::Right && type != HistoryViewCellDataSource::SubviewType::Ellipsis && displayedEllipsis()) {
    otherSubviewType = HistoryViewCellDataSource::SubviewType::Ellipsis;
  }
  if (otherSubviewType == HistoryViewCellDataSource::SubviewType::None) {
    return false;
  }
  m_dataSource->setSelectedSubviewType(otherSubviewType, true);
  return true;
}

}
