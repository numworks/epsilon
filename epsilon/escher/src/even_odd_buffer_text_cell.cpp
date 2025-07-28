#include <assert.h>
#include <escher/even_odd_buffer_text_cell.h>

namespace Escher {

void AbstractEvenOddBufferTextCell::
    updateSubviewsBackgroundAfterChangingState() {
  bufferTextView()->setBackgroundColor(backgroundColor());
}

int AbstractEvenOddBufferTextCell::numberOfSubviews() const { return 1; }

View* AbstractEvenOddBufferTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return bufferTextView();
}

void AbstractEvenOddBufferTextCell::layoutSubviews(bool force) {
  KDRect boundsThis = bounds();
  KDRect boundsBuffer =
      KDRect(boundsThis.left() + k_horizontalMargin, boundsThis.top(),
             boundsThis.width() - 2 * k_horizontalMargin, boundsThis.height());
  setChildFrame(bufferTextView(), boundsBuffer, force);
}

}  // namespace Escher
