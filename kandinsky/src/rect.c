#include <kandinsky/rect.h>
#include <kandinsky/pixel.h>
#include <kandinsky/context.h>
#include <string.h>
#include <assert.h>

KDRect KDRectZero = {.x = 0, .y = 0, .width = 0, .height = 0};

static inline KDCoordinate left(KDRect r) { return r.x; }
static inline KDCoordinate right(KDRect r) { return r.x+r.width; }
static inline KDCoordinate top(KDRect r) { return r.y; }
static inline KDCoordinate bottom(KDRect r) { return r.y+r.height; }
static inline KDCoordinate min(KDCoordinate x, KDCoordinate y) { return (x<y ? x : y); }
static inline KDCoordinate max(KDCoordinate x, KDCoordinate y) { return (x>y ? x : y); }

bool KDRectIntersect(KDRect r1, KDRect r2) {
    return !(right(r2) < left(r1)
             ||
             left(r2) > right(r1)
             ||
             top(r2) > bottom(r1)
             ||
             bottom(r2) < top(r1)
             );
}

KDRect KDRectIntersection(KDRect r1, KDRect r2) {
    if (!KDRectIntersect(r1, r2)) {
        return KDRectZero;
    }

    KDCoordinate intersectionLeft = max(left(r1), left(r2));
    KDCoordinate intersectionRight = min(right(r1), right(r2));
    KDCoordinate intersectionTop = max(top(r1), top(r2));
    KDCoordinate intersectionBottom = min(bottom(r1), bottom(r2));

    KDRect intersection;
    intersection.x = intersectionLeft;
    intersection.width = intersectionRight-intersectionLeft;
    intersection.y = intersectionTop;
    intersection.height = intersectionBottom-intersectionTop;
    return intersection;
}

static void KDRectComputeUnionBound(KDCoordinate size1, KDCoordinate size2,
    KDCoordinate * outputMin, KDCoordinate * outputMax,
    KDCoordinate min1, KDCoordinate min2,
    KDCoordinate max1, KDCoordinate max2)
{
  if (size1 != 0) {
    if (size2 != 0) {
      *outputMin = min(min1, min2);
      *outputMax = max(max1, max2);
    } else {
      *outputMin = min1;
      *outputMax = max1;
    }
  } else {
    if (size2 != 0) {
      *outputMin = min2;
      *outputMax = max2;
    }
  }
}

KDRect KDRectUnion(KDRect r1, KDRect r2) {
  /* We should ignore coordinate whose size is zero
   * For example, if r1.height is zero, just ignore r1.y and r1.height. */

  KDCoordinate resultLeft = 0;
  KDCoordinate resultTop = 0;
  KDCoordinate resultRight = 0;
  KDCoordinate resultBottom = 0;

  KDRectComputeUnionBound(r1.width, r2.width,
      &resultLeft, &resultRight,
      left(r1), left(r2),
      right(r1), right(r2));

  KDRectComputeUnionBound(r1.height, r2.height,
      &resultTop, &resultBottom,
      top(r1), top(r2),
      bottom(r1), bottom(r2));

  return (KDRect){
    .x = resultLeft,
    .y = resultTop,
    .width = resultRight-resultLeft,
    .height = resultBottom-resultTop
  };
}

bool KDRectContains(KDRect r, KDPoint p) {
  return (p.x >= r.x && p.x < (r.x+r.width) && p.y >= r.y && p.y < (r.y+r.height));
}

KDRect KDRectTranslate(KDRect r, KDPoint p) {
  return (KDRect){
    .origin = KDPointTranslate(r.origin, p),
    .size = r.size
  };
}

KDRect absoluteFillRect(KDRect rect) {
  KDRect absolutRect = rect;
  absolutRect.origin = KDPointTranslate(absolutRect.origin, KDCurrentContext->origin);

  KDRect rectToBeFilled = KDRectIntersection(absolutRect, KDCurrentContext->clippingRect);
  return rectToBeFilled;
}

void KDFillRect(KDRect rect, KDColor color) {
  KDRect absoluteRect = absoluteFillRect(rect);
  if (absoluteRect.width > 0 && absoluteRect.height > 0) {
    KDCurrentContext->io.pushRectUniform(absoluteRect, color);
  }
}

/* Note: we support the case where workingBuffer IS equal to pixels */
void KDFillRectWithPixels(KDRect rect, const KDColor * pixels, KDColor * workingBuffer) {
  KDRect absoluteRect = absoluteFillRect(rect);

  if (absoluteRect.width == 0 || absoluteRect.height == 0) {
    return;
  }

  /* Caution:
   * The absoluteRect may have a SMALLER size than the original rect because it
   * has been clipped. Therefore we cannot assume that the mask can be read as a
   * continuous area. */

  if (absoluteRect.width == rect.width && absoluteRect.height == rect.height) {
    KDCurrentContext->io.pushRect(absoluteRect, pixels);
    return;
  }

  /* At this point we need the working buffer */
  assert(workingBuffer != NULL);
  for (KDCoordinate j=0; j<absoluteRect.height; j++) {
    for (KDCoordinate i=0; i<absoluteRect.width; i++) {
      workingBuffer[i+absoluteRect.width*j] = pixels[i+rect.width*j];
    }
  }
  KDCurrentContext->io.pushRect(absoluteRect, workingBuffer);
}

// Mask's size must be rect.size
// WorkingBuffer, same deal
void KDFillRectWithMask(KDRect rect, KDColor color, const uint8_t * mask, KDColor * workingBuffer) {
  KDRect absoluteRect = absoluteFillRect(rect);

  /* Caution:
   * The absoluteRect may have a SMALLER size than the original rect because it
   * has been clipped. Therefore we cannot assume that the mask can be read as a
   * continuous area. */

  KDCurrentContext->io.pullRect(absoluteRect, workingBuffer);
  for (KDCoordinate j=0; j<absoluteRect.height; j++) {
    for (KDCoordinate i=0; i<absoluteRect.width; i++) {
      KDColor * currentPixelAdress = workingBuffer + i + absoluteRect.width*j;
      const uint8_t * currentMaskAddress = mask + i + rect.width*j;
      *currentPixelAdress = KDColorBlend(*currentPixelAdress, color, *currentMaskAddress);
    }
  }
  KDCurrentContext->io.pushRect(absoluteRect, workingBuffer);
}

#if 0

/* Takes an absolute rect and pushes the pixels */
/* Does the pattern-to-continuous-memory conversion */
void pushRect(KDRect rect, const KDColor * pattern, KDSize patternSize) {
  KDCurrentContext->io.setWorkingArea(rect);
//#define ION_DEVICE_FILL_RECT_FAST_PATH 1
#if ION_DEVICE_FILL_RECT_FAST_PATH
  /* If the pattern width matches the target rect width, we can easily push
   * mutliple lines at once since those will be contiguous in memory. */
  if (patternSize.width == rect.width) {
    size_t remainingPixelCount = rect.width*rect.height;
    size_t patternPixelCount = patternSize.width*patternSize.height;
    while (remainingPixelCount > 0) {
      int32_t blockSize = remainingPixelCount < patternPixelCount ? remainingPixelCount : patternPixelCount;
      KDCurrentContext->io.pushPixels(pattern, blockSize);
      remainingPixelCount -= blockSize;
    }
    return;
  }
#endif
  uint16_t remainingHeight = rect.height;
  uint16_t patternLine = 0;
  while (remainingHeight-- > 0) {
    uint16_t remainingWidth = rect.width;
    while (remainingWidth > 0) {
      int32_t blockSize = remainingWidth < patternSize.width ? remainingWidth : patternSize.width;
      KDCurrentContext->io.pushPixels(pattern + patternLine*patternSize.width, blockSize);
      remainingWidth -= blockSize;
    }
    if (++patternLine >= patternSize.height) {
      patternLine = 0;
    }
  }
}

void KDBlitRect(KDRect rect, const KDColor * pattern, KDSize patternSize, const uint8_t * mask, KDSize maskSize) {
  assert(pattern != NULL && patternSize.width > 0 && patternSize.height > 0);
  KDRect absolutRect = rect;
  absolutRect.origin = KDPointTranslate(absolutRect.origin, KDCurrentContext->origin);

  KDRect rectToBeFilled = KDRectIntersection(absolutRect, KDCurrentContext->clippingRect);

  bool useBlending = (mask != NULL && maskSize.width > 0 && maskSize.height > 0);

  if (!useBlending) {
    pushRect(rectToBeFilled, pattern, patternSize);
    return;
  }

  assert(KDCurrentContext->io.pullPixels != NULL);

  KDCoordinate patternX = 0, patternY = 0, maskX = 0, maskY = 0;
  for (KDCoordinate j=0; j<rectToBeFilled.height; j++) {
    for (KDCoordinate i=0; i<rectToBeFilled.width; i++) {
      KDColor foregroundColor = pattern[patternX+patternSize.width*patternY];
      KDPoint p = {
        .x = rectToBeFilled.x + i,
        .y = rectToBeFilled.y + j
      };
      KDColor backgroundColor = KDGetAbsolutePixelDirect(p);
      uint8_t alpha = mask[maskX+maskSize.width*maskY];
      KDColor newColor = KDColorBlend(backgroundColor, foregroundColor, alpha);
      KDSetAbsolutePixelDirect(p, newColor);
      if (++patternX >= patternSize.width) {
        patternX = 0;
      }
      if (++maskX >= maskSize.width) {
        maskX = 0;
      }
    }
    if (++patternY >= patternSize.height) {
      patternY = 0;
    }
    if (++maskY >= maskSize.height) {
      maskY = 0;
    }
  }
}
#endif

void KDDrawRect(KDRect rect, KDColor color) {
  KDPoint p1, p2;
  p1.x = rect.x;
  p1.y = rect.y;
  p2.x = rect.x;
  p2.y = rect.y + rect.height;
  for (int i = 0; i<rect.width; i++) {
    KDSetPixel(p1, color);
    KDSetPixel(p2, color);
    p1.x++;
    p2.x++;
  }
  p1.x = rect.x;
  p1.y = rect.y;
  p2.x = rect.x + rect.width;
  p2.y = rect.y;
  for (int i = 0; i<rect.height; i++) {
    KDSetPixel(p1, color);
    KDSetPixel(p2, color);
    p1.y++;
    p2.y++;
  }
}
