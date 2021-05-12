#include "banner_view.h"
#include <assert.h>
#include <ion.h>

namespace Shared {

KDCoordinate BannerView::HeightGivenNumberOfLines(int linesCount) {
  return LineSpacing + (Font()->glyphSize().height() + LineSpacing) * linesCount;
}

void BannerView::drawRect(KDContext * ctx, KDRect rect) const {
  const KDCoordinate frameHeight = minimalSizeForOptimalDisplay().height();
  const KDCoordinate lineHeight = Font()->glyphSize().height() + LineSpacing;
  const KDCoordinate lineWidth = m_frame.width();
  for (KDCoordinate y = 0; y < frameHeight; y += lineHeight) {
    ctx->fillRect(KDRect(0, y, lineWidth, LineSpacing), BackgroundColor());
  }
}

KDSize BannerView::minimalSizeForOptimalDisplay() const {
  return KDSize(Ion::Display::Width, minimalHeightForOptimalDisplayGivenWidth(Ion::Display::Width));
}

KDCoordinate BannerView::minimalHeightForOptimalDisplayGivenWidth(KDCoordinate width) const {
  return HeightGivenNumberOfLines(numberOfLinesGivenWidth(width));
}

void BannerView::layoutSubviews(bool force) {
  if (m_frame.isEmpty()) {
    /* If the frame has not been set yet, there is no point in layouting the
     * subviews.
     * TODO: Do not call layout Subviews if the frame has not been set? */
    return;
  }
  /* We iterate on subviews, adding their width until we exceed the view bound.
   * The last subview that exceeds the bound is recorded as the first subview of
   * the next line. For the current line, we scan again the subviews and frame
   * them by equally distributing the remaining width. We then jump to the next
   * line and iterate the process. */
  const KDCoordinate lineWidth = m_frame.width();
  KDCoordinate remainingWidth = lineWidth;
  KDCoordinate mandatorySpacing = k_minimalSpaceBetweenSubviews;
  int indexOfFirstViewOfCurrentLine = 0;
  KDCoordinate y = LineSpacing;
  /* We do a last iteration of the loop to layout the last line. */
  for (int i = 0; i <= numberOfSubviews(); i++) {
    KDCoordinate subviewWidth = i < numberOfSubviews() ? subviewAtIndex(i)->minimalSizeForOptimalDisplay().width() : lineWidth;
    if (subviewWidth + mandatorySpacing > remainingWidth || lineBreakBeforeSubview(subviewAtIndex(i))) {
      KDCoordinate x = 0;
      int nbOfSubviewsOnLine = i > indexOfFirstViewOfCurrentLine ? i-indexOfFirstViewOfCurrentLine : 1;
      KDCoordinate roundingError = remainingWidth % nbOfSubviewsOnLine;
      View * subviewPreviousLine = nullptr;
      for (int j = indexOfFirstViewOfCurrentLine; j < nbOfSubviewsOnLine + indexOfFirstViewOfCurrentLine; j++) {
        subviewPreviousLine = subviewAtIndex(j);
        KDCoordinate width = subviewPreviousLine->minimalSizeForOptimalDisplay().width() + remainingWidth/nbOfSubviewsOnLine + (j == i-1) * roundingError;
        KDCoordinate height = subviewPreviousLine->minimalSizeForOptimalDisplay().height();
        /* A subview's structure can change without it being reflected on the
         * frame. As such, we force a relayout. */
        subviewPreviousLine->setFrame(KDRect(x, y, width, height), true);
        x += width;
      }
      // Next line
      assert(subviewPreviousLine);
      y += subviewPreviousLine->minimalSizeForOptimalDisplay().height() + LineSpacing;
      remainingWidth = lineWidth;
      mandatorySpacing = 0;
      indexOfFirstViewOfCurrentLine = i;
    }
    mandatorySpacing += k_minimalSpaceBetweenSubviews;
    remainingWidth -= subviewWidth;
  }
}

int BannerView::numberOfLinesGivenWidth(KDCoordinate width) const {
  int lineNumber = 1;
  const KDCoordinate lineWidth = width - k_minimalSpaceBetweenSubviews;
  KDCoordinate remainingWidth = lineWidth;
  for (int i = 0; i < numberOfSubviews(); i++) {
    View * subview = const_cast<Shared::BannerView *>(this)->subviewAtIndex(i);
    KDCoordinate subviewWidth = subview->minimalSizeForOptimalDisplay().width();
    if (subviewWidth > remainingWidth || lineBreakBeforeSubview(subview)) {
      remainingWidth = lineWidth;
      lineNumber++;
    }
    remainingWidth -= subviewWidth + k_minimalSpaceBetweenSubviews;
  }
  return lineNumber;
}

// BannerView::LabelledView

KDSize BannerView::LabelledView::minimalSizeForOptimalDisplay() const {
  KDSize labelSize = m_labelView->minimalSizeForOptimalDisplay();
  KDSize infoSize = m_infoView->minimalSizeForOptimalDisplay();
  assert(labelSize.height() == infoSize.height());
  return KDSize(labelSize.width() + infoSize.width(), labelSize.height());
}

Escher::View * BannerView::LabelledView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  if (index == 0) {
    return m_labelView;
  }
  return m_infoView;
}

void BannerView::LabelledView::layoutSubviews(bool force) {
  KDSize labelSize = m_labelView->minimalSizeForOptimalDisplay();
  KDSize infoSize = m_infoView->minimalSizeForOptimalDisplay();
  KDCoordinate spacing = (bounds().width() - labelSize.width() - infoSize.width()) / 2;
  KDCoordinate labelTotalWidth = spacing + labelSize.width();
  m_labelView->setFrame(KDRect(0, 0, labelTotalWidth, bounds().height()), force);
  m_infoView->setFrame(KDRect(labelTotalWidth, 0, bounds().width() - labelTotalWidth, bounds().height()), force);
}

}
