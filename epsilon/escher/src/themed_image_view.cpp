#include <escher/theme/color_picker.h>
#include <escher/themed_image_view.h>
#include <omg/memory.h>
#include <omg/unreachable.h>

extern "C" {
#include <assert.h>
}
#include <ion.h>

namespace Escher {

void ThemedImageView::drawRect(KDContext* ctx, KDRect rect) const {
  // TODO: do not duplicate with ImageView
  const Image* image = currentImage();
  if (image == nullptr) {
    return;
  }
  assert(bounds().width() == image->width());
  assert(bounds().height() == image->height());

  int imageSize = image->width() * image->height();
  assert(imageSize <= k_maxImageSize);
  KDColor pixelBuffer[k_maxImageSize];
  // That's a VERY big buffer we're allocating on the stack
  assert(Ion::stackSafe());

  OMG::Memory::Decompress(
      image->compressedPixelData(), reinterpret_cast<uint8_t*>(pixelBuffer),
      image->compressedPixelDataSize(), imageSize * sizeof(KDColor));

  ctx->fillRectWithPixels(bounds(), pixelBuffer, nullptr);
}

KDSize ThemedImageView::minimalSizeForOptimalDisplay() const {
  return m_themedImage.light()
             ? KDSize(m_themedImage.width(), m_themedImage.height())
             : KDSizeZero;
}

const Image* ThemedImageView::currentImage() const {
  switch (SharedPreferences->theme()) {
    case Theme::Light:
      return m_themedImage.light();
    case Theme::Dark:
      return m_themedImage.dark();
    default:
      OMG_UNREACHABLE
  }
}

}  // namespace Escher
