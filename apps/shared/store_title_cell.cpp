#include "store_title_cell.h"
#include "hideable_even_odd_editable_text_cell.h"

namespace Shared {

void StoreTitleCell::setSeparatorRight(bool separator) {
  if (separatorRight() != separator) {
    StoreSeparatorCell::setSeparatorRight(separator);
    reloadCell();
  }
}

void StoreTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  BufferFunctionTitleCell::drawRect(ctx, rect);
  // Draw the separator
  KDRect separatorRect(bounds().width() - StoreSeparatorCell::k_separatorThickness, separatorRight() ? 0 : k_colorIndicatorThickness, StoreSeparatorCell::k_separatorThickness, bounds().height());
  if (separatorRight()) {
    ctx->fillRect(separatorRect, HideableEvenOddEditableTextCell::hideColor());
  } else {
    ctx->fillRect(separatorRect, backgroundColor());
  }
}

void StoreTitleCell::layoutSubviews() {
  KDRect textFrame = bufferTextViewFrame();
  bufferTextView()->setFrame(KDRect(textFrame.topLeft(), textFrame.width() - StoreSeparatorCell::k_separatorThickness, textFrame.height() ));
}

}
