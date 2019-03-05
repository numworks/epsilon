#include "banner_view.h"
#include <string.h>

namespace Shared {

KDCoordinate BannerView::HeightGivenNumberOfLines(int linesCount) {
  return KDFont::SmallFont->glyphSize().height()*linesCount;
}

KDSize BannerView::minimalSizeForOptimalDisplay() const {
  return KDSize(0, HeightGivenNumberOfLines(numberOfLines()));
}

void BannerView::layoutSubviews() {
  /* We iterate on subviews, adding their width until we exceed the view bound.
  * The last subview that exceeds the bound is recorded as the first subview of
  * the next line. For the current line, we scan again the subviews and frame
  * them by equally distributing the remaining width. We then jump to the next
  * line and iterate the process. */
  KDCoordinate totalWidth = bounds().width();
  KDCoordinate lineWidth = 0;
  View * textViewPreviousLine = subviewAtIndex(0);
  int indexOfFirstViewOfCurrentLine = 0;
  KDCoordinate y = 0;
  /* We do a last iteration of the loop to layout the last line. */
  for (int i = 0; i <= numberOfSubviews(); i++) {
    View * textView = nullptr;
    KDCoordinate currentViewWidth = totalWidth;
    if (i < numberOfSubviews()) {
      textView = subviewAtIndex(i);
      currentViewWidth = textView->minimalSizeForOptimalDisplay().width();
    }
    // The subview exceed the total width
    if (lineWidth + currentViewWidth > totalWidth) {
      KDCoordinate x = 0;
      int nbOfTextViewInLine = i > indexOfFirstViewOfCurrentLine ? i-indexOfFirstViewOfCurrentLine : 1;
      KDCoordinate roundingError = totalWidth-lineWidth-nbOfTextViewInLine*(int)((totalWidth-lineWidth)/nbOfTextViewInLine);
      for (int j = indexOfFirstViewOfCurrentLine; j < i; j++) {
        textViewPreviousLine = subviewAtIndex(j);
        KDCoordinate textWidth = textViewPreviousLine->minimalSizeForOptimalDisplay().width() + (totalWidth - lineWidth)/nbOfTextViewInLine;
        // For the last text view, avoid letting a 1-pixel-wide empty vertical due to rounding error:
        textWidth = j == i-1 ? textWidth + roundingError : textWidth;
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

int BannerView::numberOfLines() const {
  KDCoordinate width = bounds().width();
  KDCoordinate usedWidth = 0;
  KDCoordinate lineNumber = 0;
  for (int i = 0; i < numberOfSubviews(); i++) {
    KDCoordinate textWidth = const_cast<Shared::BannerView *>(this)->subviewAtIndex(i)->minimalSizeForOptimalDisplay().width();
    if (usedWidth+textWidth > width) {
      usedWidth = textWidth;
      lineNumber++;
    } else {
      usedWidth += textWidth;
    }
  }
  return lineNumber+1;
}

}
