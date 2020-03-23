#include <escher/palette.h>
#include <assert.h>

constexpr KDColor Palette::YellowDark;
constexpr KDColor Palette::YellowLight;
constexpr KDColor Palette::PurpleBright;
constexpr KDColor Palette::PurpleDark;
constexpr KDColor Palette::GreyWhite;
constexpr KDColor Palette::GreyBright;
constexpr KDColor Palette::GreyMiddle;
constexpr KDColor Palette::GreyDark;
constexpr KDColor Palette::GreyVeryDark;
constexpr KDColor Palette::Select;
constexpr KDColor Palette::SelectDark;
constexpr KDColor Palette::WallScreen;
constexpr KDColor Palette::WallScreenDark;
constexpr KDColor Palette::SubTab;
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
constexpr KDColor Palette::DataColor[];
constexpr KDColor Palette::DataColorLight[];

KDColor Palette::nextDataColor(int * colorIndex) {
  size_t nbOfColors = numberOfDataColors();
  assert(*colorIndex < nbOfColors);
  KDColor c = DataColor[*colorIndex];
  *colorIndex = (*colorIndex + 1) % nbOfColors;
  return c;
}
