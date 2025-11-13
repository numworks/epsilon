#include "helper.h"
#include "poincare/preferences.h"
#include "quiz.h"
#include "quiz/src/symbols.h"

using namespace Poincare;
using namespace Poincare::Internal;
using UnitFormat = Poincare::Preferences::UnitFormat;

QUIZ_CASE(pcj_simplification_unit) {
  simplifies_to("12_m", "12×_m");
  simplifies_to("1_s", "1×_s");
  simplifies_to("1_m+1_yd", "1.9144×_m");
  simplifies_to("1_mm+1_km", "1000.001×_m");
  simplifies_to("2_month×7_dm", "3681720×_s×_m");
  simplifies_to("2×_m/_m", "2");
  simplifies_to("1234_g", "1234×_g");
  simplifies_to("cos(0_rad)", "1");
  simplifies_to("sum(_s,x,0,1)", "2×_s");
  // NOTE: We only allow simple int operations in unit exponents
  simplifies_to("_s^-1", "1×_s^(-1)");
  simplifies_to("_s^(4-2^3+2×3)", "1×_s^2");
  // NOTE: this is a test for ReduceSortedAddition, see comment there
  simplifies_to("0×_A + π×_A - π×_A", "0×_A");
  simplifies_to("sum(_s,x,2,0)", "0×_s");
#if 0
  // See comment in DeepCheckDimensions
  simplifies_to("abs(-3.3_m)", "3.3×_m");
#endif
  simplifies_to("10^(-6)_m^3", "1ᴇ-6×_m^3");
  simplifies_to("1000000_cm", "1000000×_cm");
  simplifies_to("normcdf(0,20,3)×_s", "1.3083978345207ᴇ-11×_s");
  simplifies_to("17_nohm*3+2_nOhm", "53×_nΩ");
  simplifies_to("1/(1/_A)", "1×_A");
  simplifies_to("log(_s×x/(_s×y))",
                "dep(log(x/y),{nonNull(x),nonNull(1/"
                "(1×10^0×y)),realPos((1×10^0×x)/(1×10^0×y))})",
                k_keepAllSymbolsCtx);

  // No unit conversion
  // TODO: should return 1×_m+1×_cm
  simplifies_to("1_m+1_cm", "1×_cm+1×_m", {.m_unitDisplay = UnitDisplay::None});

  // Volumes
  simplifies_to("3_L+2_dL", "3.2×_L");
  simplifies_to("1_L+1_gal", "1.2641720523581×_gal");
  simplifies_to("1_mL+1_m^3", "1.000001×_m^3");
  simplifies_to("4_tsp", "1.3333333333333×_tbsp",
                {.m_unitDisplay = UnitDisplay::AutomaticImperial});
  simplifies_to("3_L*_c", "899377.374×_m^4×_s^(-1)");

  // Temperature
  simplifies_to("4_°C", "4×_°C");
  // Note: this used to be undef in previous Poincare.
  simplifies_to("((4-2)_°C)×2", "4×_°C");
  simplifies_to("((4-2)_°F)×2", "4.0000000000001×_°F");  // TODO: Fix precision
  simplifies_to("8_°C/2", "4×_°C");
  simplifies_to("2_K+2_K", "4×_K");
  simplifies_to("2_K×2_K", "4×_K^2");
  simplifies_to("1/_K", "1×_K^(-1)");
  simplifies_to("(2_K)^2", "4×_K^2");
  simplifies_to("_cKπ23", "72.256631032565×_cK");

  // TODO: Decide on implicit '_' parsing
  //   simplifies_to("1m+1km", "1_m+1_km" /  "m+k×m" / "m+km" );
  //   simplifies_to("1m+1s", "undef" / "m+s");
  //   simplifies_to("1m+x", "m+x" / "undef");

  // Implicit additions
  simplifies_to("3h300min", "28800×_s");
  simplifies_to("3h300min", "8×_h",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});

  // 0
  simplifies_to("0×0×2×(_rad + _°)×_°", "0×_rad^2");
  simplifies_to("ln(2/2)_rad", "0×_rad");
  simplifies_to("ln(2/2)_°", "0×_°", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("5×_°-5×_°", "0×_°",
                {.m_angleUnit = AngleUnit::Radian,
                 .m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("0×0×2×(_km + _m)×_km×_s", "0×_m^2×_s");
  simplifies_to("0×_°C", "0×_°C", {.m_unitDisplay = UnitDisplay::None});
  simplifies_to("6×0×_°F", "0×_°F", {.m_unitDisplay = UnitDisplay::None});
  simplifies_to("0×_K", "0×_K");
  simplifies_to("0×_K×_s×_s×(_g+4×_kg)", "0×_s^2×_kg×_K");
  simplifies_to("0×_L-0×_L", "0×_L");
  simplifies_to("3×_dm-3×_dm", "0×_dm");
  simplifies_to("30×_cm-3×_dm", "0×_m");
  simplifies_to("6×_dm-60×_cm", "0×_m");

  // Angles
  simplifies_to("_rad", "1×_rad");
  simplifies_to("360×_°", "2×π×_rad");
  simplifies_to("π×π×_rad", "π^2×_rad");
  simplifies_to("π×π×_rad", "180×π×_°", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("1×π×_°", "π×_°", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("π×π×_rad×_m", "9.8696044010894×_m×_rad");
  simplifies_to("π×π×_rad×_rad", "π^2×_rad^2");
  simplifies_to("0.2_rad", "1/5×_rad");
  simplifies_to("-0.2_rad", "-1/5×_rad");
  simplifies_to("0.2_rad^2", "1/5×_rad^2");
  // TODO_PCJ : Wasn't exact before
  simplifies_to("π×_rad×_°", "π^2/180×_rad^2");
}

QUIZ_CASE(pcj_simplification_unit_automatic) {
  // BestRepresentative
  simplifies_to("1_m+1_km", "1.001×_km",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("1ᴇ-9_s", "1×_ns",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("1234_g", "1.234×_kg",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("10^(-6)_m^3", "1×_cm^3",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("12345×_tbsp", "182.54261122453×_L",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  // TODO_PCJ: Should be -173.15×_°C
  simplifies_to("100×_K", "100×_K",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("3.6×_MN×_m", "3.6×_MJ",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
}

QUIZ_CASE(pcj_simplification_unit_basicSI) {
  simplifies_to("_km", "1000×_m", {.m_unitDisplay = UnitDisplay::BasicSI});
  // TODO_PCJ: Order of units, should be 0.06×_m^(-1)×_s
  simplifies_to("_min/_km", "0.06×_s×_m^(-1)",
                {.m_unitDisplay = UnitDisplay::BasicSI});
  simplifies_to("_km^3", "1000000000×_m^3",
                {.m_unitDisplay = UnitDisplay::BasicSI});
  simplifies_to("1_m+_km", "1001×_m", {.m_unitDisplay = UnitDisplay::BasicSI});
  simplifies_to("_L^2×3×_s", "3ᴇ-6×_m^6×_s",
                {.m_unitDisplay = UnitDisplay::BasicSI});
  simplifies_to("1000000_cm", "10000×_m",
                {.m_unitDisplay = UnitDisplay::BasicSI});
  simplifies_to("(-1/2)×_'", "-π/21600×_rad",
                {.m_unitDisplay = UnitDisplay::BasicSI});
  simplifies_to("-11.1×_°C", "262.05×_K",
                {.m_unitDisplay = UnitDisplay::BasicSI});
  simplifies_to("-4×_°F", "253.15×_K", {.m_unitDisplay = UnitDisplay::BasicSI});
}

QUIZ_CASE(pcj_simplification_unit_conversion) {
  simplifies_to("180×_°→_rad", "π×_rad", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("91.44_cm→_yd", "1×_yd");
}

QUIZ_CASE(pcj_simplification_unit_decomposition) {
  simplifies_to("123_m", "undef",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("1241_yd", "undef",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("1×_kg", "undef",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("π_year",
                "3×_year+1×_month+21×_day+6×_h+42×_min+4.3249249999999×_s",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("61×_s", "1×_min+1×_s",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("123456×_s", "1×_day+10×_h+17×_min+36×_s",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("1×_week", "7×_day",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("100.125_gon", "90×_°+6×_'+45×_\"",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("35_gon", "31×_°+30×_'",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("1241_yd", "1241×_yd",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("1241_in", "34×_yd+1×_ft+5×_in",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("102038_in-1_ft", "1×_mi+1074×_yd+2×_in",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("1234567×_in", "19×_mi+853×_yd+1×_ft+7×_in",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  // TODO_PCJ: Should be 1×_yd+7.700787×_in
  simplifies_to("1.11×_m", "undef",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("105454.5_oz", "3×_shtn+590×_lb+14.5×_oz",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("1×_lgtn", "1×_shtn+240×_lb",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  // TODO_PCJ: Should be 2×_lb+3.273962×_oz
  simplifies_to("1×_kg", "undef",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("232.8_qt", "58×_gal+1×_pt+1.2×_cup",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("12345×_tbsp", "48×_gal+1×_pt+1.5625×_cup",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  // TODO_PCJ: Should be 264×_gal+1×_pt+0.7528377×_cup
  simplifies_to("1×_m^3", "undef",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
}

QUIZ_CASE(pcj_simplification_unit_equivalent) {
  simplifies_to("3_s", "undef", {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("3_ft^3", "84.950539776×_L",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("3_L", "3×_dm^3", {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("3_dm^3", "3×_L", {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("1×_m^3", "1000×_L",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("5200_m^2", "0.52×_ha",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("6.7_ha", "0.067×_km^2",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("0_°F", "-17.777777777778×_°C",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("23_°F", "-5×_°C", {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("-4×_°F", "-20×_°C",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("0×_K", "-273.15×_°C",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("3_rad", "171.88733853925×_°",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("3_°+6_gon", "8.4×_°",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  // TODO_PCJ: Should be 3.6×_km×_h^(-1)
  simplifies_to("1×_m/_s", "undef", {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("50_mi/_h", "80.4672×_km×_h^(-1)",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("0.3_acre", "13068×_ft^2",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("30000_m^2", "7.413161444015×_acre",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("3000_m^2", "undef",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("3_ft^3", "22.441558441558×_gal",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("2_gal", "462×_in^3",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("35_°C", "95×_°F",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("-11.1×_°C", "12.02×_°F",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("300_K", "80.33×_°F",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("100×_K", "-279.67×_°F",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("30_km/_h", "18.64113576712×_mi×_h^(-1)",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  // TODO_PCJ: Should be 2.236936×_mi×_h^(-1)
  simplifies_to("1×_m/_s", "undef",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
}

QUIZ_CASE(pcj_simplification_unit_imperial) {
  simplifies_to("1609.344_m", "1.609344×_km",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("2×π×_cK", "6.2831853071796×_cK",
                {.m_unitFormat = UnitFormat::Imperial});
}

QUIZ_CASE(pcj_simplification_unit_undef) {
  ProjectionContext replaceSymbolCtx = {
      .m_symbolic = SymbolicComputation::ReplaceAllSymbols};

  // Non homogeneous
  simplifies_to("1_m+1_s", "undef");
  simplifies_to("1_m+x", "undef");
  simplifies_to("1_m+0", "undef");

  // Temperature
  simplifies_to("2_°C-1_°C", "undef");
  simplifies_to("2_°C+2_K", "undef");
  simplifies_to("2_K+2_°C", "undef");
  simplifies_to("2_K×2_°C", "undef");
  simplifies_to("1/_°C", "undef");
  simplifies_to("(1_°C)^2", "undef");

  // Exponent overflow
  /* NOTE: Order of units should not matter. If an overflow can occur when
   * reordering the multiplication, we return undef, even if no overflow
   * technically happened */
  simplifies_to("_s^127 * _s^(-1) * _s", "undef");
  simplifies_to("_s^127 * _s * _s^(-1)", "undef");

  // Only allow simple int operations in unit exponents
  simplifies_to("_s^floor(1)", "undef");
  simplifies_to("_s^ceil(2)", "undef");

  simplifies_to("tan(2_m)", "undef");
  simplifies_to("tan(2_rad^2)", "undef");
  simplifies_to("(_A×x)^2×_s", "undef", replaceSymbolCtx);
}
