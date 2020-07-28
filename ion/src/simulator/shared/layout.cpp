#include "layout.h"
#include "main.h"
#include "platform.h"
#include <ion.h>
#include <limits.h>
#include <cmath>
#include "../assets/key_layouts/horizontal_arrow.h"
#include "../assets/key_layouts/large_squircle.h"
#include "../assets/key_layouts/round.h"
#include "../assets/key_layouts/small_squircle.h"
#include "../assets/key_layouts/vertical_arrow.h"

namespace Ion {
namespace Simulator {
namespace Layout {

#if !EPSILON_SDL_SCREEN_ONLY

static constexpr int backgroundWidth = 1160;
static constexpr int backgroundHeight = 2220;

static constexpr float X(int x) { return static_cast<float>(x)/static_cast<float>(backgroundWidth); }
static constexpr float Y(int y) { return static_cast<float>(y)/static_cast<float>(backgroundHeight); }

static constexpr SDL_FRect areaOfInterest = {X(110), Y(30), X(940), Y(2150)};
static constexpr SDL_FRect screenRect = {X(192), Y(191), X(776), Y(582)};

static SDL_Rect sFrame;

static void makeAbsolute(const SDL_FRect f, SDL_Rect * r) {
  r->x = std::round(f.x * static_cast<float>(sFrame.w) + static_cast<float>(sFrame.x));
  r->y = std::round(f.y * static_cast<float>(sFrame.h) + static_cast<float>(sFrame.y));
  r->w = std::round(f.w * static_cast<float>(sFrame.w));
  r->h = std::round(f.h * static_cast<float>(sFrame.h));
}

static void makeAbsolute(const SDL_FPoint f, SDL_Point * p) {
  p->x = f.x * sFrame.w + sFrame.x;
  p->y = f.y * sFrame.h + sFrame.y;
}

void recompute(int width, int height) {
  float windowWidth = static_cast<float>(width);
  float windowHeight = static_cast<float>(height);

  float aoiRatio = (areaOfInterest.w / areaOfInterest.h) * (static_cast<float>(backgroundWidth)/static_cast<float>(backgroundHeight));
  float windowRatio = windowWidth/windowHeight;

  if (aoiRatio > windowRatio) {
    // Area of interest is wider than the window (e.g. aoe 16:9, window 4:3)
    // There will be "black bars" above and below
    // We want the areaOfInterest's rect in pixels to be
    // aoiInPixels.w = windowWidth
    // aoiInPixels.x = 0
    // aoiInPixels.h = windowWidth / (aoiRatio*deviceRatio)
    // aoiInPixels.y = (windowHeight - aoiInPixels.h)/2;
    // But we also know that
    // aoiInPixels.x = sFrame.x + areaOfInterest.x*sFrame.w
    // aoiInPixels.y = sFrame.y + areaOfInterest.y*sFrame.h
    // aoiInPixels.w = sFrame.w * areaOfInterest.w
    // aoiInPixels.h = sFrame.h * areaOfInterest*h

    sFrame.w = windowWidth / areaOfInterest.w;
    sFrame.h = (windowWidth / aoiRatio) / areaOfInterest.h;
    sFrame.x = - areaOfInterest.x * sFrame.w;
    sFrame.y = (windowHeight - windowWidth/aoiRatio)/2 - areaOfInterest.y * sFrame.h;
  } else {
    // Area of interest is taller than the window (e.g. window 16:9, aoe 4:3)
    // There will be "black bars" on the sides
    // We want the areaOfInterest's rect in pixels to be
    // aoiInPixels.h = windowHeight
    // aoiInPixels.y = 0
    // aoiInPixels.x = windowHeight * aoiRatio
    // aoiInPixels.w = (windowWidth - aoiInPixels.w)/2;
    // But we also know that
    // aoiInPixels.x = sFrame.x + areaOfInterest.x*sFrame.w
    // aoiInPixels.y = sFrame.y + areaOfInterest.y*sFrame.h
    // aoiInPixels.w = sFrame.w * areaOfInterest.w
    // aoiInPixels.h = sFrame.h * areaOfInterest*h

    sFrame.h = windowHeight / areaOfInterest.h;
    sFrame.w = (windowHeight * aoiRatio) / areaOfInterest.w;
    sFrame.y = - areaOfInterest.y * sFrame.h;
    sFrame.x = (windowWidth - windowHeight*aoiRatio)/2.0f - areaOfInterest.x * sFrame.w;
  }
}

void getScreenRect(SDL_Rect * rect) {
  makeAbsolute(screenRect, rect);
}

void getBackgroundRect(SDL_Rect * rect) {
  rect->x = sFrame.x;
  rect->y = sFrame.y;
  rect->w = sFrame.w;
  rect->h = sFrame.h;
}

class KeyLayout {
public:
  enum class Shape {
    HorizontalArrow,
    VerticalArrow,
    Round,
    SmallSquircle,
    LargeSquircle
  };
  constexpr KeyLayout(float x, float y, Shape shape) :
    m_center{X(x), Y(y)},
    m_shape(shape) {}
  SDL_FPoint center() const { return m_center; }
  Shape shape() const { return m_shape; }

private:
  SDL_FPoint m_center;
  Shape m_shape;
};

static constexpr KeyLayout sKeyLayouts[Keyboard::NumberOfValidKeys] = {
  KeyLayout(191, 1029, KeyLayout::Shape::HorizontalArrow), // A1, Left
  KeyLayout(273, 945, KeyLayout::Shape::VerticalArrow), // A2, Up
  KeyLayout(273, 1110, KeyLayout::Shape::VerticalArrow), // A3, Down
  KeyLayout(355, 1029, KeyLayout::Shape::HorizontalArrow), // A4, Right
  KeyLayout(810, 1029, KeyLayout::Shape::Round), // A5, OK
  KeyLayout(963, 1029, KeyLayout::Shape::Round), // A6, Back

  KeyLayout(580, 958, KeyLayout::Shape::LargeSquircle), // B1, Home
  KeyLayout(580, 1094, KeyLayout::Shape::LargeSquircle), // B2, Power

  KeyLayout(198, 1252, KeyLayout::Shape::SmallSquircle), // C1, Shift
  KeyLayout(352, 1252, KeyLayout::Shape::SmallSquircle), // C2, Alpha
  KeyLayout(506, 1252, KeyLayout::Shape::SmallSquircle), // C3, xnt
  KeyLayout(656, 1252, KeyLayout::Shape::SmallSquircle), // C4, var
  KeyLayout(810, 1252, KeyLayout::Shape::SmallSquircle), // C5, toolbox
  KeyLayout(963, 1252, KeyLayout::Shape::SmallSquircle), // C6, Delete

  KeyLayout(198, 1375, KeyLayout::Shape::SmallSquircle), // D1, exp
  KeyLayout(352, 1375, KeyLayout::Shape::SmallSquircle), // D2, ln
  KeyLayout(506, 1375, KeyLayout::Shape::SmallSquircle), // D3, log
  KeyLayout(656, 1375, KeyLayout::Shape::SmallSquircle), // D4, i
  KeyLayout(810, 1375, KeyLayout::Shape::SmallSquircle), // D5, comma
  KeyLayout(963, 1375, KeyLayout::Shape::SmallSquircle), // D6, power

  KeyLayout(198, 1498, KeyLayout::Shape::SmallSquircle), // E1, sin
  KeyLayout(352, 1498, KeyLayout::Shape::SmallSquircle), // E2, cos
  KeyLayout(506, 1498, KeyLayout::Shape::SmallSquircle), // E3, tan
  KeyLayout(656, 1498, KeyLayout::Shape::SmallSquircle), // E4, pi
  KeyLayout(810, 1498, KeyLayout::Shape::SmallSquircle), // E5, sqrt
  KeyLayout(963, 1498, KeyLayout::Shape::SmallSquircle), // E6, square

  KeyLayout(210, 1629, KeyLayout::Shape::LargeSquircle), // F1, 7
  KeyLayout(395, 1629, KeyLayout::Shape::LargeSquircle), // F2, 8
  KeyLayout(580, 1629, KeyLayout::Shape::LargeSquircle), // F3, 9
  KeyLayout(765, 1629, KeyLayout::Shape::LargeSquircle), // F4, (
  KeyLayout(950, 1629, KeyLayout::Shape::LargeSquircle), // F5, )

  KeyLayout(210, 1766, KeyLayout::Shape::LargeSquircle), // G1, 4
  KeyLayout(395, 1766, KeyLayout::Shape::LargeSquircle), // G2, 5
  KeyLayout(580, 1766, KeyLayout::Shape::LargeSquircle), // G3, 6
  KeyLayout(765, 1766, KeyLayout::Shape::LargeSquircle), // G4, *
  KeyLayout(950, 1766, KeyLayout::Shape::LargeSquircle), // G5, /

  KeyLayout(210, 1902, KeyLayout::Shape::LargeSquircle), // H1, 1
  KeyLayout(395, 1902, KeyLayout::Shape::LargeSquircle), // H2, 2
  KeyLayout(580, 1902, KeyLayout::Shape::LargeSquircle), // H3, 3
  KeyLayout(765, 1902, KeyLayout::Shape::LargeSquircle), // H4, +
  KeyLayout(950, 1902, KeyLayout::Shape::LargeSquircle), // H5, -

  KeyLayout(210, 2040, KeyLayout::Shape::LargeSquircle), // I1, 0
  KeyLayout(395, 2040, KeyLayout::Shape::LargeSquircle), // I2, .
  KeyLayout(580, 2040, KeyLayout::Shape::LargeSquircle), // I3, x10
  KeyLayout(765, 2040, KeyLayout::Shape::LargeSquircle), // I4, Ans
  KeyLayout(950, 2040, KeyLayout::Shape::LargeSquircle), // I5, EXE
};

const Image * imageForKey(int keyIndex) {
  if (keyIndex == -1) {
    return nullptr;
  }
  assert(keyIndex >= 0 && keyIndex < Keyboard::NumberOfValidKeys);
  switch (sKeyLayouts[keyIndex].shape()) {
    case KeyLayout::Shape::Round:
      return ImageStore::Round;
    case KeyLayout::Shape::LargeSquircle:
      return ImageStore::LargeSquircle;
    case KeyLayout::Shape::SmallSquircle:
      return ImageStore::SmallSquircle;
    case KeyLayout::Shape::VerticalArrow:
      return ImageStore::VerticalArrow;
    default:
      assert(sKeyLayouts[keyIndex].shape() == KeyLayout::Shape::HorizontalArrow);
      return ImageStore::HorizontalArrow;
  }
}

static void getKeyCenter(int validKeyIndex, SDL_Point * point) {
  assert(validKeyIndex >= 0 && validKeyIndex < Keyboard::NumberOfValidKeys);
  makeAbsolute(sKeyLayouts[validKeyIndex].center(), point);
}

static void getKeyRectangle(int validKeyIndex, SDL_Rect * rect) {
  assert(validKeyIndex >= 0 && validKeyIndex < Keyboard::NumberOfValidKeys);
  SDL_FPoint point = sKeyLayouts[validKeyIndex].center();
  const Image * img = imageForKey(validKeyIndex);
  SDL_FRect fRect;
  fRect.w = X(img->width());
  fRect.h = Y(img->height());
  fRect.x = point.x - fRect.w/2.0f;
  fRect.y = point.y - fRect.h/2.0f;
  makeAbsolute(fRect, rect);
}

int sHighlightedKeyIndex;

Keyboard::Key highlightKeyAt(SDL_Point * p) {
  int newHighlightedKeyIndex = -1;
  int minSquaredDistance = INT_MAX;
  Keyboard::Key nearestKey = Keyboard::Key::None;
  /* The closenessThreshold is apportioned to the size of the frame. As the
   * width and the height have a constant ratio, we can compute the
   * closenessThreshold from the frame width exclusively. */
  int closenessThreshold = sFrame.w/6;
  int squaredClosenessThreshold = closenessThreshold*closenessThreshold;
  for (int i=0; i<Keyboard::NumberOfValidKeys; i++) {
    SDL_Point keyCenter;
    getKeyCenter(i, &keyCenter);
    int dx = keyCenter.x - p->x;
    int dy = keyCenter.y - p->y;
    int squaredDistance = dx*dx + dy*dy;
    if (squaredDistance < squaredClosenessThreshold && squaredDistance < minSquaredDistance) {
      minSquaredDistance = squaredDistance;
      nearestKey = Keyboard::ValidKeys[i];
      newHighlightedKeyIndex = i;
    }
  }
  if (newHighlightedKeyIndex != sHighlightedKeyIndex) {
    sHighlightedKeyIndex = newHighlightedKeyIndex;
    Main::setNeedsRefresh();
  }
  return nearestKey;
}

SDL_PixelFormat * sRgbaPixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);

// round.png file is 130 x 130 and is the largest key layout
static constexpr size_t k_maxKeyLayoutSize = 130*130;
static constexpr uint8_t k_blendingRatio = 0x44;

void fillRGBABufferWithImage(Uint32 * buffer, const Image * img) {
  KDColor pixelBuffer[k_maxKeyLayoutSize];
  Ion::decompress(
    img->compressedPixelData(),
    reinterpret_cast<uint8_t *>(pixelBuffer),
    img->compressedPixelDataSize(),
    img->width() * img->height() * sizeof(KDColor)
  );
  for (int i = 0; i < img->width() * img->height(); i++) {
    buffer[i] = SDL_MapRGBA(sRgbaPixelFormat, pixelBuffer[i].red(), pixelBuffer[i].green(), pixelBuffer[i].blue(), k_blendingRatio);
  }
}

void drawHighlightedKey(SDL_Renderer * renderer) {
  if (sHighlightedKeyIndex < 0) {
    return;
  }
  const Image * img = imageForKey(sHighlightedKeyIndex);
  SDL_Texture * framebufferTexture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_RGBA32,
    SDL_TEXTUREACCESS_STREAMING,
    img->width(),
    img->height()
  );
  SDL_SetTextureBlendMode(framebufferTexture, SDL_BLENDMODE_BLEND);
  int pitch = 0;
  void * pixels = nullptr;
  SDL_LockTexture(framebufferTexture, nullptr, &pixels, &pitch);
  assert(pitch == sizeof(Uint32) * img->width());
  fillRGBABufferWithImage(static_cast<Uint32 *>(pixels), img);
  SDL_UnlockTexture(framebufferTexture);
  SDL_Rect rect;
  getKeyRectangle(sHighlightedKeyIndex, &rect);
  SDL_RenderCopy(renderer, framebufferTexture, nullptr, &rect);
  SDL_DestroyTexture(framebufferTexture);

  // Reset highlighted key
  sHighlightedKeyIndex = -1;
  Main::setNeedsRefresh();
}

static SDL_Texture * sBackgroundTexture = nullptr;

void init(SDL_Renderer * renderer) {
  sBackgroundTexture = IonSimulatorLoadImage(renderer, "background.jpg");
}

void draw(SDL_Renderer * renderer) {
  SDL_Rect backgroundRect;
  getBackgroundRect(&backgroundRect);
  SDL_RenderCopy(renderer, sBackgroundTexture, nullptr, &backgroundRect);
  drawHighlightedKey(renderer);
}

void quit() {
  SDL_DestroyTexture(sBackgroundTexture);
  sBackgroundTexture = nullptr;
}

#endif

}
}
}
