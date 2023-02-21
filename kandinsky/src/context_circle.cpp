/* Based on https://cs.uwaterloo.ca/research/tr/1984/CS-84-38.pdf
 * Algorithms for drawing anti-aliased circles and ellipses
 * Dan Field */
#include <kandinsky/context.h>

#include <cmath>

// Returns index of position in buffer
static int posToIndex(int i, int j, int r) {
  return r + i + (r - 1 - j) * 2 * r;
}

// TODO both setToAllSymetries set values to the same pixels several times
/* Sets the pixel value to all eight octant
 * when the diameter is even
 *          , - ~ ~ ~ - ,
 *      , '   x  |  x    ' ,
 *    ,   ⟍      |       ⟋  ',
 *   ,  x    ⟍   |    ⟋    x  ,
 *  ,          ⟍ | ⟋           ,
 *  ,------------|--------------,
 *  ,          ⟋ | ⟍           ,
 *   ,  x   ⟋    |   ⟍     x  ,
 *    ,   ⟋      |      ⟍    ,
 *      ,     x  |  x      ,'
 *        ' - , _ _ _ ,  '
 */
static void setToAllSymetries(float buffer[], float alpha, int i, int j,
                              int r) {
  buffer[posToIndex(i, j, r)] = alpha;
  buffer[posToIndex(i, -j - 1, r)] = alpha;
  buffer[posToIndex(-i - 1, j, r)] = alpha;
  buffer[posToIndex(-i - 1, -j - 1, r)] = alpha;
  buffer[posToIndex(j, i, r)] = alpha;
  buffer[posToIndex(-j - 1, i, r)] = alpha;
  buffer[posToIndex(j, -i - 1, r)] = alpha;
  buffer[posToIndex(-j - 1, -i - 1, r)] = alpha;
}

static float alphaGivenSpacingAndPosition(int i, int j, KDCoordinate spacing,
                                          bool ascending) {
  return (i + (ascending ? -j : j)) % (spacing + 1) == 0 ? 1.f : 0.f;
}

void KDContext::fillCircleWithStripes(KDPoint topLeft, KDCoordinate radius,
                                      KDColor color, KDColor background,
                                      KDCoordinate spacing, bool ascending) {
  // This algorithm can be greatly optimized
  constexpr static int maxDiameter = 30;
  assert(radius <= maxDiameter / 2);
  float buffer[maxDiameter * maxDiameter]{0};
  KDColor colorBuffer[maxDiameter * maxDiameter];

  int numberOfIterations = std::round((radius)*M_SQRT1_2);
  int r2 = radius * radius;
  float hi, hip1 = radius - 1e-5, alphaAbove, alphaBelow;
  int yi;

  for (int i = 0; i < numberOfIterations; i++) {
    hi = hip1;
    hip1 = std::sqrt(r2 - pow(i + 1, 2));
    yi = std::floor(hi - 1e-5);

    if (spacing == 0) {
      if (yi > hip1) {  // Circle crosses two pixels
        alphaAbove = (hi - yi) / 2;
        alphaBelow = (2 + hip1 - yi) / 2;
        setToAllSymetries(buffer, alphaBelow, i, yi - 1, radius);
        setToAllSymetries(buffer, alphaAbove, i, yi, radius);
      } else {  // Circle crosses one pixel
        alphaBelow = (hi + hip1 - 2 * yi) / 2;
        setToAllSymetries(buffer, alphaBelow, i, yi, radius);
      }
    } else {
      yi += 1;
    }

    // Filled lines
    for (int j = yi - 1; j > -yi - 1; j--) {
      buffer[posToIndex(i, j, radius)] =
          alphaGivenSpacingAndPosition(i, j, spacing, ascending);
      buffer[posToIndex(-i - 1, j, radius)] =
          alphaGivenSpacingAndPosition(-i - 1, j, spacing, ascending);
    }
    for (int j = i; j > -i - 2; j--) {
      buffer[posToIndex(yi - 1, j, radius)] =
          alphaGivenSpacingAndPosition(yi - 1, j, spacing, ascending);
      buffer[posToIndex(-yi, j, radius)] =
          alphaGivenSpacingAndPosition(-yi, j, spacing, ascending);
    }
  }

  // Convert to KDColor array
  uint8_t r = color.red(), g = color.green(), b = color.blue();
  uint8_t br = background.red(), bg = background.green(),
          bb = background.blue();
  for (int index = 0; index < (2 * radius) * (2 * radius); index++) {
    float alpha = buffer[index];
    colorBuffer[index] = KDColor::RGB888(r * alpha + br * (1 - alpha),
                                         g * alpha + bg * (1 - alpha),
                                         b * alpha + bb * (1 - alpha));
  }
  KDRect circleRect =
      KDRect(0, 0, 2 * radius, 2 * radius).translatedBy(topLeft);
  fillRectWithPixels(circleRect, colorBuffer, colorBuffer);
}
