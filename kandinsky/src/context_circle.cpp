#include <kandinsky/context.h>

#include <cmath>

// Returns index of position in buffer
static int posToIndex(int i, int j, int r) {
  return r + i + (r - 1 - j) * 2 * r;
}

// Sets the pixel value to all eight octant
static void setToAllSymetries(float buffer[], float alpha, int i, int j, int r) {
  buffer[posToIndex(i, j, r)] = alpha;
  buffer[posToIndex(i, -j - 1, r)] = alpha;
  buffer[posToIndex(-i - 1, j, r)] = alpha;
  buffer[posToIndex(-i - 1, -j - 1, r)] = alpha;
  buffer[posToIndex(j, i, r)] = alpha;
  buffer[posToIndex(-j - 1, i, r)] = alpha;
  buffer[posToIndex(j, -i - 1, r)] = alpha;
  buffer[posToIndex(-j - 1, -i - 1, r)] = alpha;
}

void KDContext::fillAntialiasedCircle(KDPoint topLeft, KDCoordinate radius, KDColor color, KDColor background) {
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

    if (yi > hip1) {      // Circle crosses two pixels
      alphaAbove = (hi - yi) / 2;
      alphaBelow = (2 + hip1 - yi) / 2;
      setToAllSymetries(buffer, alphaBelow, i, yi - 1, radius);
      setToAllSymetries(buffer, alphaAbove, i, yi, radius);
    } else {              // Circle crosses one pixel
      alphaBelow = (hi + hip1 - 2 * yi) / 2;
      setToAllSymetries(buffer, alphaBelow, i, yi, radius);
    }

    // Filled lines
    for (int j = yi - 1; j > -yi - 1; j--) {
      buffer[posToIndex(i, j, radius)] = 1;
      buffer[posToIndex(-i - 1, j, radius)] = 1;
    }
    for (int j = i; j > -i - 2; j--) {
      buffer[posToIndex(yi - 1, j, radius)] = 1;
      buffer[posToIndex(-yi, j, radius)] = 1;
    }
  }

  // Convert to KDColor array
  uint8_t r = color.red(), g = color.green(), b = color.blue();
  uint8_t br = background.red(), bg = background.green(), bb = background.blue();
  for (int index = 0; index < (2 * radius) * (2 * radius); index++) {
    float alpha = buffer[index];
    colorBuffer[index] = KDColor::RGB888(
        r * alpha + br * (1 - alpha), g * alpha + bg * (1 - alpha), b * alpha + bb * (1 - alpha));
  }
  KDRect circleRect = KDRect(0, 0, 2 * radius, 2 * radius).translatedBy(topLeft);
  fillRectWithPixels(circleRect, colorBuffer, colorBuffer);
}