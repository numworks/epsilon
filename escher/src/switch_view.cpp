#include <escher/switch_view.h>

SwitchView::SwitchView() :
ChildlessView()
{
}

void SwitchView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();

  ctx->fillRect(KDRect(0, 0, width, 1), Palette::LineColor);
  ctx->fillRect(KDRect(0, 1, 1, height-1), Palette::LineColor);
  ctx->fillRect(KDRect(1, height-1, width-1, 1), Palette::LineColor);
  ctx->fillRect(KDRect(width-1, 1, 1, height-1), Palette::LineColor);
  ctx->fillRect(KDRect(1, 1, width/3, height-2), KDColorBlack);
  ctx->fillRect(KDRect(width/3+1, 1, width-2-width/3, height-2), KDColorGreen);
}
