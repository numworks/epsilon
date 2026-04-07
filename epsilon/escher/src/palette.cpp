#include <assert.h>
#include <escher/palette.h>

namespace Escher {

KDColor Palette::nextDataColor(int* colorIndex) {
  assert(*colorIndex < k_numberOfDataColors);
  KDColor c = DataColor[*colorIndex];
  *colorIndex = (*colorIndex + 1) % k_numberOfDataColors;
  return c;
}

}  // namespace Escher
