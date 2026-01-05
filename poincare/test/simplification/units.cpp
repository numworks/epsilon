#include <poincare/src/expression/units/unit.h>
#include <poincare/user_expression.h>

#include "../helpers/symbol_store.h"
#include "helper.h"
#include "poincare/preferences.h"

using namespace Poincare;
using namespace Poincare::Internal;
using UnitFormat = Poincare::Preferences::UnitFormat;

void assert_parsed_units_simplify_to_with_prefixes() {
  const Units::Representative* const* representatives =
      Units::Representative::DefaultRepresentatives();
  for (int i = 0; i < Units::Representative::k_numberOfDimensions; i++) {
    const Units::Representative* representative = representatives[i];
    if (representative->numberOfRepresentatives() == 0) {
      // Some dimensions have no representative (e.g. Speed)
      continue;
    }
    int numberOfPrefixes;
    const Units::Prefix* prefixes;
    constexpr static size_t bufferSize = 12;
    char buffer[bufferSize] = "1×";
    if (representative->isOutputPrefixable()) {
      numberOfPrefixes = Units::Prefix::k_numberOfPrefixes;
      prefixes = Units::Unit::k_prefixes;
    } else {
      numberOfPrefixes = 1;
      prefixes = Units::Prefix::EmptyPrefix();
    }
    for (int j = 0; j < numberOfPrefixes; j++) {
      if (representative->canPrefix(prefixes + j, true) &&
          representative->canPrefix(prefixes + j, false)) {
        UserExpression unit = UserExpression::Builder(
            Units::Unit::Push(representative, prefixes + j));
        unit.serialize(
            std::span<char>(buffer + strlen("1×"), bufferSize - strlen("1×")));
        simplifies_to(buffer, buffer);
      }
    }
  }
}

QUIZ_CASE(pcj_simplification_unit) {
  // SI base units
  simplifies_to("_s", "1×_s");
  simplifies_to("_m", "1×_m");
  simplifies_to("_kg", "1×_kg");
  simplifies_to("_A", "1×_A");
  simplifies_to("_K", "1×_K");
  simplifies_to("_mol", "1×_mol");
  simplifies_to("_cd", "1×_cd");
  simplifies_to("-_s", "-1×_s");

  // Inverses of SI base units
  simplifies_to("_s^-1", "1×_s^(-1)");
  simplifies_to("_m^-1", "1×_m^(-1)");
  simplifies_to("_kg^-1", "1×_kg^(-1)");
  simplifies_to("_A^-1", "1×_A^(-1)");
  simplifies_to("_K^-1", "1×_K^(-1)");
  simplifies_to("_mol^-1", "1×_mol^(-1)");
  simplifies_to("_cd^-1", "1×_cd^(-1)");

  // Power of SI units
  simplifies_to("_s^3", "1×_s^3");
  simplifies_to("_m^2", "1×_m^2");
  simplifies_to("_m^3", "1×_m^3");

  // Parse units with prefixes
  assert_parsed_units_simplify_to_with_prefixes();
  simplifies_to("_min", "1×_min");
  simplifies_to("_h", "1×_h");
  simplifies_to("_day", "1×_day");
  simplifies_to("_week", "1×_week");
  simplifies_to("_month", "1×_month");
  simplifies_to("_year", "1×_year");
  simplifies_to("_au", "1×_au");
  simplifies_to("_ly", "1×_ly");
  simplifies_to("_pc", "1×_pc");
  simplifies_to("_in", "1×_in");
  simplifies_to("_ft", "1×_ft");
  simplifies_to("_yd", "1×_yd");
  simplifies_to("_mi", "1×_mi");
  simplifies_to("_t", "1×_t");
  simplifies_to("_oz", "1×_oz");
  simplifies_to("_lb", "1×_lb");
  simplifies_to("_shtn", "1×_shtn");
  simplifies_to("_°C", "1×_°C");
  simplifies_to("_°F", "1×_°F");
  simplifies_to("_atm", "1×_atm");
  simplifies_to("_Hz", "1×_Hz");
  simplifies_to("_S", "1×_S");
  simplifies_to("_L", "1×_L");
  simplifies_to("_ha", "1×_ha");

  // SI derived units with special names and symbols
  simplifies_to("_kg×_m×_s^(-2)", "1×_N");
  simplifies_to("_kg×_m^(-1)×_s^(-2)", "1×_Pa");
  simplifies_to("_kg×_m^2×_s^(-2)", "1×_J");
  simplifies_to("_kg×_m^2×_s^(-3)", "1×_W");
  simplifies_to("_A×_s", "1×_C");
  simplifies_to("_kg×_m^2×_s^(-3)×_A^(-1)", "1×_V");
  simplifies_to("_m^(-2)×_kg^(-1)×_s^4×_A^2", "1×_F");
  simplifies_to("_kg×_m^2×_s^(-3)×_A^(-2)", "1×_Ω");
  simplifies_to("_kg×_m^2×_s^(-2)×_A^(-1)", "1×_Wb");
  simplifies_to("_kg×_s^(-2)×_A^(-1)", "1×_T");
  simplifies_to("_kg×_m^2×_s^(-2)×_A^(-2)", "1×_H");
  simplifies_to("_mol×_s^-1", "1×_kat");

  // Usual physical quantities
  simplifies_to("_A×_s×_m^(-3)", "1×_C×_m^(-3)");  // Charge density
  simplifies_to("_kg×_m×_s^(-3)×_K^(-1)",
                "1×_N×_s^(-1)×_K^(-1)");  // Thermal conductivity _W×_m^-1×_K^-1
  simplifies_to("_K×_kg^(-1)×_m^(-2)×_s^3",
                "1×_K×_W^(-1)");  // Thermal resistance
  simplifies_to("_kg×_m×_s^(-3)×_A^(-1)", "1×_V×_m^(-1)");  // Electrical field
  simplifies_to("_kg×_m^2×_s^(-1)",
                "1×_s×_J");  // Action
  simplifies_to("_kg×_m^2×_s^(-2)×_K^(-1)",
                "1×_J×_K^(-1)");  // Entropy | Heat capacity
  simplifies_to(
      "_m^2×_s^(-2)×_K^(-1)",
      "1×_m^2×_K^(-1)×_s^(-2)");  // Specific heat capacity _J×_K^-1×_kg^-1
  simplifies_to("_kg×_m^2×_s^(-2)×_K^(-1)×_mol^(-1)",
                "1×_J×_K^(-1)×_mol^(-1)");  // Molar heat capacity
  simplifies_to("_kg×_m^(-1)×_s^(-2)×_K^(-1)",
                "1×_Pa×_K^(-1)");  // Volumetric heat capacity _J×_K^-1×_m^-3
  simplifies_to(
      "_kg×_s^(-3)×_K^(-1)",
      "1×_kg×_K^(-1)×_s^(-3)");  // Heat transfer coefficient _W×_m^-2×_K^-1
  simplifies_to("_kg×_m^2×_s^(-3)×_K^(-1)",
                "1×_W×_K^(-1)");  // Thermal conductivity
  simplifies_to("_kg^(-1)×_m^(-3)×_s^3×_A^2",
                "1×_m^(-1)×_Ω^(-1)");  // Electrical conductivity _S×_m^-1
  simplifies_to("_kg×_s^(-2)", "1×_kg×_s^(-2)");  // Stiffness _N×_m^-1
  simplifies_to("_kg×_m^(-1)×_s^(-3)",
                "1×_Pa×_s^(-1)");  // Power density _W×_m^-3
  simplifies_to("_kg×_m^3×_s^(-3)×_A^(-1)",
                "1×_m×_V");  // Electric flux
  simplifies_to(
      "_K×_kg^(-1)×_s^(3)",
      "1×_s^3×_K×_kg^(-1)");  // Superficial thermal resistance _m^2×_K×_W^-1
  simplifies_to("_kg^(-1)×_m^(-2)×_s^2", "1×_J^(-1)");  // Thermodynamic beta
  simplifies_to("_kg×_m^(-1)×_s^(-1)",
                "1×_s×_Pa");                        // Dynamic viscosity
  simplifies_to("_m^2×_s^(-2)", "1×_m^2×_s^(-2)");  // Gray/Sievert _J×_kg^(-1)
  simplifies_to("_m^2×_kg×_s^(-1)", "1×_s×_J");   // Angular momentum _N×_m×_s
  simplifies_to("_m^(-1)×_kg×_s^(-2)", "1×_Pa");  // Energy density _J×_m^-3
  simplifies_to("_m×_kg×_s^(-3)", "1×_N×_s^(-1)");  // Spectral power _W×_m^-1
  simplifies_to("_m×_kg^(-1)×_s^2", "1×_Pa^(-1)");  // Compressibility
  simplifies_to(
      "_kg^(-1)×_s^3×_A^2×_mol^(-1)",
      "1×_m^2×_mol^(-1)×_Ω^(-1)");  // Molar conductivity _S×_m^2×_mol^-1
  simplifies_to("_m^(-2)×_s×_A", "1×_C×_m^(-2)");    // Polarization density
  simplifies_to("_kg^(-1)×_s×_A", "1×_C×_kg^(-1)");  // Exposure
  simplifies_to("_kg×_m^3×_s^(-3)×_A^(-2)",
                "1×_m×_Ω");                        // Electrical resistivity
  simplifies_to("_m^(-1)×_s×_A", "1×_C×_m^(-1)");  // Dipole moment
  simplifies_to("_kg^(-1)×_s^2×_A",
                "1×_T^(-1)");  // Electron mobility _m^2×_V^-1×_s^-1
  simplifies_to("_m^(-2)×_kg^(-1)×_s^2×_A^2",
                "1×_H^(-1)");  // Magnetic reluctance
  simplifies_to("_m×_kg×_s^(-2)×_A^(-1)",
                "1×_N×_A^(-1)");  // Magnetic vector potential _Wb×_m^-1 and
                                  // Magnetic rigidity _T×_m
  simplifies_to("_m^3×_kg×_s^(-2)×_A^(-1)", "1×_m×_Wb");  // Magnetic moment
  simplifies_to("_m^(-1)×_kg^(-1)×_s^2×_A^2",
                "1×_A^2×_N^(-1)");  // Magnetic susceptibility _H^-1×_m

  // Physical constants
  // Gravitational constant G _N×_m^2×_kg^-2
  simplifies_to("_kg^(-1)×_m^3×_s^(-2)", "1×_m^3×_kg^(-1)×_s^(-2)");
  // Vacuum electric permittivity µ0 _H×_m^-1
  simplifies_to("_kg×_m×_s^(-2)×_A^(-2)", "1×_N×_A^(-2)");
  // Vacuum magnetic permeability 𝝴0
  simplifies_to("_A^2×_s^4×_kg^(-1)×_m^(-3)", "1×_F×_m^(-1)");
  // Stefan–Boltzmann constant _W×_m^-2×_K^-4
  simplifies_to("_kg×_s^(-3)×_K^(-4)", "1×_kg×_s^(-3)×_K^(-4)");

  // Unit operations
  simplifies_to("_C^3", "1×_C^3");
  simplifies_to("(3_m^2)^3", "27×_m^6");
  simplifies_to("-2×_A", "-2×_A");
  simplifies_to("_m*_s^-2", "1×_m×_s^(-2)");
  simplifies_to("_m+_m", "2×_m");
  simplifies_to("_m-_m", "0×_m");
  simplifies_to("1_m+1_yd", "1.9144×_m");
  simplifies_to("1_mm+1_km", "1000.001×_m");
  simplifies_to("2_month×7_dm", "3681720×_s×_m");
  simplifies_to("2×_m/_m", "2");
  simplifies_to("1234_g", "1234×_g");
  simplifies_to("cos(0_rad)", "1");
  // NOTE: We only allow simple int operations in unit exponents
  simplifies_to("_s^-1", "1×_s^(-1)");
  simplifies_to("_s^(4-2^3+2×3)", "1×_s^2");
  // NOTE: this is a test for ReduceSortedAddition, see comment there
  simplifies_to("0×_A + π×_A - π×_A", "0×_A");
  simplifies_to("sum(_s,x,2,0)", "0×_s");
  simplifies_to("sum(_s,x,0,1)", "2×_s");
  simplifies_to("10^(-6)_m^3", "1ᴇ-6×_m^3");
  simplifies_to("1000000_cm", "1000000×_cm");
  simplifies_to("normcdf(0,20,3)×_s", "1.3083978345207ᴇ-11×_s");
  simplifies_to("17_nohm*3+2_nOhm", "53×_nΩ");
  simplifies_to("1/(1/_A)", "1×_A");
  simplifies_to("log(_s×x/(_s×y))",
                "dep(log(x/y),{nonNull(x),nonNull(1/"
                "(1×10^0×y)),realPos((1×10^0×x)/(1×10^0×y))})",
                k_keepAllSymbolsCtx);
  simplifies_to("cos(1_s/1_s)", "cos(1)");
  simplifies_to("sin(90_°)", "1");
  simplifies_to("cos(π_rad/4)", "√(2)/2", k_degreeCtx);
  simplifies_to("cot((π/4)_rad)", "1", k_gradianCtx);
  simplifies_to("1_m+π_m+√(2)_m-cos(15)_m", "6.3154941288217×_m");
  simplifies_to("abs(-3.3_m)", "3.3×_m");
  simplifies_to("abs(_s)", "1×_s");
  simplifies_to("abs(3_m)", "3×_m");
  // abs(units) is forbidden for complex values
  simplifies_to("abs(-3i*_m)", "undef");
  simplifies_to("1_m^((1-1)^0)", "undef");
  simplifies_to("((0h0s)^2)^2", "0×_s^4");

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
  simplifies_to("0×_°C", "0×_°C");
  simplifies_to("-32×_°F", "-32×_°F");
  simplifies_to("273.16×_K", "273.16×_K");
  simplifies_to("(12_m/6_m)×_°C", "2×_°C");
  simplifies_to("°C→x", "1×_°C→x");
  simplifies_to("123°C→x", "123×_°C→x");
  simplifies_to("-4.56°C→x", "-4.56×_°C→x");

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
  simplifies_to("det([[cos(3gon)]])", "cos((3×π)/200)");
  simplifies_to("ref([[cos(3gon)]])", "[[1]]");
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

  // Order of magnitude
  simplifies_to("100_kg", "100×_kg",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("10000_kg", "10×_t",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("1000000_kg", "1×_kt",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("1_min", "1×_min",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("0.1_m", "1×_dm",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("1/_m+1/_km", "1.001×_m^(-1)",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("180_MΩ", "180×_MΩ",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("180_MH", "180×_MH",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("1×_A×_kg", "2.2046226218488×_A×_lb",
                {.m_unitDisplay = UnitDisplay::AutomaticImperial});
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
  // Angle
  simplifies_to("180×_°→_rad", "π×_rad", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("0_°→_rad", "0×_rad");
  simplifies_to("180_°→_rad", "π×_rad");
  simplifies_to("π_rad→_'", "10800×_'");
  simplifies_to("1_°+60_'+3600_\"→_°", "3×_°");
  simplifies_to("1°+60'+3600\"→°", "3×_°");

  // Temperature
  simplifies_to("100×_°C→_K", "373.15×_K");
  simplifies_to("-100×_°C→_K", "173.15×_K");
  // TODO: should be 0×_°C
  simplifies_to("273.15×_K→_°C", "-5.6843418860808ᴇ-14×_°C");
  simplifies_to("0_K→_°C", "-273.15×_°C");
  simplifies_to("0_°C→_K", "273.15×_K");
  simplifies_to("_°C→_K", "274.15×_K");
  simplifies_to("0_K→_°F", "-459.67×_°F");
  simplifies_to("0_°F→_K", "255.37222222222×_K");
  simplifies_to("_°F→_K", "255.92777777778×_K");

  simplifies_to("91.44_cm→_yd", "1×_yd");
  simplifies_to("10_m/_s→_km/_h", "36×_km×_h^(-1)");
  simplifies_to("2_m→_km×_m/_m", "0.002×_km");
  simplifies_to("10_m/_s→_km/_h", "36×_km×_h^(-1)");
  simplifies_to("10_m^2→_mm×_km", "10×_mm×_km");
  simplifies_to("2_h+2_min→_s", "7320×_s");
  simplifies_to("2×_kg×_m^2×_s^(-2)→_J", "2×_J");
  simplifies_to("4×_min→_s^3/_s^2", "240×_s");
  simplifies_to("4×_N×3_N×2_N→_N^3", "24×_N^3");
  simplifies_to("-5_cm→_m", "-0.05×_m");
  simplifies_to("-5_cm→_m", "-0.05×_m", {.m_unitFormat = UnitFormat::Imperial});
  simplifies_to("10_m/_s→_km", "undef");
  simplifies_to("_hplanck→_eV×_s", "4.1356676969239ᴇ-15×_s×_eV");

  PoincareTest::SymbolStore symbolStore;
  PoincareTest::store("2_kg→a", symbolStore);
  ProjectionContext replaceSymbolCtx = {
      .m_symbolic = SymbolicComputation::ReplaceAllSymbols,
      .m_context = symbolStore};
  simplifies_to("a→g", "2000×_g", replaceSymbolCtx);
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
  simplifies_to("_lgtn", "1.0160469088×_t",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("_lgtn", "1.12×_shtn",
                {
                    .m_unitFormat = UnitFormat::Imperial,
                    .m_unitDisplay = UnitDisplay::AutomaticImperial,
                });
  simplifies_to("_in", "2.54×_cm",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("_in", "1×_in",
                {
                    .m_unitFormat = UnitFormat::Imperial,
                    .m_unitDisplay = UnitDisplay::AutomaticImperial,
                });
  simplifies_to("_ft", "1×_ft",
                {
                    .m_unitFormat = UnitFormat::Imperial,
                    .m_unitDisplay = UnitDisplay::AutomaticImperial,
                });
  simplifies_to("_yd", "1×_yd",
                {
                    .m_unitFormat = UnitFormat::Imperial,
                    .m_unitDisplay = UnitDisplay::AutomaticImperial,
                });
  simplifies_to("1_qt", "1×_qt",
                {
                    .m_unitFormat = UnitFormat::Imperial,
                    .m_unitDisplay = UnitDisplay::AutomaticImperial,
                });
  simplifies_to("1_qt", "9.46352946×_dL",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::AutomaticMetric});
}

QUIZ_CASE(pcj_simplification_unit_undef) {
  ProjectionContext replaceSymbolCtx = {
      .m_symbolic = SymbolicComputation::ReplaceAllSymbols};

  // Non homogeneous
  simplifies_to("1_m+1_s", "undef");
  simplifies_to("1_m+x", "undef");
  simplifies_to("1_m+0", "undef");
  simplifies_to("1+_s", "undef");
  simplifies_to("0+_A", "undef");
  simplifies_to("i _A", "undef");
  simplifies_to("ln(cos(3)i) _s", "undef", k_cartesianCtx);
  simplifies_to("_A+i", "undef");
  simplifies_to("_m+_s", "undef");
  simplifies_to("_m^2+_m", "undef");
  simplifies_to("acos(_s)", "undef");
  simplifies_to("arcosh(_s)", "undef");
  simplifies_to("arg(_s)", "undef");
  simplifies_to("asin(_s)", "undef");
  simplifies_to("arsinh(_s)", "undef");
  simplifies_to("arctan(_s)", "undef");
  simplifies_to("artanh(_s)", "undef");
  simplifies_to("binomcdf(_s,2,3)", "undef");
  simplifies_to("binomcdf(2,_s,3)", "undef");
  simplifies_to("binomcdf(2,3,_s)", "undef");
  simplifies_to("binomial(_s,2)", "undef");
  simplifies_to("binomial(2,_s)", "undef");
  simplifies_to("binompdf(_s,2,3)", "undef");
  simplifies_to("binompdf(2,_s,3)", "undef");
  simplifies_to("binompdf(2,3,_s)", "undef");
  simplifies_to("conj(_s)", "undef");
  simplifies_to("cos(_s)", "undef");
  simplifies_to("cosh(_s)", "undef");
  simplifies_to("cross(_s,[[1][2][3]])", "undef");
  simplifies_to("det(_s)", "undef");
  simplifies_to("det([[_s]])", "undef");
  simplifies_to("diff(_s,x,0)", "undef");
  simplifies_to("diff(0,x,_s)", "undef");
  simplifies_to("dim(_s)", "undef");
  simplifies_to("dot(_s,[[1][2][3]])", "undef");
  simplifies_to("factor(_s)", "undef");
  simplifies_to("(_s)!", "undef");
  simplifies_to("frac(_s)", "undef");
  simplifies_to("gcd(1,_s)", "undef");
  simplifies_to("gcd(_s,1)", "undef");
  simplifies_to("gcd(1,2,3,_s)", "undef");
  simplifies_to("identity(_s)", "undef");
  simplifies_to("im(_s)", "undef");
  simplifies_to("int(_s,x,0,1)", "undef");
  simplifies_to("int(0,x,_s,1)", "undef");
  simplifies_to("int(0,x,0,_s)", "undef");
  simplifies_to("invbinom(_s,2,3)", "undef");
  simplifies_to("invbinom(2,_s,3)", "undef");
  simplifies_to("invbinom(2,3,_s)", "undef");
  simplifies_to("invnorm(_s,2,3)", "undef");
  simplifies_to("invnorm(2,_s,3)", "undef");
  simplifies_to("invnorm(2,3,_s)", "undef");
  simplifies_to("inverse(_s)", "undef");
  simplifies_to("lcm(1,_s)", "undef");
  simplifies_to("lcm(_s,1)", "undef");
  simplifies_to("lcm(1,2,3,_s)", "undef");
  simplifies_to("ln(_s)", "undef");
  simplifies_to("log(_s)", "undef");
  simplifies_to("log(_s,2)", "undef");
  simplifies_to("log(1,_s)", "undef");
  simplifies_to("log(_N^2,_N)", "undef");
  simplifies_to("norm(_s)", "undef");
  simplifies_to("normcdf(_s,2,3)", "undef");
  simplifies_to("normcdf(2,_s,3)", "undef");
  simplifies_to("normcdf(2,3,_s)", "undef");
  simplifies_to("normcdfrange(_s,2,3,4)", "undef");
  simplifies_to("normcdfrange(2,_s,3,4)", "undef");
  simplifies_to("normcdfrange(2,3,_s,4)", "undef");
  simplifies_to("normcdfrange(2,3,4,_s)", "undef");
  simplifies_to("normpdf(_s,2,3)", "undef");
  simplifies_to("normpdf(2,_s,3)", "undef");
  simplifies_to("normpdf(2,3,_s)", "undef");
  simplifies_to("permute(_s,2)", "undef");
  simplifies_to("permute(2,_s)", "undef");
  simplifies_to("product(_s,x,0,1)", "undef");
  simplifies_to("product(1,x,_s,1)", "undef");
  simplifies_to("product(1,x,0,_s)", "undef");
  simplifies_to("quo(_s,1)", "undef");
  simplifies_to("quo(1,_s)", "undef");
  simplifies_to("re(_s)", "undef");
  simplifies_to("ref(_s)", "undef");
  simplifies_to("ref([[_s]])", "undef");
  simplifies_to("rem(_s,1)", "undef");
  simplifies_to("rem(1,_s)", "undef");
  simplifies_to("round(1,_s)", "undef");
  simplifies_to("round(1_s,_s)", "undef");
  simplifies_to("round(1_s,0.1)", "undef");
  simplifies_to("round(1_s,0.1)+abs(1_s)", "undef");
  simplifies_to("rref(_s)", "undef");
  simplifies_to("sin(_s)", "undef");
  simplifies_to("sinh(_s)", "undef");
  simplifies_to("sum(0,x,_s,1)", "undef");
  simplifies_to("sum(0,x,0,_s)", "undef");
  simplifies_to("tan(_s)", "undef");
  simplifies_to("tanh(_s)", "undef");
  simplifies_to("trace(_s)", "undef");
  simplifies_to("transpose(_s)", "undef");

  /* Keep SI units for infinity float results, Remove unit for undefined
   * expression */
  simplifies_to("inf×_s", "∞×_s");
  simplifies_to("-inf×_oz", "-∞×_oz");
  simplifies_to("2_s+3_s-5_s", "0×_s");
  simplifies_to("log(0)×_s", "undef");
  simplifies_to("log(undef)*_s", "undef");

  // Temperature
  simplifies_to("2_°C-1_°C", "undef");
  simplifies_to("2_°C+2_K", "undef");
  simplifies_to("2_K+2_°C", "undef");
  simplifies_to("2_K×2_°C", "undef");
  simplifies_to("1/_°C", "undef");
  simplifies_to("(1_°C)^2", "undef");
  simplifies_to("_°C+_°C", "undef");
  simplifies_to("_°C+_°F", "undef");
  simplifies_to("_K+_°F", "undef");
#if TODO_PCJ  // Forbid multiplications involving °F and °C
  simplifies_to("2*20_°F", "undef");
  simplifies_to("-1×100×_°C→_K", "undef");
#endif
  simplifies_to("_°C^2", "undef");
  simplifies_to("1/(-3_°C)", "undef");
  simplifies_to("_K×_°C", "undef");

  /* Units with invalid exponent */
  simplifies_to("_s^(_s)", "undef");
  simplifies_to("_s^(π)", "undef");
  // Exponent overflow
  /* NOTE: Order of units should not matter. If an overflow can occur when
   * reordering the multiplication, we return undef, even if no overflow
   * technically happened */
  simplifies_to("_s^127 * _s^(-1) * _s", "undef");
  simplifies_to("_s^127 * _s * _s^(-1)", "undef");

  // Only allow simple int operations in unit exponents
  simplifies_to("_s^floor(1)", "undef");
  simplifies_to("_s^ceil(2)", "undef");

#if TODO_PCJ  // handle non-integer exponents
  simplifies_to("_m^(1/2)", "1×_m^(1/2)");
  simplifies_to("√(_m)", "1×_m^(1/2)");
  simplifies_to("_N^(1/2)", "1×_N^(1/2)");
  simplifies_to("√(_N)", "1×_kg^(1/2)×_m^(1/2)×_s^(-1)");
  simplifies_to("√(_N)",
                "1.5527410012845×_lb^(1/2)×_yd^(1/"
                "2)×_s^(-1)",
                {
                    .m_unitFormat = UnitFormat::Imperial,
                });
  simplifies_to("_C^0.3", "1×_A^(3/10)×_s^(3/10)");
  simplifies_to("_kat_kg^-2.8", "1×_mol×_kg^(-14/5)×_s^(-1)");
  simplifies_to("300000×_m^3→_km^(2.3+0.7)", "3ᴇ-4×_km^3");
#endif

  // Invalid operations
  simplifies_to("ceil(3.3_m)", "undef");
  simplifies_to("floor(_s)", "undef");
  simplifies_to("floor(3.3_m)", "undef");
  simplifies_to("round(3.3_m, 0)", "undef");
  simplifies_to("round(_s,1)", "undef");
  simplifies_to("sign(-2_m)", "undef");
  simplifies_to("sign(4_m)", "undef");
  simplifies_to("sign(_s)", "undef");
#if TODO_PCJ  // handle sqrt
  simplifies_to("√(16×_m^2)", "4×_m");
#endif
  simplifies_to("abs(2_m) + ceil(3_m) + floor(4_m) + round(5_m, 1)", "undef");
  simplifies_to("sign(3_m) + 2", "undef");
  simplifies_to("tan(2_m)", "undef");
  simplifies_to("tan(2_rad^2)", "undef");
  simplifies_to("(_A×x)^2×_s", "undef", replaceSymbolCtx);
}
