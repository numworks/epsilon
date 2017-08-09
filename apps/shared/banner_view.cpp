#include "banner_view.h"
#include <string.h>
#include <assert.h>

namespace Shared {

void BannerView::setLegendAtIndex(char * text, int index) {
  /* The layout of the banner's subviews depends on their content.
   * Indeed, we're using a "centered text" algorithm to layout the subviews.
   * So changing a legend implies two things
   *  - First, we need to update the textView to ensure it has the new content
   *  - Second, we need to relayout *all* of our subviews. */
  TextView * textView = textViewAtIndex(index);
  textView->setText(text);
  layoutSubviews();
}

void BannerView::setMessageAtIndex(I18n::Message text, int index) {
  MessageTextView * textView = messageTextViewAtIndex(index);
  textView->setMessage(text);
  layoutSubviews();
}

KDSize BannerView::minimalSizeForOptimalDisplay() const {
  return KDSize(0, KDText::charSize(KDText::FontSize::Small).height()*numberOfLines());
}

int BannerView::numberOfSubviews() const {
  return 0;
}

void BannerView::layoutSubviews() {
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
      int nbOfTextViewInLine = i > indexOfFirstViewOfCurrentLine ? i-indexOfFirstViewOfCurrentLine : 1;
      KDCoordinate roundingError = totalWidth-lineWidth-nbOfTextViewInLine*(int)((totalWidth-lineWidth)/nbOfTextViewInLine);
      for (int j = indexOfFirstViewOfCurrentLine; j < i; j++) {
        textViewPreviousLine = textViewAtIndex(j);
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

View * BannerView::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  return textViewAtIndex(index);
}

int BannerView::numberOfLines() const {
  KDCoordinate width = bounds().width();
  KDCoordinate usedWidth = 0;
  KDCoordinate lineNumber = 0;
  for (int i = 0; i < numberOfSubviews(); i++) {
    KDCoordinate textWidth = KDText::stringSize(textViewAtIndex(i)->text(), KDText::FontSize::Small).width();
    if (usedWidth+textWidth > width) {
      usedWidth = textWidth;
      lineNumber++;
    } else {
      usedWidth += textWidth;
    }
  }
  return lineNumber+1;
}

MessageTextView * BannerView::messageTextViewAtIndex(int i) const {
  return nullptr;
}

}
