#include "banner_view.h"

#include <assert.h>
#include <ion.h>

namespace Shared {

KDCoordinate BannerView::HeightGivenNumberOfLines(int linesCount) {
  return LineSpacing + (KDFont::GlyphHeight(k_font) + LineSpacing) * linesCount;
}

void BannerView::drawRect(KDContext *ctx, KDRect rect) const {
  assert(!bounds().isEmpty());
  const KDCoordinate frameHeight = bounds().height();
  const KDCoordinate lineHeight = KDFont::GlyphHeight(k_font) + LineSpacing;
  const KDCoordinate lineWidth = m_frame.width();
  for (KDCoordinate y = 0; y < frameHeight; y += lineHeight) {
    ctx->fillRect(KDRect(0, y, lineWidth, LineSpacing), BackgroundColor());
  }
}

KDSize BannerView::minimalSizeForOptimalDisplay() const {
  return KDSize(Ion::Display::Width,
                minimalHeightForOptimalDisplayGivenWidth(Ion::Display::Width));
}

KDCoordinate BannerView::minimalHeightForOptimalDisplayGivenWidth(
    KDCoordinate width) const {
  return HeightGivenNumberOfLines(numberOfLinesGivenWidth(width));
}

void BannerView::layoutSubviews(bool force) {
  if (m_frame.isEmpty()) {
    /* If the frame has not been set yet, there is no point in layouting the
     * subviews.
     * TODO: Do not call layout Subviews if the frame has not been set? */
    return;
  }
  const KDCoordinate width = m_frame.width();
  int firstSubview = 0;
  KDCoordinate y = LineSpacing;
  while (firstSubview < numberOfSubviews()) {
    KDCoordinate remainingWidth;
    int numberOfSubviewsCurrentLine =
        numberOfSubviewsOnOneLine(firstSubview, width, &remainingWidth);
    KDCoordinate roundingError = remainingWidth % numberOfSubviewsCurrentLine;
    KDCoordinate spacePerSubview = remainingWidth / numberOfSubviewsCurrentLine;
    KDCoordinate x = 0;
    KDCoordinate maxHeight = 0;
    for (int i = firstSubview; i < firstSubview + numberOfSubviewsCurrentLine;
         i++) {
      assert(i < numberOfSubviews());
      KDSize size = subviewAtIndex(i)->minimalSizeForOptimalDisplay();
      KDCoordinate width =
          size.width() + k_minimalSpaceBetweenSubviews + spacePerSubview +
          (i == firstSubview + numberOfSubviewsCurrentLine - 1) * roundingError;
      if (size.height() > maxHeight) {
        maxHeight = size.height();
      }
      /* A subview's structure can change without it being reflected on the
       * frame. As such, we force a relayout. */
      setChildFrame(subviewAtIndex(i), KDRect(x, y, width, size.height()),
                    true);
      x += width;
    }
    y += maxHeight + LineSpacing;
    firstSubview += numberOfSubviewsCurrentLine;
  }
}

int BannerView::numberOfLinesGivenWidth(KDCoordinate width) const {
  int currentSubview = 0;
  int numberOfLines = 0;
  while (currentSubview < numberOfSubviews()) {
    currentSubview += numberOfSubviewsOnOneLine(currentSubview, width);
    numberOfLines++;
  }
  return numberOfLines;
}

int BannerView::numberOfSubviewsOnOneLine(int firstSubview, KDCoordinate width,
                                          KDCoordinate *remainingWidth) const {
  KDCoordinate tempWidth;
  KDCoordinate *lineWidth = remainingWidth ? remainingWidth : &tempWidth;
  *lineWidth = width;
  for (int i = firstSubview; i < numberOfSubviews(); i++) {
    View *subview = const_cast<Shared::BannerView *>(this)->subviewAtIndex(i);
    KDCoordinate subviewWidth =
        subview->minimalSizeForOptimalDisplay().width() +
        k_minimalSpaceBetweenSubviews;
    if (subviewWidth > *lineWidth ||
        (lineBreakBeforeSubview(subview) && i != firstSubview)) {
      if (i == firstSubview) {
        // Subview should fit without k_minimalSpaceBetweenSubviews
        assert(subviewWidth - k_minimalSpaceBetweenSubviews <= *lineWidth);
        *lineWidth -= subviewWidth - k_minimalSpaceBetweenSubviews;
        return 1;
      }
      return i - firstSubview;
    }
    *lineWidth -= subviewWidth;
  }
  return numberOfSubviews() - firstSubview;
}

// BannerView::LabelledView

KDSize BannerView::LabelledView::minimalSizeForOptimalDisplay() const {
  KDSize labelSize = m_labelView->minimalSizeForOptimalDisplay();
  KDSize infoSize = m_infoView->minimalSizeForOptimalDisplay();
  assert(labelSize.height() == infoSize.height());
  return KDSize(labelSize.width() + infoSize.width(), labelSize.height());
}

Escher::View *BannerView::LabelledView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  if (index == 0) {
    return m_labelView;
  }
  return m_infoView;
}

void BannerView::LabelledView::layoutSubviews(bool force) {
  KDSize labelSize = m_labelView->minimalSizeForOptimalDisplay();
  KDSize infoSize = m_infoView->minimalSizeForOptimalDisplay();
  KDCoordinate spacing =
      (bounds().width() - labelSize.width() - infoSize.width()) / 2;
  KDCoordinate labelTotalWidth = spacing + labelSize.width();
  setChildFrame(m_labelView, KDRect(0, 0, labelTotalWidth, bounds().height()),
                force);
  setChildFrame(m_infoView,
                KDRect(labelTotalWidth, 0, bounds().width() - labelTotalWidth,
                       bounds().height()),
                force);
}

}  // namespace Shared
