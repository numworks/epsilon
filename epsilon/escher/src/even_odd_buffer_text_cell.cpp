#include <assert.h>
#include <escher/even_odd_buffer_text_cell.h>

namespace Escher {

void AbstractEvenOddTableBufferTextCell::setHighlighted(bool highlighted) {
  EvenOddCell::setHighlighted(highlighted);
  bufferTextView()->setHighlighted(highlighted);
}

void AbstractEvenOddTableBufferTextCell::
    updateSubviewsBackgroundAfterChangingState() {
  bufferTextView()->setBackgroundColor(backgroundColor());
}

int AbstractEvenOddTableBufferTextCell::numberOfSubviews() const { return 1; }

View* AbstractEvenOddTableBufferTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return bufferTextView();
}

void AbstractEvenOddTableBufferTextCell::layoutSubviews(bool force) {
  KDRect boundsThis = bounds();
  KDRect boundsBuffer =
      KDRect(boundsThis.left() + k_horizontalMargin, boundsThis.top(),
             boundsThis.width() - 2 * k_horizontalMargin, boundsThis.height());
  setChildFrame(bufferTextView(), boundsBuffer, force);
}

}  // namespace Escher
