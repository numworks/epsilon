#include "banner_view.h"
#include <assert.h>

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
  int indexOfFirstViewOfCurrentLine = 0;
  KDCoordinate y = LineSpacing;
  /* We do a last iteration of the loop to layout the last line. */
  for (int i = 0; i <= numberOfSubviews(); i++) {
    KDCoordinate subviewWidth = (i < numberOfSubviews()) ? subviewAtIndex(i)->minimalSizeForOptimalDisplay().width() : lineWidth;
    if (subviewWidth > remainingWidth) {
      KDCoordinate x = 0;
      int nbOfSubviewsOnLine = i > indexOfFirstViewOfCurrentLine ? i-indexOfFirstViewOfCurrentLine : 1;
      KDCoordinate roundingError = remainingWidth % nbOfSubviewsOnLine;
      View * subviewPreviousLine = nullptr;
      for (int j = indexOfFirstViewOfCurrentLine; j < i; j++) {
        subviewPreviousLine = subviewAtIndex(j);
        KDCoordinate width = subviewPreviousLine->minimalSizeForOptimalDisplay().width() + remainingWidth/nbOfSubviewsOnLine + (j == i-1) * roundingError;
        KDCoordinate height = subviewPreviousLine->minimalSizeForOptimalDisplay().height();
        subviewPreviousLine->setFrame(KDRect(x, y, width, height), force);
        x += width;
      }
      // Next line
      assert(subviewPreviousLine);
      y += subviewPreviousLine->minimalSizeForOptimalDisplay().height() + LineSpacing;
      remainingWidth = lineWidth;
      indexOfFirstViewOfCurrentLine = i;
    }
    remainingWidth -= subviewWidth;
  }
}

int BannerView::numberOfLinesGivenWidth(KDCoordinate width) const {
  int lineNumber = 1;
  const KDCoordinate lineWidth = width;
  KDCoordinate remainingWidth = lineWidth;
  for (int i = 0; i < numberOfSubviews(); i++) {
    KDCoordinate subviewWidth = const_cast<Shared::BannerView *>(this)->subviewAtIndex(i)->minimalSizeForOptimalDisplay().width();
    if (subviewWidth > remainingWidth) {
      remainingWidth = lineWidth;
      lineNumber++;
    }
    remainingWidth -= subviewWidth;
  }
  return lineNumber;
}

}
