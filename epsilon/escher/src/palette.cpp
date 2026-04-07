#include <assert.h>
#include <escher/palette.h>

namespace Escher {

KDColor Palette::nextDataColor(int* colorIndex) {
  int nbOfColors = numberOfDataColors();
  assert(*colorIndex < nbOfColors);
  KDColor c = DataColor[*colorIndex];
  *colorIndex = (*colorIndex + 1) % nbOfColors;
  return c;
}

}  // namespace Escher
