#include <escher/background_view.h>
#include <ion.h>
#include <kandinsky/ion_context.h>
#include <escher/palette.h>//AND THIS
#include "apps/home/controller.h"
#ifdef HOME_DISPLAY_EXTERNALS
#include "apps/external/external_icon.h"
#include "apps/external/archive.h"
#include <string.h>
#endif

//To store the Omega backgrounds, we use a specific file in the "OmegaBitMap" format.
//Here is its header
struct OBMHeader
{
  uint32_t signature; //Normally it is 32145
  int32_t width;
  int32_t height;
  const KDColor image_data;
};

BackgroundView::BackgroundView():
  m_data(nullptr),
  m_isDataValid(false),
  m_defaultColor(Palette::BackgroundHard)
{

}

void BackgroundView::setBackgroundImage(const uint8_t * data) {
  m_data = data;
  updateDataValidity();
  markRectAsDirty(bounds());
}

void BackgroundView::setDefaultColor(KDColor defaultColor) {
  m_defaultColor = defaultColor;
}

void BackgroundView::drawRect(KDContext * ctx, KDRect rect) const {
  if(!m_isDataValid) {
    ctx->fillRect(rect, m_defaultColor);
    return;
  }

  OBMHeader* h = (OBMHeader*)m_data;
  
  int yrectToImage = ctx->origin().y() - m_frame.y();
  int xrectToImage = ctx->origin().x() - m_frame.x();

  for (int line = rect.y(); line <= rect.bottom(); line++) {
    int offset = ((line + yrectToImage) * h->width) + (rect.x() + xrectToImage);
    ctx->fillRectWithPixels(KDRect(rect.x(), line, rect.width(), 1), &(h->image_data) + offset, nullptr);
  }
}

void BackgroundView::updateDataValidity() {
  if(m_data == nullptr || KDIonContext::sharedContext()->gammaEnabled) {
    m_isDataValid = false;
    return;
  }
  
  OBMHeader* h = (OBMHeader*)m_data;
  m_isDataValid = h->signature==466512775 && h->height==m_frame.height() && h->width==m_frame.width();
}