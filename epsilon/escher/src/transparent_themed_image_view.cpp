#include <escher/theme/color_picker.h>
#include <escher/theme/highlight.h>
#include <escher/transparent_themed_image_view.h>
#include <ion.h>
#include <omg/memory.h>

namespace Escher {

void TransparentThemedImageView::drawRect(KDContext* ctx, KDRect rect) const {
  const Image* image = currentImage();
  if (image == nullptr) {
    return;
  }
  if (!image->hasTransparency()) {
    ThemedImageView::drawRect(ctx, rect);
    return;
  }

  assert(bounds().width() == image->width());
  assert(bounds().height() == image->height());

  int imageSize = image->width() * image->height();
  assert(imageSize <= k_maxTransparentImageSize);

  /* The pixel buffer is a concatenation of the pixel colors, which are encoded
   * on two bytes per pixel (the size of KDColor), and then the pixel
   * transparencies, which are encoded on 1 byte per pixel */
  constexpr int k_maxSizeOfPixelBuffer =
      k_maxTransparentImageSize * (sizeof(KDColor) + sizeof(uint8_t));
  /* Note: "/" is an integer division so k_maxSizeOfPixelBuffer must be an even
   * integer */
  static_assert(k_maxSizeOfPixelBuffer % sizeof(KDColor) == 0);
  KDColor pixelBuffer[k_maxSizeOfPixelBuffer / sizeof(KDColor)];

  int sizeOfPixelBuffer = imageSize * (sizeof(KDColor) + sizeof(uint8_t));

  // That's a VERY big buffer we're allocating on the stack
  assert(Ion::stackSafe());

  OMG::Memory::Decompress(image->compressedPixelData(),
                          reinterpret_cast<uint8_t*>(pixelBuffer),
                          image->compressedPixelDataSize(), sizeOfPixelBuffer);

  uint8_t* mask =
      reinterpret_cast<uint8_t*>(pixelBuffer) + sizeof(KDColor) * imageSize;
  blendInPlace(pixelBuffer, mask);

  ctx->fillRectWithPixels(bounds(), pixelBuffer, nullptr);
}

void TransparentThemedImageView::blendInPlace(KDColor* colorBuffer,
                                              uint8_t* alphaBuffer) const {
  const Image* image = currentImage();
  KDColor backgroundColor = ColorPicker::Resolve(m_backgroundColor);
  for (int row = 0; row < image->height(); row++) {
    for (int col = 0; col < image->width(); col++) {
      int position = col + row * image->width();
      colorBuffer[position] = KDColor::Blend(
          colorBuffer[position], backgroundColor, alphaBuffer[position]);
    }
  }
}

void TransparentThemedImageView::setHighlighted(bool highlighted) {
  setBackgroundColor(Highlight(k_defaultBackground, highlighted));
  ThemedImageView::setHighlighted(highlighted);
}

}  // namespace Escher
