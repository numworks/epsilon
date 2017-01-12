#include "banner_view.h"
#include <string.h>
#include <assert.h>

void BannerView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
}

void BannerView::setLegendAtIndex(char * text, int index) {
  TextView * textView = textViewAtIndex(index);
  textView->setText(text);
}

KDSize BannerView::minimalSizeForOptimalDisplay() {
  return KDSize(0, KDText::stringSize(" ").height()*numberOfLines());
}

int BannerView::numberOfSubviews() const {
  return 0;
}

void BannerView::layoutSubviews() {
  markRectAsDirty(bounds());
  /* We iterate on subviews, adding their width until we exceed the view bound.
  * The last subview that exceeds the bound is recorded as the first subview of
  * the next line. For the current line, we scan again the subviews and frame
  * them by equally distributing the remaining width. We then jump to the next
  * line and iterate the process. */
  KDCoordinate totalWidth = bounds().width();
  KDCoordinate lineWidth = 0;
  TextView * textViewPreviousLine = textViewAtIndex(0);
  int indexOfFirstViewOfCurrentLine = 0;
  KDCoordinate y = 0;
  /* We do a last iteration of the loop to layout the last line. */
  for (int i = 0; i <= numberOfSubviews(); i++) {
    TextView * textView = nullptr;
    KDCoordinate currentViewWidth = totalWidth;
    if (i < numberOfSubviews()) {
      textView = textViewAtIndex(i);
      currentViewWidth = textView->minimalSizeForOptimalDisplay().width();
    }
    // The subview exceed the total width
    if (lineWidth + currentViewWidth > totalWidth) {
      KDCoordinate x = 0;
      for (int j = indexOfFirstViewOfCurrentLine; j < i; j++) {
        textViewPreviousLine = textViewAtIndex(j);
        KDCoordinate textWidth = textViewPreviousLine->minimalSizeForOptimalDisplay().width() + (totalWidth - lineWidth)/(i-indexOfFirstViewOfCurrentLine);
        KDCoordinate textHeight = textViewPreviousLine->minimalSizeForOptimalDisplay().height();
        textViewPreviousLine->setFrame(KDRect(x, y, textWidth, textHeight));
        x += textWidth;
      }
      // Next line
      y += textViewPreviousLine->minimalSizeForOptimalDisplay().height();
      lineWidth = 0;
      indexOfFirstViewOfCurrentLine = i;
    }
    lineWidth += currentViewWidth;
  }
}

View * BannerView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  return textViewAtIndex(index);
}

int BannerView::numberOfLines() {
  KDCoordinate width = bounds().width();
  KDCoordinate usedWidth = 0;
  KDCoordinate lineNumber = 0;
  for (int i = 0; i < numberOfSubviews(); i++) {
    KDCoordinate textWidth = KDText::stringSize(textViewAtIndex(i)->text()).width();
    if (usedWidth+textWidth > width) {
      usedWidth = textWidth;
      lineNumber++;
    } else {
      usedWidth += textWidth;
    }
  }
  return lineNumber+1;
}
