#include <assert.h>
#include <escher/i18n.h>
#include <escher/modal_view_empty_controller.h>

namespace Escher {

void ModalViewEmptyController::ModalViewEmptyView::drawRect(KDContext* ctx,
                                                            KDRect rect) const {
  ctx->fillRect(bounds(), k_backgroundColor);
  drawBorderOfRect(ctx, bounds(), Palette::GrayBright);
}

void ModalViewEmptyController::ModalViewEmptyView::layoutSubviews(bool force) {
  setChildFrame(&m_messageTextView, bounds(), force);
}

}  // namespace Escher
