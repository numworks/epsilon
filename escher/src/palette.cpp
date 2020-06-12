#include <escher/palette.h>
#include <assert.h>

constexpr KDColor Palette::PrimaryText;
constexpr KDColor Palette::SecondaryText; // =GREYDARK
constexpr KDColor Palette::AccentText;
constexpr KDColor Palette::BackgroundHard;
constexpr KDColor Palette::BackgroundApps;
constexpr KDColor Palette::BackgroundAppsSecondary;
constexpr KDColor Palette::Toolbar;
constexpr KDColor Palette::ToolbarText;
constexpr KDColor Palette::ExpressionInputBackground;
constexpr KDColor Palette::ExpressionInputBorder;
constexpr KDColor Palette::Battery;
constexpr KDColor Palette::BatteryInCharge;
constexpr KDColor Palette::BatteryLow;
constexpr KDColor Palette::ApproximateSignText;
constexpr KDColor Palette::ApproximateExpressionText;
constexpr KDColor Palette::ScrollBarForeground;
constexpr KDColor Palette::ScrollBarBackground;

constexpr KDColor Palette::Control;
constexpr KDColor Palette::ControlEnabled;
constexpr KDColor Palette::ControlDisabled;

constexpr KDColor Palette::CalculationBackgroundOdd;
constexpr KDColor Palette::CalculationBackgroundEven;
constexpr KDColor Palette::CalculationEmptyBox;
constexpr KDColor Palette::CalculationEmptyBoxNeeded;

constexpr KDColor Palette::CodeBackground;
constexpr KDColor Palette::CodeBackgroundSelected;
constexpr KDColor Palette::CodeText;
constexpr KDColor Palette::CodeComment;
constexpr KDColor Palette::CodeNumber;
constexpr KDColor Palette::CodeKeyword;
constexpr KDColor Palette::CodeOperator;
constexpr KDColor Palette::CodeString;

constexpr KDColor Palette::ProbabilityCurve;
constexpr KDColor Palette::ProbabilityCellBorder;
constexpr KDColor Palette::ProbabilityHistogramBar;

constexpr KDColor Palette::StatisticsBox;
constexpr KDColor Palette::StatisticsBoxVerticalLine;
constexpr KDColor Palette::StatisticsSelected;
constexpr KDColor Palette::StatisticsNotSelected;

constexpr KDColor Palette::GraphTangent;

constexpr KDColor Palette::SubMenuBackground;
constexpr KDColor Palette::SubMenuBorder;
constexpr KDColor Palette::SubMenuText;

constexpr KDColor Palette::ToolboxHeaderBackground;
constexpr KDColor Palette::ToolboxHeaderText;
constexpr KDColor Palette::ToolboxHeaderBorder;
constexpr KDColor Palette::ToolboxBackground;

constexpr KDColor Palette::ListCellBackground;
constexpr KDColor Palette::ListCellBackgroundSelected;
constexpr KDColor Palette::ListCellBorder;

constexpr KDColor Palette::ButtonBackground;
constexpr KDColor Palette::ButtonBackgroundSelected;
constexpr KDColor Palette::ButtonBackgroundSelectedHighContrast;
constexpr KDColor Palette::ButtonBorder;
constexpr KDColor Palette::ButtonRowBorder;
constexpr KDColor Palette::ButtonBorderOut;
constexpr KDColor Palette::ButtonShadow;
constexpr KDColor Palette::ButtonText;

constexpr KDColor Palette::TabBackground;
constexpr KDColor Palette::TabBackgroundSelected;
constexpr KDColor Palette::TabBackgroundActive;
constexpr KDColor Palette::TabBackgroundSelectedAndActive;
constexpr KDColor Palette::TabText;
constexpr KDColor Palette::TabTextActive;

constexpr KDColor Palette::SubTabBackground;
constexpr KDColor Palette::SubTabBackgroundSelected;
constexpr KDColor Palette::SubTabText;

constexpr KDColor Palette::BannerFirstBackground;
constexpr KDColor Palette::BannerFirstBorder;
constexpr KDColor Palette::BannerFirstText;
constexpr KDColor Palette::BannerFirstVariantBackground;
constexpr KDColor Palette::BannerFirstVariantBorder;
constexpr KDColor Palette::BannerFirstVariantText;
constexpr KDColor Palette::BannerSecondBackground;
constexpr KDColor Palette::BannerSecondBorder;
constexpr KDColor Palette::BannerSecondText;
constexpr KDColor Palette::GridPrimaryLine;
constexpr KDColor Palette::GridSecondaryLine;

constexpr KDColor Palette::HomeBackground;
constexpr KDColor Palette::HomeCellBackground;
constexpr KDColor Palette::HomeCellBackgroundActive;
constexpr KDColor Palette::HomeCellText;
constexpr KDColor Palette::HomeCellTextActive;
constexpr KDColor Palette::HomeCellTextExternal;
constexpr KDColor Palette::HomeCellTextExternalActive;

constexpr KDColor Palette::YellowDark; // Done
constexpr KDColor Palette::YellowLight; // Done
constexpr KDColor Palette::PurpleBright; // Done
constexpr KDColor Palette::PurpleDark; // Done
constexpr KDColor Palette::GreyWhite; // Done
constexpr KDColor Palette::GreyBright; // Done
constexpr KDColor Palette::GreyMiddle; // Done
constexpr KDColor Palette::GreyDark; // Done
constexpr KDColor Palette::GreyVeryDark; // Done
constexpr KDColor Palette::Select; // Done
constexpr KDColor Palette::SelectDark; // Done
constexpr KDColor Palette::WallScreen; // Done
constexpr KDColor Palette::WallScreenDark; // Done
constexpr KDColor Palette::SubTab; // Done
constexpr KDColor Palette::LowBattery; // Done
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

constexpr KDColor Palette::AtomUnknown;
constexpr KDColor Palette::AtomText;
constexpr KDColor Palette::AtomAlkaliMetal;
constexpr KDColor Palette::AtomAlkaliEarthMetal;
constexpr KDColor Palette::AtomLanthanide;
constexpr KDColor Palette::AtomActinide;
constexpr KDColor Palette::AtomTransitionMetal;
constexpr KDColor Palette::AtomPostTransitionMetal;
constexpr KDColor Palette::AtomMetalloid;
constexpr KDColor Palette::AtomHalogen;
constexpr KDColor Palette::AtomReactiveNonmetal;
constexpr KDColor Palette::AtomNobleGas;
constexpr KDColor Palette::AtomTableLines;
constexpr KDColor Palette::AtomColor[];

KDColor Palette::nextDataColor(int * colorIndex) {
  size_t nbOfColors = numberOfDataColors();
  assert(*colorIndex < nbOfColors);
  KDColor c = DataColor[*colorIndex];
  *colorIndex = (*colorIndex + 1) % nbOfColors;
  return c;
}
