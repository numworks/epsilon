#include "even_odd_buffer_text_cell_with_margin.h"
#include <assert.h>

namespace Regression {

void EvenOddBufferTextCellWithMargin::layoutSubviews() {
  KDRect boundsThis = bounds();
  KDRect boundsBuffer = KDRect(boundsThis.left() + k_horizontalMargin, boundsThis.top(), boundsThis.width() - 2*k_horizontalMargin, boundsThis.height());
  m_bufferTextView.setFrame(boundsBuffer);
}

}
