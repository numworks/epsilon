#include <assert.h>
#include <escher/palette.h>

namespace Escher {

constexpr KDColor Palette::YellowDark;
constexpr KDColor Palette::YellowLight;
constexpr KDColor Palette::PurpleBright;
constexpr KDColor Palette::PurpleDark;
constexpr KDColor Palette::GrayWhite;
constexpr KDColor Palette::GrayBright;
constexpr KDColor Palette::GrayMiddle;
constexpr KDColor Palette::GrayDark;
constexpr KDColor Palette::GrayDarkMiddle;
constexpr KDColor Palette::GrayVeryDark;
constexpr KDColor Palette::Select;
constexpr KDColor Palette::SelectDark;
constexpr KDColor Palette::WallScreen;
constexpr KDColor Palette::WallScreenDark;
constexpr KDColor Palette::PopUpTitleBackground;
constexpr KDColor Palette::LowBattery;
constexpr KDColor Palette::Red;
constexpr KDColor Palette::RedLight;
constexpr KDColor Palette::Magenta;
constexpr KDColor Palette::Turquoise;
constexpr KDColor Palette::Pink;
constexpr KDColor Palette::Blue;
constexpr KDColor Palette::BlueLight;
constexpr KDColor Palette::Orange;
constexpr KDColor Palette::Green;
constexpr KDColor Palette::GreenLight;
constexpr KDColor Palette::Brown;
constexpr KDColor Palette::Purple;
constexpr KDColor Palette::Cyan;
constexpr KDColor Palette::BlueishGray;
constexpr KDColor Palette::DataColor[];
constexpr KDColor Palette::DataColorLight[];

KDColor Palette::nextDataColor(int* colorIndex) {
  int nbOfColors = numberOfDataColors();
  assert(*colorIndex < nbOfColors);
  KDColor c = DataColor[*colorIndex];
  *colorIndex = (*colorIndex + 1) % nbOfColors;
  return c;
}

}  // namespace Escher
