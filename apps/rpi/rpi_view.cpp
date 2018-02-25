#include "rpi_view.h"
#include <assert.h>

namespace Rpi {

RpiView::RpiView() :
  View()
{
}

void RpiView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColor::RGB24(0x808080));
}

}
