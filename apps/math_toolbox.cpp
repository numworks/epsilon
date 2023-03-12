#include "math_toolbox.h"
#include "global_preferences.h"
#include "./shared/toolbox_helpers.h"
#include <assert.h>
#include <string.h>

using namespace Poincare;

/* TODO
 * Unit submenu should be created from the Poincare::Unit::Representative table.
 * This would avoid to store duplicates const char *.
 * This would requires classes as:
 * - PointerTree parent of MessageTree and BufferTree
 * - PointerTableCell instead of MessageTableCell
 *
 * We should add in the model tree the possibility to indicate a Leaf that has
 * to be selected when the menu appears. */

/* We create one model tree: each node keeps the label of the row it refers to
 * and the text which would be edited by clicking on the row. When the node is a
 * subtree, the edited text is set at I18n::Message::Default. */

const ToolboxMessageTree calculChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::DiffCommandWithArg, I18n::Message::DerivateNumber, false, I18n::Message::DiffCommand),
  ToolboxMessageTree::Leaf(I18n::Message::IntCommandWithArg, I18n::Message::Integral, false, I18n::Message::IntCommand),
  ToolboxMessageTree::Leaf(I18n::Message::SumCommandWithArg, I18n::Message::Sum, false, I18n::Message::SumCommand),
  ToolboxMessageTree::Leaf(I18n::Message::ProductCommandWithArg, I18n::Message::Product, false, I18n::Message::ProductCommand),
  ToolboxMessageTree::Leaf(I18n::Message::Infinity, I18n::Message::InfinityMessage, false, I18n::Message::Infinity),
};

const ToolboxMessageTree complexChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::AbsCommandWithArg,I18n::Message::ComplexAbsoluteValue),
  ToolboxMessageTree::Leaf(I18n::Message::ArgCommandWithArg, I18n::Message::Argument),
  ToolboxMessageTree::Leaf(I18n::Message::ReCommandWithArg, I18n::Message::RealPart),
  ToolboxMessageTree::Leaf(I18n::Message::ImCommandWithArg, I18n::Message::ImaginaryPart),
  ToolboxMessageTree::Leaf(I18n::Message::ConjCommandWithArg, I18n::Message::Conjugate)
};

const ToolboxMessageTree combinatoricsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::BinomialCommandWithArg, I18n::Message::Combination),
  ToolboxMessageTree::Leaf(I18n::Message::PermuteCommandWithArg, I18n::Message::Permutation)
};

const ToolboxMessageTree normalDistributionChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::NormCDFCommandWithArg, I18n::Message::NormCDF),
  ToolboxMessageTree::Leaf(I18n::Message::NormCDF2CommandWithArg, I18n::Message::NormCDF2),
  ToolboxMessageTree::Leaf(I18n::Message::InvNormCommandWithArg, I18n::Message::InvNorm),
  ToolboxMessageTree::Leaf(I18n::Message::NormPDFCommandWithArg, I18n::Message::NormPDF)
};

const ToolboxMessageTree binomialDistributionChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::BinomialPDFCommandWithArg, I18n::Message::BinomialPDF),
  ToolboxMessageTree::Leaf(I18n::Message::BinomialCDFCommandWithArg, I18n::Message::BinomialCDF),
  ToolboxMessageTree::Leaf(I18n::Message::InvBinomialCommandWithArg, I18n::Message::InvBinomial),
};

const ToolboxMessageTree probabilityChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::NormalDistribution, normalDistributionChildren),
  ToolboxMessageTree::Node(I18n::Message::BinomialDistribution, binomialDistributionChildren)};

const ToolboxMessageTree arithmeticChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::GcdCommandWithArg, I18n::Message::GreatCommonDivisor),
  ToolboxMessageTree::Leaf(I18n::Message::LcmCommandWithArg, I18n::Message::LeastCommonMultiple),
  ToolboxMessageTree::Leaf(I18n::Message::FactorCommandWithArg, I18n::Message::PrimeFactorDecomposition),
  ToolboxMessageTree::Leaf(I18n::Message::RemCommandWithArg, I18n::Message::Remainder),
  ToolboxMessageTree::Leaf(I18n::Message::QuoCommandWithArg, I18n::Message::Quotient)
};

const ToolboxMessageTree matricesChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::DeterminantCommandWithArg, I18n::Message::Determinant),
  ToolboxMessageTree::Leaf(I18n::Message::InverseCommandWithArg, I18n::Message::Inverse),
  ToolboxMessageTree::Leaf(I18n::Message::IndentityCommandWithArg, I18n::Message::Identity),
  ToolboxMessageTree::Leaf(I18n::Message::TraceCommandWithArg, I18n::Message::Trace),
  ToolboxMessageTree::Leaf(I18n::Message::RowEchelonFormCommandWithArg, I18n::Message::RowEchelonForm),
  ToolboxMessageTree::Leaf(I18n::Message::ReducedRowEchelonFormCommandWithArg, I18n::Message::ReducedRowEchelonForm)
};

const ToolboxMessageTree vectorsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::DotCommandWithArg, I18n::Message::Dot),
  ToolboxMessageTree::Leaf(I18n::Message::CrossCommandWithArg, I18n::Message::Cross),
  ToolboxMessageTree::Leaf(I18n::Message::NormVectorCommandWithArg, I18n::Message::NormVector),
};

const ToolboxMessageTree matricesAndVectorsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::MatrixCommandWithArg, I18n::Message::NewMatrix, false, I18n::Message::MatrixCommand),
  ToolboxMessageTree::Leaf(I18n::Message::TransposeCommandWithArg, I18n::Message::Transpose),
  ToolboxMessageTree::Leaf(I18n::Message::DimensionCommandWithArg, I18n::Message::Dimension),
  ToolboxMessageTree::Node(I18n::Message::Matrices, matricesChildren),
  ToolboxMessageTree::Node(I18n::Message::Vectors, vectorsChildren)
};

#if LIST_ARE_DEFINED
const ToolboxMessageTree listsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::SortCommandWithArg, I18n::Message::Sort),
  ToolboxMessageTree::Leaf(I18n::Message::InvSortCommandWithArg, I18n::Message::InvSort),
  ToolboxMessageTree::Leaf(I18n::Message::MaxCommandWithArg, I18n::Message::Maximum),
  ToolboxMessageTree::Leaf(I18n::Message::MinCommandWithArg, I18n::Message::Minimum),
  ToolboxMessageTree::Leaf(I18n::Message::DimensionCommandWithArg, I18n::Message::Dimension)
};
#endif

const ToolboxMessageTree unitTimeChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeSecondNanoSymbol, I18n::Message::UnitTimeSecondNano),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeSecondMicroSymbol, I18n::Message::UnitTimeSecondMicro),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeSecondMilliSymbol, I18n::Message::UnitTimeSecondMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeSecondSymbol, I18n::Message::UnitTimeSecond),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeMinuteSymbol, I18n::Message::UnitTimeMinute),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeHourSymbol, I18n::Message::UnitTimeHour),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeDaySymbol, I18n::Message::UnitTimeDay),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeWeekSymbol, I18n::Message::UnitTimeWeek),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeMonthSymbol, I18n::Message::UnitTimeMonth),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTimeYearSymbol, I18n::Message::UnitTimeYear)
};

constexpr ToolboxMessageTree unitDistanceMeterPico = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterPicoSymbol, I18n::Message::UnitDistanceMeterPico);
constexpr ToolboxMessageTree unitDistanceMeterNano = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterNanoSymbol, I18n::Message::UnitDistanceMeterNano);
constexpr ToolboxMessageTree unitDistanceMeterMicro = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterMicroSymbol, I18n::Message::UnitDistanceMeterMicro);
constexpr ToolboxMessageTree unitDistanceMeterMilli = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterMilliSymbol, I18n::Message::UnitDistanceMeterMilli);
constexpr ToolboxMessageTree unitDistanceMeter = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterSymbol, I18n::Message::UnitDistanceMeter);
constexpr ToolboxMessageTree unitDistanceMeterKilo = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterKiloSymbol, I18n::Message::UnitDistanceMeterKilo);
constexpr ToolboxMessageTree unitDistanceAstronomicalUnit = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceAstronomicalUnitSymbol, I18n::Message::UnitDistanceAstronomicalUnit);
constexpr ToolboxMessageTree unitDistanceLightYear = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceLightYearSymbol, I18n::Message::UnitDistanceLightYear);
constexpr ToolboxMessageTree unitDistanceParsec = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceParsecSymbol, I18n::Message::UnitDistanceParsec);
constexpr ToolboxMessageTree unitDistanceInch = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceInchSymbol, I18n::Message::UnitDistanceInch);
constexpr ToolboxMessageTree unitDistanceFoot = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceFootSymbol, I18n::Message::UnitDistanceFoot);
constexpr ToolboxMessageTree unitDistanceYard = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceYardSymbol, I18n::Message::UnitDistanceYard);
constexpr ToolboxMessageTree unitDistanceMile = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMileSymbol, I18n::Message::UnitDistanceMile);

const ToolboxMessageTree * unitDistanceMeterChildren[] = {&unitDistanceMeterPico, &unitDistanceMeterNano, &unitDistanceMeterMicro, &unitDistanceMeterMilli, &unitDistanceMeter, &unitDistanceMeterKilo};
const ToolboxMessageTree unitDistanceMeterNode = ToolboxMessageTree::Node(I18n::Message::UnitMetricMenu, unitDistanceMeterChildren);
const ToolboxMessageTree * unitDistanceChildrenForImperialToolbox[] = {
  &unitDistanceInch,
  &unitDistanceFoot,
  &unitDistanceYard,
  &unitDistanceMile,
  &unitDistanceAstronomicalUnit,
  &unitDistanceLightYear,
  &unitDistanceParsec,
  &unitDistanceMeterNode
};
const ToolboxMessageTree * unitDistanceImperialChildren[] = {&unitDistanceInch, &unitDistanceFoot, &unitDistanceYard, &unitDistanceMile};
const ToolboxMessageTree unitDistanceImperialNode = ToolboxMessageTree::Node(I18n::Message::UnitImperialMenu, unitDistanceImperialChildren);
const ToolboxMessageTree * unitDistanceChildrenForMetricToolbox[] = {
  &unitDistanceMeterPico,
  &unitDistanceMeterNano,
  &unitDistanceMeterMicro,
  &unitDistanceMeterMilli,
  &unitDistanceMeter,
  &unitDistanceMeterKilo,
  &unitDistanceAstronomicalUnit,
  &unitDistanceLightYear,
  &unitDistanceParsec,
  &unitDistanceImperialNode
};
const ToolboxMessageTree unitDistanceFork[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitDistanceMenu, unitDistanceChildrenForMetricToolbox),
  ToolboxMessageTree::Node(I18n::Message::UnitDistanceMenu, unitDistanceChildrenForImperialToolbox)
};

constexpr ToolboxMessageTree unitMassGramMicro = ToolboxMessageTree::Leaf(I18n::Message::UnitMassGramMicroSymbol, I18n::Message::UnitMassGramMicro);
constexpr ToolboxMessageTree unitMassGramMilli = ToolboxMessageTree::Leaf(I18n::Message::UnitMassGramMilliSymbol, I18n::Message::UnitMassGramMilli);
constexpr ToolboxMessageTree unitMassGram = ToolboxMessageTree::Leaf(I18n::Message::UnitMassGramSymbol, I18n::Message::UnitMassGram);
constexpr ToolboxMessageTree unitMassGramKilo = ToolboxMessageTree::Leaf(I18n::Message::UnitMassGramKiloSymbol, I18n::Message::UnitMassGramKilo);
constexpr ToolboxMessageTree unitMassTonne = ToolboxMessageTree::Leaf(I18n::Message::UnitMassTonneSymbol, I18n::Message::UnitMassTonne);
constexpr ToolboxMessageTree unitMassOunce = ToolboxMessageTree::Leaf(I18n::Message::UnitMassOunceSymbol, I18n::Message::UnitMassOunce);
constexpr ToolboxMessageTree unitMassPound = ToolboxMessageTree::Leaf(I18n::Message::UnitMassPoundSymbol, I18n::Message::UnitMassPound);
constexpr ToolboxMessageTree unitMassShortTon = ToolboxMessageTree::Leaf(I18n::Message::UnitMassShortTonSymbol, I18n::Message::UnitMassShortTon);
constexpr ToolboxMessageTree unitMassLongTon = ToolboxMessageTree::Leaf(I18n::Message::UnitMassLongTonSymbol, I18n::Message::UnitMassLongTon);

const ToolboxMessageTree * unitMassGramChildren[] = {&unitMassGramMicro, &unitMassGramMilli, &unitMassGram, &unitMassGramKilo, &unitMassTonne};
const ToolboxMessageTree unitMassGramNode = ToolboxMessageTree::Node(I18n::Message::UnitMetricMenu, unitMassGramChildren);
const ToolboxMessageTree * unitMassChildrenForImperialToolbox[] = {
  &unitMassOunce,
  &unitMassPound,
  &unitMassShortTon,
  &unitMassLongTon,
  &unitMassGramNode
};
const ToolboxMessageTree * unitMassImperialChildren[] = {&unitMassOunce, &unitMassPound, &unitMassShortTon, &unitMassLongTon};
const ToolboxMessageTree unitMassImperialNode = ToolboxMessageTree::Node(I18n::Message::UnitImperialMenu, unitMassImperialChildren);
const ToolboxMessageTree * unitMassChildrenForMetricToolbox[] = {
  &unitMassGramMicro,
  &unitMassGramMilli,
  &unitMassGram,
  &unitMassGramKilo,
  &unitMassTonne,
  &unitMassImperialNode
};
const ToolboxMessageTree unitMassFork[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitMassMenu, unitMassChildrenForMetricToolbox),
  ToolboxMessageTree::Node(I18n::Message::UnitMassMenu, unitMassChildrenForImperialToolbox),
};

const ToolboxMessageTree unitCurrentAmpereChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitCurrentAmpereMicroSymbol, I18n::Message::UnitCurrentAmpereMicro),
  ToolboxMessageTree::Leaf(I18n::Message::UnitCurrentAmpereMilliSymbol, I18n::Message::UnitCurrentAmpereMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitCurrentAmpereSymbol, I18n::Message::UnitCurrentAmpere),
};

const ToolboxMessageTree unitTemperatureChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitTemperatureKelvinSymbol, I18n::Message::UnitTemperatureKelvin),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTemperatureCelsiusSymbol, I18n::Message::UnitTemperatureCelsius),
  ToolboxMessageTree::Leaf(I18n::Message::UnitTemperatureFahrenheitSymbol, I18n::Message::UnitTemperatureFahrenheit),
};

const ToolboxMessageTree unitAmountMoleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitAmountMoleMicroSymbol, I18n::Message::UnitAmountMoleMicro),
  ToolboxMessageTree::Leaf(I18n::Message::UnitAmountMoleMilliSymbol, I18n::Message::UnitAmountMoleMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitAmountMoleSymbol, I18n::Message::UnitAmountMole),
};

const ToolboxMessageTree unitLuminousIntensityChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitLuminousIntensityCandelaSymbol, I18n::Message::UnitLuminousIntensityCandela)};


const ToolboxMessageTree unitLuminousFluxChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitLuminousFluxLumenSymbol, I18n::Message::UnitLuminousFluxLumen),
};

const ToolboxMessageTree unitSolidAngleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitSolidAngleSteradianSymbol, I18n::Message::UnitSolidAngleSteradian),
};

const ToolboxMessageTree unitIlluminanceChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitIlluminanceLuxSymbol, I18n::Message::UnitIlluminanceLux),
};

const ToolboxMessageTree unitFrequencyHertzChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitFrequencyHertzSymbol, I18n::Message::UnitFrequencyHertz),
  ToolboxMessageTree::Leaf(I18n::Message::UnitFrequencyHertzKiloSymbol, I18n::Message::UnitFrequencyHertzKilo),
  ToolboxMessageTree::Leaf(I18n::Message::UnitFrequencyHertzMegaSymbol, I18n::Message::UnitFrequencyHertzMega),
  ToolboxMessageTree::Leaf(I18n::Message::UnitFrequencyHertzGigaSymbol, I18n::Message::UnitFrequencyHertzGiga)
};

const ToolboxMessageTree unitForceNewtonChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitForceNewtonMilliSymbol, I18n::Message::UnitForceNewtonMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitForceNewtonSymbol, I18n::Message::UnitForceNewton),
  ToolboxMessageTree::Leaf(I18n::Message::UnitForceNewtonKiloSymbol, I18n::Message::UnitForceNewtonKilo),
};

const ToolboxMessageTree unitPressureChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitPressurePascalSymbol, I18n::Message::UnitPressurePascal),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPressurePascalHectoSymbol, I18n::Message::UnitPressurePascalHecto),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPressureBarSymbol, I18n::Message::UnitPressureBar),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPressureAtmSymbol, I18n::Message::UnitPressureAtm)};

const ToolboxMessageTree unitEnergyJouleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyJouleMilliSymbol, I18n::Message::UnitEnergyJouleMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyJouleSymbol, I18n::Message::UnitEnergyJoule),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyJouleKiloSymbol, I18n::Message::UnitEnergyJouleKilo),
};
const ToolboxMessageTree unitEnergyElectronVoltChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyElectronVoltMilliSymbol, I18n::Message::UnitEnergyElectronVoltMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyElectronVoltSymbol, I18n::Message::UnitEnergyElectronVolt),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyElectronVoltKiloSymbol, I18n::Message::UnitEnergyElectronVoltKilo),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyElectronVoltMegaSymbol, I18n::Message::UnitEnergyElectronVoltMega),
};
const ToolboxMessageTree unitEnergyChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitEnergyJouleMenu, unitEnergyJouleChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitEnergyElectronVoltMenu, unitEnergyElectronVoltChildren)};

const ToolboxMessageTree unitPowerWattChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattMicroSymbol, I18n::Message::UnitPowerWattMicro),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattMilliSymbol, I18n::Message::UnitPowerWattMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattSymbol, I18n::Message::UnitPowerWatt),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattKiloSymbol, I18n::Message::UnitPowerWattKilo),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattMegaSymbol, I18n::Message::UnitPowerWattMega),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattGigaSymbol, I18n::Message::UnitPowerWattGiga),
};

const ToolboxMessageTree unitElectricChargeCoulombChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitChargeCoulombSymbol, I18n::Message::UnitChargeCoulomb),
};


const ToolboxMessageTree unitPotentialVoltChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitPotentialVoltMicroSymbol, I18n::Message::UnitPotentialVoltMicro),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPotentialVoltMilliSymbol, I18n::Message::UnitPotentialVoltMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPotentialVoltSymbol, I18n::Message::UnitPotentialVolt),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPotentialVoltKiloSymbol, I18n::Message::UnitPotentialVoltKilo),
};

const ToolboxMessageTree unitCapacitanceFaradChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitCapacitanceFaradMicroSymbol, I18n::Message::UnitCapacitanceFaradMicro),
  ToolboxMessageTree::Leaf(I18n::Message::UnitCapacitanceFaradMilliSymbol, I18n::Message::UnitCapacitanceFaradMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitCapacitanceFaradSymbol, I18n::Message::UnitCapacitanceFarad),
};

const ToolboxMessageTree unitResistanceOhmChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitResistanceOhmSymbol, I18n::Message::UnitResistanceOhm),
  ToolboxMessageTree::Leaf(I18n::Message::UnitResistanceOhmKiloSymbol, I18n::Message::UnitResistanceOhmKilo),
};

const ToolboxMessageTree unitConductanceSiemensChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitConductanceSiemensMilliSymbol, I18n::Message::UnitConductanceSiemensMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitConductanceSiemensSymbol, I18n::Message::UnitConductanceSiemens),
};

const ToolboxMessageTree unitMagneticFieldChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitMagneticFieldTeslaSymbol, I18n::Message::UnitMagneticFieldTesla)};

const ToolboxMessageTree unitInductanceChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitInductanceHenrySymbol, I18n::Message::UnitInductanceHenry)};

const ToolboxMessageTree unitSurfaceChildrenForMetricToolbox[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitSurfaceHectarSymbol, I18n::Message::UnitSurfaceHectar),
  ToolboxMessageTree::Leaf(I18n::Message::UnitSurfaceAcreSymbol, I18n::Message::UnitSurfaceAcre)
};
const ToolboxMessageTree unitSurfaceChildrenForImperialToolbox[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitSurfaceAcreSymbol, I18n::Message::UnitSurfaceAcre),
  ToolboxMessageTree::Leaf(I18n::Message::UnitSurfaceHectarSymbol, I18n::Message::UnitSurfaceHectar)
};
const ToolboxMessageTree unitSurfaceFork[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitSurfaceMenu, unitSurfaceChildrenForMetricToolbox),
  ToolboxMessageTree::Node(I18n::Message::UnitSurfaceMenu, unitSurfaceChildrenForImperialToolbox)
};

constexpr ToolboxMessageTree unitVolumeLiterMilli = ToolboxMessageTree::Leaf(I18n::Message::UnitVolumeLiterMilliSymbol, I18n::Message::UnitVolumeLiterMilli);
constexpr ToolboxMessageTree unitVolumeLiterCenti = ToolboxMessageTree::Leaf(I18n::Message::UnitVolumeLiterCentiSymbol, I18n::Message::UnitVolumeLiterCenti);
constexpr ToolboxMessageTree unitVolumeLiterDeci = ToolboxMessageTree::Leaf(I18n::Message::UnitVolumeLiterDeciSymbol, I18n::Message::UnitVolumeLiterDeci);
constexpr ToolboxMessageTree unitVolumeLiter = ToolboxMessageTree::Leaf(I18n::Message::UnitVolumeLiterSymbol, I18n::Message::UnitVolumeLiter);
constexpr ToolboxMessageTree unitVolumeTeaspoon = ToolboxMessageTree::Leaf(I18n::Message::UnitVolumeTeaspoonSymbol, I18n::Message::UnitVolumeTeaspoon);
constexpr ToolboxMessageTree unitVolumeTablespoon = ToolboxMessageTree::Leaf(I18n::Message::UnitVolumeTablespoonSymbol, I18n::Message::UnitVolumeTablespoon);
constexpr ToolboxMessageTree unitVolumeFluidOunce = ToolboxMessageTree::Leaf(I18n::Message::UnitVolumeFluidOunceSymbol, I18n::Message::UnitVolumeFluidOunce);
constexpr ToolboxMessageTree unitVolumeCup = ToolboxMessageTree::Leaf(I18n::Message::UnitVolumeCupSymbol, I18n::Message::UnitVolumeCup);
constexpr ToolboxMessageTree unitVolumePint = ToolboxMessageTree::Leaf(I18n::Message::UnitVolumePintSymbol, I18n::Message::UnitVolumePint);
constexpr ToolboxMessageTree unitVolumeQuart = ToolboxMessageTree::Leaf(I18n::Message::UnitVolumeQuartSymbol, I18n::Message::UnitVolumeQuart);
constexpr ToolboxMessageTree unitVolumeGallon = ToolboxMessageTree::Leaf(I18n::Message::UnitVolumeGallonSymbol, I18n::Message::UnitVolumeGallon);

const ToolboxMessageTree * unitVolumeLiterChildren[] = {&unitVolumeLiterMilli, &unitVolumeLiterCenti, &unitVolumeLiterDeci, &unitVolumeLiter};
const ToolboxMessageTree unitVolumeLiterNode = ToolboxMessageTree::Node(I18n::Message::UnitMetricMenu, unitVolumeLiterChildren);
const ToolboxMessageTree * unitVolumeChildrenForImperialToolbox[] = {
  &unitVolumeTeaspoon,
  &unitVolumeTablespoon,
  &unitVolumeFluidOunce,
  &unitVolumeCup,
  &unitVolumePint,
  &unitVolumeQuart,
  &unitVolumeGallon,
  &unitVolumeLiterNode
};
const ToolboxMessageTree * unitVolumeImperialChildren[] = {&unitVolumeTeaspoon, &unitVolumeTablespoon, &unitVolumeFluidOunce, &unitVolumeCup, &unitVolumePint, &unitVolumeQuart, &unitVolumeGallon};
const ToolboxMessageTree unitVolumeImperialNode = ToolboxMessageTree::Node(I18n::Message::UnitImperialMenu, unitVolumeImperialChildren);
const ToolboxMessageTree * unitVolumeChildrenForMetricToolbox[] = {
  &unitVolumeLiterMilli,
  &unitVolumeLiterCenti,
  &unitVolumeLiterDeci,
  &unitVolumeLiter,
  &unitVolumeImperialNode
};
const ToolboxMessageTree unitVolumeFork[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitVolumeMenu, unitVolumeChildrenForMetricToolbox),
  ToolboxMessageTree::Node(I18n::Message::UnitVolumeMenu, unitVolumeChildrenForImperialToolbox),
};

const ToolboxMessageTree unitChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitTimeMenu, unitTimeChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitDistanceMenu, unitDistanceFork, true),
  ToolboxMessageTree::Node(I18n::Message::UnitSurfaceMenu, unitSurfaceFork, true),
  ToolboxMessageTree::Node(I18n::Message::UnitVolumeMenu, unitVolumeFork, true),
  ToolboxMessageTree::Node(I18n::Message::UnitMassMenu, unitMassFork, true),
  ToolboxMessageTree::Node(I18n::Message::UnitCurrentMenu, unitCurrentAmpereChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitTemperatureMenu, unitTemperatureChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitAmountMenu, unitAmountMoleChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitLuminousIntensityMenu, unitLuminousIntensityChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitLuminousFluxMenu, unitLuminousFluxChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitIlluminanceMenu, unitIlluminanceChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitSolidAngleMenu, unitSolidAngleChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitFrequencyMenu, unitFrequencyHertzChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitForceMenu, unitForceNewtonChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitPressureMenu, unitPressureChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitEnergyMenu, unitEnergyChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitPowerMenu, unitPowerWattChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitElectricChargeMenu, unitElectricChargeCoulombChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitPotentialMenu, unitPotentialVoltChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitCapacitanceMenu, unitCapacitanceFaradChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitResistanceMenu, unitResistanceOhmChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitConductanceMenu, unitConductanceSiemensChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitMagneticFieldMenu, unitMagneticFieldChildren),
  ToolboxMessageTree::Node(I18n::Message::InductanceMenu, unitInductanceChildren),
};

const ToolboxMessageTree randomAndApproximationChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::RandomCommandWithArg, I18n::Message::RandomFloat),
  ToolboxMessageTree::Leaf(I18n::Message::RandintCommandWithArg, I18n::Message::RandomInteger),
  ToolboxMessageTree::Leaf(I18n::Message::FloorCommandWithArg, I18n::Message::Floor),
  ToolboxMessageTree::Leaf(I18n::Message::FracCommandWithArg, I18n::Message::FracPart),
  ToolboxMessageTree::Leaf(I18n::Message::CeilCommandWithArg, I18n::Message::Ceiling),
  ToolboxMessageTree::Leaf(I18n::Message::RoundCommandWithArg, I18n::Message::Rounding)};

const ToolboxMessageTree trigonometryChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::CoshCommandWithArg, I18n::Message::HyperbolicCosine),
  ToolboxMessageTree::Leaf(I18n::Message::SinhCommandWithArg, I18n::Message::HyperbolicSine),
  ToolboxMessageTree::Leaf(I18n::Message::TanhCommandWithArg, I18n::Message::HyperbolicTangent),
  ToolboxMessageTree::Leaf(I18n::Message::AcoshCommandWithArg, I18n::Message::InverseHyperbolicCosine),
  ToolboxMessageTree::Leaf(I18n::Message::AsinhCommandWithArg, I18n::Message::InverseHyperbolicSine),
  ToolboxMessageTree::Leaf(I18n::Message::AtanhCommandWithArg, I18n::Message::InverseHyperbolicTangent)};

const ToolboxMessageTree predictionChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::Prediction95CommandWithArg, I18n::Message::Prediction95),
  ToolboxMessageTree::Leaf(I18n::Message::PredictionCommandWithArg, I18n::Message::Prediction),
  ToolboxMessageTree::Leaf(I18n::Message::ConfidenceCommandWithArg, I18n::Message::Confidence)};

const ToolboxMessageTree chemistryMolarMassesByNumber[] = {
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementH, I18n::Message::ElementHMass, false, I18n::Message::ElementHMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementHe, I18n::Message::ElementHeMass, false, I18n::Message::ElementHeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementLi, I18n::Message::ElementLiMass, false, I18n::Message::ElementLiMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementBe, I18n::Message::ElementBeMass, false, I18n::Message::ElementBeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementB, I18n::Message::ElementBMass, false, I18n::Message::ElementBMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementC, I18n::Message::ElementCMass, false, I18n::Message::ElementCMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementN, I18n::Message::ElementNMass, false, I18n::Message::ElementNMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementO, I18n::Message::ElementOMass, false, I18n::Message::ElementOMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementF, I18n::Message::ElementFMass, false, I18n::Message::ElementFMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNe, I18n::Message::ElementNeMass, false, I18n::Message::ElementNeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNa, I18n::Message::ElementNaMass, false, I18n::Message::ElementNaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementMg, I18n::Message::ElementMgMass, false, I18n::Message::ElementMgMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAl, I18n::Message::ElementAlMass, false, I18n::Message::ElementAlMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSi, I18n::Message::ElementSiMass, false, I18n::Message::ElementSiMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementP, I18n::Message::ElementPMass, false, I18n::Message::ElementPMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementS, I18n::Message::ElementSMass, false, I18n::Message::ElementSMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCl, I18n::Message::ElementClMass, false, I18n::Message::ElementClMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAr, I18n::Message::ElementArMass, false, I18n::Message::ElementArMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementK, I18n::Message::ElementKMass, false, I18n::Message::ElementKMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCa, I18n::Message::ElementCaMass, false, I18n::Message::ElementCaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSc, I18n::Message::ElementScMass, false, I18n::Message::ElementScMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTi, I18n::Message::ElementTiMass, false, I18n::Message::ElementTiMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementV, I18n::Message::ElementVMass, false, I18n::Message::ElementVMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCr, I18n::Message::ElementCrMass, false, I18n::Message::ElementCrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementMn, I18n::Message::ElementMnMass, false, I18n::Message::ElementMnMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementFe, I18n::Message::ElementFeMass, false, I18n::Message::ElementFeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCo, I18n::Message::ElementCoMass, false, I18n::Message::ElementCoMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNi, I18n::Message::ElementNiMass, false, I18n::Message::ElementNiMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCu, I18n::Message::ElementCuMass, false, I18n::Message::ElementCuMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementZn, I18n::Message::ElementZnMass, false, I18n::Message::ElementZnMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementGa, I18n::Message::ElementGaMass, false, I18n::Message::ElementGaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementGe, I18n::Message::ElementGeMass, false, I18n::Message::ElementGeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAs, I18n::Message::ElementAsMass, false, I18n::Message::ElementAsMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSe, I18n::Message::ElementSeMass, false, I18n::Message::ElementSeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementBr, I18n::Message::ElementBrMass, false, I18n::Message::ElementBrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementKr, I18n::Message::ElementKrMass, false, I18n::Message::ElementKrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRb, I18n::Message::ElementRbMass, false, I18n::Message::ElementRbMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSr, I18n::Message::ElementSrMass, false, I18n::Message::ElementSrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementY, I18n::Message::ElementYMass, false, I18n::Message::ElementYMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementZr, I18n::Message::ElementZrMass, false, I18n::Message::ElementZrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNb, I18n::Message::ElementNbMass, false, I18n::Message::ElementNbMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementMo, I18n::Message::ElementMoMass, false, I18n::Message::ElementMoMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTc, I18n::Message::ElementTcMass, false, I18n::Message::ElementTcMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRu, I18n::Message::ElementRuMass, false, I18n::Message::ElementRuMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRh, I18n::Message::ElementRhMass, false, I18n::Message::ElementRhMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPd, I18n::Message::ElementPdMass, false, I18n::Message::ElementPdMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAg, I18n::Message::ElementAgMass, false, I18n::Message::ElementAgMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCd, I18n::Message::ElementCdMass, false, I18n::Message::ElementCdMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementIn, I18n::Message::ElementInMass, false, I18n::Message::ElementInMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSn, I18n::Message::ElementSnMass, false, I18n::Message::ElementSnMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSb, I18n::Message::ElementSbMass, false, I18n::Message::ElementSbMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTe, I18n::Message::ElementTeMass, false, I18n::Message::ElementTeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementI, I18n::Message::ElementIMass, false, I18n::Message::ElementIMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementXe, I18n::Message::ElementXeMass, false, I18n::Message::ElementXeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCs, I18n::Message::ElementCsMass, false, I18n::Message::ElementCsMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementBa, I18n::Message::ElementBaMass, false, I18n::Message::ElementBaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementLa, I18n::Message::ElementLaMass, false, I18n::Message::ElementLaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCe, I18n::Message::ElementCeMass, false, I18n::Message::ElementCeMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPr, I18n::Message::ElementPrMass, false, I18n::Message::ElementPrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNd, I18n::Message::ElementNdMass, false, I18n::Message::ElementNdMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPm, I18n::Message::ElementPmMass, false, I18n::Message::ElementPmMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSm, I18n::Message::ElementSmMass, false, I18n::Message::ElementSmMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementEu, I18n::Message::ElementEuMass, false, I18n::Message::ElementEuMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementGd, I18n::Message::ElementGdMass, false, I18n::Message::ElementGdMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTb, I18n::Message::ElementTbMass, false, I18n::Message::ElementTbMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementDy, I18n::Message::ElementDyMass, false, I18n::Message::ElementDyMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementHo, I18n::Message::ElementHoMass, false, I18n::Message::ElementHoMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementEr, I18n::Message::ElementErMass, false, I18n::Message::ElementErMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTm, I18n::Message::ElementTmMass, false, I18n::Message::ElementTmMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementYb, I18n::Message::ElementYbMass, false, I18n::Message::ElementYbMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementLu, I18n::Message::ElementLuMass, false, I18n::Message::ElementLuMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementHf, I18n::Message::ElementHfMass, false, I18n::Message::ElementHfMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTa, I18n::Message::ElementTaMass, false, I18n::Message::ElementTaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementW, I18n::Message::ElementWMass, false, I18n::Message::ElementWMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRe, I18n::Message::ElementReMass, false, I18n::Message::ElementReMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementOs, I18n::Message::ElementOsMass, false, I18n::Message::ElementOsMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementIr, I18n::Message::ElementIrMass, false, I18n::Message::ElementIrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPt, I18n::Message::ElementPtMass, false, I18n::Message::ElementPtMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAu, I18n::Message::ElementAuMass, false, I18n::Message::ElementAuMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementHg, I18n::Message::ElementHgMass, false, I18n::Message::ElementHgMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTl, I18n::Message::ElementTlMass, false, I18n::Message::ElementTlMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPb, I18n::Message::ElementPbMass, false, I18n::Message::ElementPbMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementBi, I18n::Message::ElementBiMass, false, I18n::Message::ElementBiMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPo, I18n::Message::ElementPoMass, false, I18n::Message::ElementPoMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAt, I18n::Message::ElementAtMass, false, I18n::Message::ElementAtMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRn, I18n::Message::ElementRnMass, false, I18n::Message::ElementRnMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementFr, I18n::Message::ElementFrMass, false, I18n::Message::ElementFrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRa, I18n::Message::ElementRaMass, false, I18n::Message::ElementRaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAc, I18n::Message::ElementAcMass, false, I18n::Message::ElementAcMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTh, I18n::Message::ElementThMass, false, I18n::Message::ElementThMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPa, I18n::Message::ElementPaMass, false, I18n::Message::ElementPaMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementU, I18n::Message::ElementUMass, false, I18n::Message::ElementUMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNp, I18n::Message::ElementNpMass, false, I18n::Message::ElementNpMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementPu, I18n::Message::ElementPuMass, false, I18n::Message::ElementPuMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementAm, I18n::Message::ElementAmMass, false, I18n::Message::ElementAmMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCm, I18n::Message::ElementCmMass, false, I18n::Message::ElementCmMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementBk, I18n::Message::ElementBkMass, false, I18n::Message::ElementBkMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCf, I18n::Message::ElementCfMass, false, I18n::Message::ElementCfMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementEs, I18n::Message::ElementEsMass, false, I18n::Message::ElementEsMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementFm, I18n::Message::ElementFmMass, false, I18n::Message::ElementFmMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementMd, I18n::Message::ElementMdMass, false, I18n::Message::ElementMdMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNo, I18n::Message::ElementNoMass, false, I18n::Message::ElementNoMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementLr, I18n::Message::ElementLrMass, false, I18n::Message::ElementLrMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRf, I18n::Message::ElementRfMass, false, I18n::Message::ElementRfMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementDb, I18n::Message::ElementDbMass, false, I18n::Message::ElementDbMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementSg, I18n::Message::ElementSgMass, false, I18n::Message::ElementSgMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementBh, I18n::Message::ElementBhMass, false, I18n::Message::ElementBhMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementHs, I18n::Message::ElementHsMass, false, I18n::Message::ElementHsMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementMt, I18n::Message::ElementMtMass, false, I18n::Message::ElementMtMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementDs, I18n::Message::ElementDsMass, false, I18n::Message::ElementDsMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementRg, I18n::Message::ElementRgMass, false, I18n::Message::ElementRgMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementCn, I18n::Message::ElementCnMass, false, I18n::Message::ElementCnMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementNh, I18n::Message::ElementNhMass, false, I18n::Message::ElementNhMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementFl, I18n::Message::ElementFlMass, false, I18n::Message::ElementFlMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementMc, I18n::Message::ElementMcMass, false, I18n::Message::ElementMcMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementLv, I18n::Message::ElementLvMass, false, I18n::Message::ElementLvMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementTs, I18n::Message::ElementTsMass, false, I18n::Message::ElementTsMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementOg, I18n::Message::ElementOgMass, false, I18n::Message::ElementOgMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementUue, I18n::Message::ElementUueMass, false, I18n::Message::ElementUueMass),
  ToolboxMessageTree::Leaf(I18n::Message::NumberElementUbn, I18n::Message::ElementUbnMass, false, I18n::Message::ElementUbnMass)
};

const ToolboxMessageTree chemistryMolarMassesByAlpha[] = {
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAc, I18n::Message::ElementAcMass, false, I18n::Message::ElementAcMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAg, I18n::Message::ElementAgMass, false, I18n::Message::ElementAgMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAl, I18n::Message::ElementAlMass, false, I18n::Message::ElementAlMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAm, I18n::Message::ElementAmMass, false, I18n::Message::ElementAmMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAr, I18n::Message::ElementArMass, false, I18n::Message::ElementArMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAs, I18n::Message::ElementAsMass, false, I18n::Message::ElementAsMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAt, I18n::Message::ElementAtMass, false, I18n::Message::ElementAtMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementAu, I18n::Message::ElementAuMass, false, I18n::Message::ElementAuMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementB, I18n::Message::ElementBMass, false, I18n::Message::ElementBMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementBa, I18n::Message::ElementBaMass, false, I18n::Message::ElementBaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementBe, I18n::Message::ElementBeMass, false, I18n::Message::ElementBeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementBh, I18n::Message::ElementBhMass, false, I18n::Message::ElementBhMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementBi, I18n::Message::ElementBiMass, false, I18n::Message::ElementBiMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementBk, I18n::Message::ElementBkMass, false, I18n::Message::ElementBkMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementBr, I18n::Message::ElementBrMass, false, I18n::Message::ElementBrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementC, I18n::Message::ElementCMass, false, I18n::Message::ElementCMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCa, I18n::Message::ElementCaMass, false, I18n::Message::ElementCaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCd, I18n::Message::ElementCdMass, false, I18n::Message::ElementCdMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCe, I18n::Message::ElementCeMass, false, I18n::Message::ElementCeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCf, I18n::Message::ElementCfMass, false, I18n::Message::ElementCfMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCl, I18n::Message::ElementClMass, false, I18n::Message::ElementClMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCm, I18n::Message::ElementCmMass, false, I18n::Message::ElementCmMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCn, I18n::Message::ElementCnMass, false, I18n::Message::ElementCnMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCo, I18n::Message::ElementCoMass, false, I18n::Message::ElementCoMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCs, I18n::Message::ElementCsMass, false, I18n::Message::ElementCsMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCr, I18n::Message::ElementCrMass, false, I18n::Message::ElementCrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementCu, I18n::Message::ElementCuMass, false, I18n::Message::ElementCuMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementDb, I18n::Message::ElementDbMass, false, I18n::Message::ElementDbMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementDs, I18n::Message::ElementDsMass, false, I18n::Message::ElementDsMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementDy, I18n::Message::ElementDyMass, false, I18n::Message::ElementDyMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementEr, I18n::Message::ElementErMass, false, I18n::Message::ElementErMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementEs, I18n::Message::ElementEsMass, false, I18n::Message::ElementEsMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementEu, I18n::Message::ElementEuMass, false, I18n::Message::ElementEuMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementF, I18n::Message::ElementFMass, false, I18n::Message::ElementFMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementFe, I18n::Message::ElementFeMass, false, I18n::Message::ElementFeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementFl, I18n::Message::ElementFlMass, false, I18n::Message::ElementFlMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementFm, I18n::Message::ElementFmMass, false, I18n::Message::ElementFmMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementFr, I18n::Message::ElementFrMass, false, I18n::Message::ElementFrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementGa, I18n::Message::ElementGaMass, false, I18n::Message::ElementGaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementGd, I18n::Message::ElementGdMass, false, I18n::Message::ElementGdMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementGe, I18n::Message::ElementGeMass, false, I18n::Message::ElementGeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementH, I18n::Message::ElementHMass, false, I18n::Message::ElementHMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementHe, I18n::Message::ElementHeMass, false, I18n::Message::ElementHeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementHf, I18n::Message::ElementHfMass, false, I18n::Message::ElementHfMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementHg, I18n::Message::ElementHgMass, false, I18n::Message::ElementHgMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementHo, I18n::Message::ElementHoMass, false, I18n::Message::ElementHoMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementHs, I18n::Message::ElementHsMass, false, I18n::Message::ElementHsMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementI, I18n::Message::ElementIMass, false, I18n::Message::ElementIMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementIn, I18n::Message::ElementInMass, false, I18n::Message::ElementInMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementIr, I18n::Message::ElementIrMass, false, I18n::Message::ElementIrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementK, I18n::Message::ElementKMass, false, I18n::Message::ElementKMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementKr, I18n::Message::ElementKrMass, false, I18n::Message::ElementKrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementLa, I18n::Message::ElementLaMass, false, I18n::Message::ElementLaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementLi, I18n::Message::ElementLiMass, false, I18n::Message::ElementLiMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementLr, I18n::Message::ElementLrMass, false, I18n::Message::ElementLrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementLu, I18n::Message::ElementLuMass, false, I18n::Message::ElementLuMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementLv, I18n::Message::ElementLvMass, false, I18n::Message::ElementLvMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementMc, I18n::Message::ElementMcMass, false, I18n::Message::ElementMcMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementMd, I18n::Message::ElementMdMass, false, I18n::Message::ElementMdMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementMg, I18n::Message::ElementMgMass, false, I18n::Message::ElementMgMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementMn, I18n::Message::ElementMnMass, false, I18n::Message::ElementMnMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementMo, I18n::Message::ElementMoMass, false, I18n::Message::ElementMoMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementMt, I18n::Message::ElementMtMass, false, I18n::Message::ElementMtMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementN, I18n::Message::ElementNMass, false, I18n::Message::ElementNMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNa, I18n::Message::ElementNaMass, false, I18n::Message::ElementNaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNb, I18n::Message::ElementNbMass, false, I18n::Message::ElementNbMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNd, I18n::Message::ElementNdMass, false, I18n::Message::ElementNdMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNe, I18n::Message::ElementNeMass, false, I18n::Message::ElementNeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNh, I18n::Message::ElementNhMass, false, I18n::Message::ElementNhMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNi, I18n::Message::ElementNiMass, false, I18n::Message::ElementNiMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNo, I18n::Message::ElementNoMass, false, I18n::Message::ElementNoMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementNp, I18n::Message::ElementNpMass, false, I18n::Message::ElementNpMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementO, I18n::Message::ElementOMass, false, I18n::Message::ElementOMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementOg, I18n::Message::ElementOgMass, false, I18n::Message::ElementOgMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementOs, I18n::Message::ElementOsMass, false, I18n::Message::ElementOsMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementP, I18n::Message::ElementPMass, false, I18n::Message::ElementPMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPa, I18n::Message::ElementPaMass, false, I18n::Message::ElementPaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPb, I18n::Message::ElementPbMass, false, I18n::Message::ElementPbMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPd, I18n::Message::ElementPdMass, false, I18n::Message::ElementPdMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPm, I18n::Message::ElementPmMass, false, I18n::Message::ElementPmMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPo, I18n::Message::ElementPoMass, false, I18n::Message::ElementPoMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPr, I18n::Message::ElementPrMass, false, I18n::Message::ElementPrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPt, I18n::Message::ElementPtMass, false, I18n::Message::ElementPtMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementPu, I18n::Message::ElementPuMass, false, I18n::Message::ElementPuMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRa, I18n::Message::ElementRaMass, false, I18n::Message::ElementRaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRb, I18n::Message::ElementRbMass, false, I18n::Message::ElementRbMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRe, I18n::Message::ElementReMass, false, I18n::Message::ElementReMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRf, I18n::Message::ElementRfMass, false, I18n::Message::ElementRfMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRg, I18n::Message::ElementRgMass, false, I18n::Message::ElementRgMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRh, I18n::Message::ElementRhMass, false, I18n::Message::ElementRhMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRn, I18n::Message::ElementRnMass, false, I18n::Message::ElementRnMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementRu, I18n::Message::ElementRuMass, false, I18n::Message::ElementRuMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementS, I18n::Message::ElementSMass, false, I18n::Message::ElementSMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSb, I18n::Message::ElementSbMass, false, I18n::Message::ElementSbMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSc, I18n::Message::ElementScMass, false, I18n::Message::ElementScMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSe, I18n::Message::ElementSeMass, false, I18n::Message::ElementSeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSg, I18n::Message::ElementSgMass, false, I18n::Message::ElementSgMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSi, I18n::Message::ElementSiMass, false, I18n::Message::ElementSiMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSm, I18n::Message::ElementSmMass, false, I18n::Message::ElementSmMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSn, I18n::Message::ElementSnMass, false, I18n::Message::ElementSnMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementSr, I18n::Message::ElementSrMass, false, I18n::Message::ElementSrMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTa, I18n::Message::ElementTaMass, false, I18n::Message::ElementTaMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTb, I18n::Message::ElementTbMass, false, I18n::Message::ElementTbMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTc, I18n::Message::ElementTcMass, false, I18n::Message::ElementTcMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTe, I18n::Message::ElementTeMass, false, I18n::Message::ElementTeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTh, I18n::Message::ElementThMass, false, I18n::Message::ElementThMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTi, I18n::Message::ElementTiMass, false, I18n::Message::ElementTiMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTl, I18n::Message::ElementTlMass, false, I18n::Message::ElementTlMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTm, I18n::Message::ElementTmMass, false, I18n::Message::ElementTmMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementTs, I18n::Message::ElementTsMass, false, I18n::Message::ElementTsMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementU, I18n::Message::ElementUMass, false, I18n::Message::ElementUMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementUue, I18n::Message::ElementUueMass, false, I18n::Message::ElementUueMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementUbn, I18n::Message::ElementUbnMass, false, I18n::Message::ElementUbnMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementV, I18n::Message::ElementVMass, false, I18n::Message::ElementVMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementW, I18n::Message::ElementWMass, false, I18n::Message::ElementWMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementXe, I18n::Message::ElementXeMass, false, I18n::Message::ElementXeMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementY, I18n::Message::ElementYMass, false, I18n::Message::ElementYMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementYb, I18n::Message::ElementYbMass, false, I18n::Message::ElementYbMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementZn, I18n::Message::ElementZnMass, false, I18n::Message::ElementZnMass),
  ToolboxMessageTree::Leaf(I18n::Message::AlphaElementZr, I18n::Message::ElementZrMass, false, I18n::Message::ElementZrMass)
};

const ToolboxMessageTree Pka[] = {
  ToolboxMessageTree::Leaf(I18n::Message::Pka01, I18n::Message::Pka01Value, false, I18n::Message::Pka01Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka02, I18n::Message::Pka02Value, false, I18n::Message::Pka02Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka03, I18n::Message::Pka03Value, false, I18n::Message::Pka03Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka04, I18n::Message::Pka04Value, false, I18n::Message::Pka04Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka05, I18n::Message::Pka05Value, false, I18n::Message::Pka05Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka06, I18n::Message::Pka06Value, false, I18n::Message::Pka06Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka07, I18n::Message::Pka07Value, false, I18n::Message::Pka07Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka08, I18n::Message::Pka08Value, false, I18n::Message::Pka08Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka09, I18n::Message::Pka09Value, false, I18n::Message::Pka09Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka10, I18n::Message::Pka10Value, false, I18n::Message::Pka10Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka11, I18n::Message::Pka11Value, false, I18n::Message::Pka11Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka12, I18n::Message::Pka12Value, false, I18n::Message::Pka12Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka13, I18n::Message::Pka13Value, false, I18n::Message::Pka13Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka14, I18n::Message::Pka14Value, false, I18n::Message::Pka14Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka15, I18n::Message::Pka15Value, false, I18n::Message::Pka15Value),
  ToolboxMessageTree::Leaf(I18n::Message::Pka16, I18n::Message::Pka16Value, false, I18n::Message::Pka16Value),
  ToolboxMessageTree::Leaf(I18n::Message::UltimateAnswer, I18n::Message::UltimateAnswerValue, false, I18n::Message::UltimateAnswerValue),
};

const ToolboxMessageTree chemistry[] = {
  ToolboxMessageTree::Node(I18n::Message::MolarMassesByNumber, chemistryMolarMassesByNumber),
  ToolboxMessageTree::Node(I18n::Message::MolarMassesByAlpha, chemistryMolarMassesByAlpha),
  ToolboxMessageTree::Node(I18n::Message::Pka, Pka),
};

const ToolboxMessageTree FundamentalConstants[] = {
  ToolboxMessageTree::Leaf(I18n::Message::SpeedOfLightTag, I18n::Message::SpeedOfLight, false, I18n::Message::SpeedOfLight),
  ToolboxMessageTree::Leaf(I18n::Message::Vacuum_permittivityTag, I18n::Message::Vacuum_permittivity, false, I18n::Message::Vacuum_permittivity),
  ToolboxMessageTree::Leaf(I18n::Message::Vacuum_permeabilityTag, I18n::Message::Vacuum_permeability, false, I18n::Message::Vacuum_permeability),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckTag, I18n::Message::Planck, false, I18n::Message::Planck),
  ToolboxMessageTree::Leaf(I18n::Message::VacuumImpedanceTag, I18n::Message::VacuumImpedance, false, I18n::Message::VacuumImpedance),
};

const ToolboxMessageTree SpeedsOfSound[] = {
  ToolboxMessageTree::Leaf(I18n::Message::SpeedOfSound0Tag, I18n::Message::SpeedOfSound0, false, I18n::Message::SpeedOfSound0),
  ToolboxMessageTree::Leaf(I18n::Message::SpeedOfSoundWaterTag, I18n::Message::SpeedOfSoundWater, false, I18n::Message::SpeedOfSoundWater),
  ToolboxMessageTree::Leaf(I18n::Message::SpeedOfSoundGlassTag, I18n::Message::SpeedOfSoundGlass, false, I18n::Message::SpeedOfSoundGlass),
  ToolboxMessageTree::Leaf(I18n::Message::SpeedOfSoundSteelTag, I18n::Message::SpeedOfSoundSteel, false, I18n::Message::SpeedOfSoundSteel)
};

const ToolboxMessageTree EscapeVelocities[] = {
    ToolboxMessageTree::Leaf(I18n::Message::EscapeVelocityFromMoon, I18n::Message::EscapeVelocityOfMoon, false, I18n::Message::EscapeVelocityOfMoon),
  ToolboxMessageTree::Leaf(I18n::Message::EscapeVelocityFromEarth, I18n::Message::EscapeVelocityOfEarth, false, I18n::Message::EscapeVelocityOfEarth),
  ToolboxMessageTree::Leaf(I18n::Message::EscapeVelocityFromSun, I18n::Message::EscapeVelocityOfSun, false, I18n::Message::EscapeVelocityOfSun),
};

const ToolboxMessageTree Speed[] = {
  ToolboxMessageTree::Node(I18n::Message::SpeedOfSound, SpeedsOfSound),
  ToolboxMessageTree::Node(I18n::Message::EscapeVelocity, EscapeVelocities)
};

const ToolboxMessageTree Thermodynamics[] =  {
  ToolboxMessageTree::Leaf(I18n::Message::BoltzmannTag,I18n::Message::Boltzmann, false, I18n::Message::Boltzmann),
  ToolboxMessageTree::Leaf(I18n::Message::AvogadroTag, I18n::Message::Avogadro, false, I18n::Message::Avogadro),
  ToolboxMessageTree::Leaf(I18n::Message::GasTag,I18n::Message::Gas, false, I18n::Message::Gas),
  ToolboxMessageTree::Leaf(I18n::Message::StefanBoltzmannTag, I18n::Message::StefanBoltzmann, false, I18n::Message::StefanBoltzmann),
  ToolboxMessageTree::Leaf(I18n::Message::WaterTriplePointTag, I18n::Message::WaterTriplePoint, false, I18n::Message::WaterTriplePoint),
  ToolboxMessageTree::Leaf(I18n::Message::WienTag, I18n::Message::Wien, false, I18n::Message::Wien),
  ToolboxMessageTree::Leaf(I18n::Message::AtmosphericPressureTag, I18n::Message::AtmosphericPressure, false, I18n::Message::AtmosphericPressure),
};

const ToolboxMessageTree Electromagnetism[] = {
  ToolboxMessageTree::Leaf(I18n::Message::CoulombTag, I18n::Message::Coulomb, false, I18n::Message::Coulomb),
  ToolboxMessageTree::Leaf(I18n::Message::ElementalChargeTag, I18n::Message::ElementalCharge, false, I18n::Message::ElementalCharge),
  ToolboxMessageTree::Leaf(I18n::Message::FaradayConstantTag, I18n::Message::FaradayConstant, false, I18n::Message::FaradayConstant),

};

const ToolboxMessageTree Resistivity[] = {
  ToolboxMessageTree::Leaf(I18n::Message::Silver, I18n::Message::Rstvt_Silver, false, I18n::Message::Rstvt_Silver),
  ToolboxMessageTree::Leaf(I18n::Message::Copper, I18n::Message::Rstvt_Copper, false, I18n::Message::Rstvt_Copper),
  ToolboxMessageTree::Leaf(I18n::Message::Gold, I18n::Message::Rstvt_Gold, false, I18n::Message::Rstvt_Gold),
  ToolboxMessageTree::Leaf(I18n::Message::Aluminium, I18n::Message::Rstvt_Aluminium, false, I18n::Message::Rstvt_Aluminium),
  ToolboxMessageTree::Leaf(I18n::Message::Calcium, I18n::Message::Rstvt_Calcium, false, I18n::Message::Rstvt_Calcium),
  ToolboxMessageTree::Leaf(I18n::Message::Tungsten, I18n::Message::Rstvt_Tungsten, false, I18n::Message::Rstvt_Tungsten),
  ToolboxMessageTree::Leaf(I18n::Message::Zinc, I18n::Message::Rstvt_Zinc, false, I18n::Message::Rstvt_Zinc),
  ToolboxMessageTree::Leaf(I18n::Message::Cobalt, I18n::Message::Rstvt_Cobalt, false, I18n::Message::Rstvt_Cobalt),
  ToolboxMessageTree::Leaf(I18n::Message::Nickel, I18n::Message::Rstvt_Nickel, false, I18n::Message::Rstvt_Nickel),
  ToolboxMessageTree::Leaf(I18n::Message::Lithium, I18n::Message::Rstvt_Lithium, false, I18n::Message::Rstvt_Lithium),
  ToolboxMessageTree::Leaf(I18n::Message::Iron, I18n::Message::Rstvt_Iron, false, I18n::Message::Rstvt_Iron),
  ToolboxMessageTree::Leaf(I18n::Message::Platinum, I18n::Message::Rstvt_Platinum, false, I18n::Message::Rstvt_Platinum),
  ToolboxMessageTree::Leaf(I18n::Message::Tin, I18n::Message::Rstvt_Tin, false, I18n::Message::Rstvt_Tin),
  ToolboxMessageTree::Leaf(I18n::Message::Sea_water, I18n::Message::Rstvt_Sea_water, false, I18n::Message::Rstvt_Sea_water),
  ToolboxMessageTree::Leaf(I18n::Message::Water, I18n::Message::Rstvt_Water, false, I18n::Message::Rstvt_Water),
  ToolboxMessageTree::Leaf(I18n::Message::Air, I18n::Message::Rstvt_Air, false, I18n::Message::Rstvt_Air),
  ToolboxMessageTree::Leaf(I18n::Message::Wood, I18n::Message::Rstvt_Wood, false, I18n::Message::Rstvt_Wood),
  ToolboxMessageTree::Leaf(I18n::Message::Glass, I18n::Message::Rstvt_Glass, false, I18n::Message::Rstvt_Glass)
};

const ToolboxMessageTree Conductivity[] = {
  ToolboxMessageTree::Leaf(I18n::Message::Silver, I18n::Message::Cndcvt_Silver, false, I18n::Message::Cndcvt_Silver),
  ToolboxMessageTree::Leaf(I18n::Message::Copper, I18n::Message::Cndcvt_Copper, false, I18n::Message::Cndcvt_Copper),
  ToolboxMessageTree::Leaf(I18n::Message::Gold, I18n::Message::Cndcvt_Gold, false, I18n::Message::Cndcvt_Gold),
  ToolboxMessageTree::Leaf(I18n::Message::Aluminium, I18n::Message::Cndcvt_Aluminium, false, I18n::Message::Cndcvt_Aluminium),
  ToolboxMessageTree::Leaf(I18n::Message::Calcium, I18n::Message::Cndcvt_Calcium, false, I18n::Message::Cndcvt_Calcium),
  ToolboxMessageTree::Leaf(I18n::Message::Tungsten, I18n::Message::Cndcvt_Tungsten, false, I18n::Message::Cndcvt_Tungsten),
  ToolboxMessageTree::Leaf(I18n::Message::Zinc, I18n::Message::Cndcvt_Zinc, false, I18n::Message::Cndcvt_Zinc),
  ToolboxMessageTree::Leaf(I18n::Message::Cobalt, I18n::Message::Cndcvt_Cobalt, false, I18n::Message::Cndcvt_Cobalt),
  ToolboxMessageTree::Leaf(I18n::Message::Nickel, I18n::Message::Cndcvt_Nickel, false, I18n::Message::Cndcvt_Nickel),
  ToolboxMessageTree::Leaf(I18n::Message::Lithium, I18n::Message::Cndcvt_Lithium, false, I18n::Message::Cndcvt_Lithium),
  ToolboxMessageTree::Leaf(I18n::Message::Iron, I18n::Message::Cndcvt_Iron, false, I18n::Message::Cndcvt_Iron),
  ToolboxMessageTree::Leaf(I18n::Message::Platinum, I18n::Message::Cndcvt_Platinum, false, I18n::Message::Cndcvt_Platinum),
  ToolboxMessageTree::Leaf(I18n::Message::Tin, I18n::Message::Cndcvt_Tin, false, I18n::Message::Cndcvt_Tin),
  ToolboxMessageTree::Leaf(I18n::Message::Sea_water, I18n::Message::Cndcvt_Sea_water, false, I18n::Message::Cndcvt_Sea_water),
  ToolboxMessageTree::Leaf(I18n::Message::Water, I18n::Message::Cndcvt_Water, false, I18n::Message::Cndcvt_Water),
  ToolboxMessageTree::Leaf(I18n::Message::Air, I18n::Message::Cndcvt_Air, false, I18n::Message::Cndcvt_Air),
  ToolboxMessageTree::Leaf(I18n::Message::Wood, I18n::Message::Cndcvt_Wood, false, I18n::Message::Cndcvt_Wood),
  ToolboxMessageTree::Leaf(I18n::Message::Glass, I18n::Message::Cndcvt_Glass, false, I18n::Message::Cndcvt_Glass)
};

const ToolboxMessageTree Electricity[] = {
  ToolboxMessageTree::Leaf(I18n::Message::ElementalChargeTag, I18n::Message::ElementalCharge, false, I18n::Message::ElementalCharge),
  ToolboxMessageTree::Node(I18n::Message::ResistivityConstants, Resistivity),
  ToolboxMessageTree::Node(I18n::Message::ConductivityConstants, Conductivity)
};

const ToolboxMessageTree ParticleMass[] = {
  ToolboxMessageTree::Leaf(I18n::Message::ElectronMassTag, I18n::Message::ElectronMass, false, I18n::Message::ElectronMass),
  ToolboxMessageTree::Leaf(I18n::Message::MuonMassTag, I18n::Message::MuonMass, false, I18n::Message::MuonMass),
  ToolboxMessageTree::Leaf(I18n::Message::NeutronMassTag, I18n::Message::NeutronMass, false, I18n::Message::NeutronMass),
  ToolboxMessageTree::Leaf(I18n::Message::ProtonMassTag, I18n::Message::ProtonMass, false, I18n::Message::ProtonMass),
  ToolboxMessageTree::Leaf(I18n::Message::TauonMassTag, I18n::Message::TauonMass, false, I18n::Message::TauonMass),
  ToolboxMessageTree::Leaf(I18n::Message::WBosonMassTag, I18n::Message::WBosonMass, false, I18n::Message::WBosonMass),
  ToolboxMessageTree::Leaf(I18n::Message::ZBosonMassTag, I18n::Message::ZBosonMass, false, I18n::Message::ZBosonMass),
  ToolboxMessageTree::Leaf(I18n::Message::AtomicMassUnitTag, I18n::Message::AtomicMassUnit, false, I18n::Message::AtomicMassUnit),
};


const ToolboxMessageTree Nuclear[] = {
  ToolboxMessageTree::Leaf(I18n::Message::FineStructureTag, I18n::Message::FineStructure, false, I18n::Message::FineStructure),
  ToolboxMessageTree::Leaf(I18n::Message::RydbergConstantTag, I18n::Message::RydbergConstant, false, I18n::Message::RydbergConstant),
  ToolboxMessageTree::Leaf(I18n::Message::HartreeConstantTag, I18n::Message::HartreeConstant, false, I18n::Message::HartreeConstant),
  ToolboxMessageTree::Leaf(I18n::Message::MagneticFluxQuantumTag, I18n::Message::MagneticFluxQuantum, false, I18n::Message::MagneticFluxQuantum),
  ToolboxMessageTree::Leaf(I18n::Message::ConductanceQuantumTag, I18n::Message::ConductanceQuantum, false, I18n::Message::ConductanceQuantum),
  ToolboxMessageTree::Leaf(I18n::Message::CirculationQuantumTag, I18n::Message::CirculationQuantum, false, I18n::Message::CirculationQuantum),
  ToolboxMessageTree::Leaf(I18n::Message::BohrRadiusTag, I18n::Message::BohrRadius, false, I18n::Message::BohrRadius),
  ToolboxMessageTree::Leaf(I18n::Message::BohrMagnetonTag, I18n::Message::BohrMagneton, false, I18n::Message::BohrMagneton),
  ToolboxMessageTree::Leaf(I18n::Message::NuclearMagnetonTag, I18n::Message::NuclearMagneton, false, I18n::Message::NuclearMagneton),
  ToolboxMessageTree::Node(I18n::Message::ParticleMass, ParticleMass),
};

const ToolboxMessageTree Gravitation[] = {
  ToolboxMessageTree::Leaf(I18n::Message::GAccelerationTag, I18n::Message::GAcceleration, false, I18n::Message::GAcceleration),
  ToolboxMessageTree::Leaf(I18n::Message::GConstantTag, I18n::Message::GConstant, false, I18n::Message::GConstant),
};


const ToolboxMessageTree Radiuses[] = {
  ToolboxMessageTree::Leaf(I18n::Message::SunMassTag, I18n::Message::SunRadius, false, I18n::Message::SunRadius),
  ToolboxMessageTree::Leaf(I18n::Message::EarthMassTag, I18n::Message::EarthRadius, false, I18n::Message::EarthRadius),
  ToolboxMessageTree::Leaf(I18n::Message::MoonMassTag, I18n::Message::MoonRadius, false, I18n::Message::MoonRadius),

};

const ToolboxMessageTree Distances[] = {
  ToolboxMessageTree::Leaf(I18n::Message::EarthMoonDistanceTag, I18n::Message::EarthMoonDistance, false, I18n::Message::EarthMoonDistance),
    ToolboxMessageTree::Leaf(I18n::Message::EarthSunDistanceTag, I18n::Message::EarthSunDistance, false, I18n::Message::EarthSunDistance),
};

const ToolboxMessageTree Length[] = {
  ToolboxMessageTree::Node(I18n::Message::Radiuses, Radiuses),
  ToolboxMessageTree::Node(I18n::Message::Distances, Distances),
};

const ToolboxMessageTree Mass[] = {
  ToolboxMessageTree::Leaf(I18n::Message::SunMassTag, I18n::Message::SunMass, false, I18n::Message::SunMass),
  ToolboxMessageTree::Leaf(I18n::Message::EarthMassTag, I18n::Message::EarthMass, false, I18n::Message::EarthMass),
  ToolboxMessageTree::Leaf(I18n::Message::MoonMassTag, I18n::Message::MoonMass, false, I18n::Message::MoonMass),
};


const ToolboxMessageTree PlanckUnits[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PlanckReduceTag, I18n::Message::PlanckReduce, false, I18n::Message::PlanckReduce),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckMassTag, I18n::Message::PlanckMass, false, I18n::Message::PlanckMass),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckLengthTag, I18n::Message::PlanckLength, false, I18n::Message::PlanckLength),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckTimeTag, I18n::Message::PlanckTime, false, I18n::Message::PlanckTime),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckTemperatureTag, I18n::Message::PlanckTemperature, false, I18n::Message::PlanckTemperature),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckChargeTag, I18n::Message::PlanckCharge, false, I18n::Message::PlanckCharge),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckForceTag, I18n::Message::PlanckForce, false, I18n::Message::PlanckForce),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckEnergyTag, I18n::Message::PlanckEnergy, false, I18n::Message::PlanckEnergy),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckPowerTag, I18n::Message::PlanckPower, false, I18n::Message::PlanckPower),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckDensityTag, I18n::Message::PlanckDensity, false, I18n::Message::PlanckDensity),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckQuantityMovementTag, I18n::Message::PlanckQuantityMovement, false, I18n::Message::PlanckQuantityMovement),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckLinearMassTag, I18n::Message::PlanckLinearMass, false, I18n::Message::PlanckLinearMass),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckTensionTag, I18n::Message::PlanckTension, false, I18n::Message::PlanckTension),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckCurrentTag, I18n::Message::PlanckCurrent, false, I18n::Message::PlanckCurrent),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckPressureTag, I18n::Message::PlanckPressure, false, I18n::Message::PlanckPressure),
  ToolboxMessageTree::Leaf(I18n::Message::PlanckImpedanceTag, I18n::Message::PlanckImpedance, false, I18n::Message::PlanckImpedance),
};

const ToolboxMessageTree Physics[] = {
  ToolboxMessageTree::Node(I18n::Message::FundamentalConstants, FundamentalConstants),
  ToolboxMessageTree::Node(I18n::Message::Electromagnetism, Electromagnetism),
  ToolboxMessageTree::Node(I18n::Message::Electricity, Electricity),
  ToolboxMessageTree::Node(I18n::Message::NuclearConstants, Nuclear),
  ToolboxMessageTree::Node(I18n::Message::Thermodynamics, Thermodynamics),
  ToolboxMessageTree::Node(I18n::Message::Gravitation, Gravitation),
  ToolboxMessageTree::Node(I18n::Message::Speed, Speed),
  ToolboxMessageTree::Node(I18n::Message::Mass, Mass),
  ToolboxMessageTree::Node(I18n::Message::Length, Length),
  ToolboxMessageTree::Node(I18n::Message::PlanckUnitsTag, PlanckUnits),
};



const ToolboxMessageTree menu[] = {
  ToolboxMessageTree::Leaf(I18n::Message::AbsCommandWithArg, I18n::Message::AbsoluteValue),
  ToolboxMessageTree::Leaf(I18n::Message::RootCommandWithArg, I18n::Message::NthRoot),
  ToolboxMessageTree::Leaf(I18n::Message::LogCommandWithArg, I18n::Message::BasedLogarithm),
  ToolboxMessageTree::Node(I18n::Message::Calculation, calculChildren),
  ToolboxMessageTree::Node(I18n::Message::ComplexNumber, complexChildren),
  ToolboxMessageTree::Node(I18n::Message::Unit, unitChildren),
  ToolboxMessageTree::Node(I18n::Message::Arithmetic, arithmeticChildren),
  ToolboxMessageTree::Node(I18n::Message::MatricesAndVectors, matricesAndVectorsChildren),
  ToolboxMessageTree::Node(I18n::Message::Probability, probabilityChildren),
#if LIST_ARE_DEFINED
  ToolboxMessageTree::Node(I18n::Message::Lists,listsChildren),
#endif
  ToolboxMessageTree::Node(I18n::Message::Combinatorics, combinatoricsChildren),
  ToolboxMessageTree::Node(I18n::Message::RandomAndApproximation, randomAndApproximationChildren),
  ToolboxMessageTree::Node(I18n::Message::HyperbolicTrigonometry, trigonometryChildren),
  ToolboxMessageTree::Node(I18n::Message::Fluctuation, predictionChildren),
  ToolboxMessageTree::Node(I18n::Message::Chemistry, chemistry),
  ToolboxMessageTree::Node(I18n::Message::Physics, Physics)
  };

const ToolboxMessageTree toolboxModel = ToolboxMessageTree::Node(I18n::Message::Toolbox, menu);

MathToolbox::MathToolbox() :
  Toolbox(nullptr, rootModel()->label())
{
  for (int i=0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].setMessageFont(KDFont::LargeFont);
    m_nodeCells[i].setMessageFont(KDFont::LargeFont);
  }
}

bool MathToolbox::selectLeaf(int selectedRow, bool quitToolbox) {
  ToolboxMessageTree * messageTree = (ToolboxMessageTree *)m_messageTreeModel->childAtIndex(selectedRow);
  m_selectableTableView.deselectTable();

  // Translate the message
  const char * text = I18n::translate(messageTree->insertedText());
  char textToInsert[k_maxMessageSize]; // Has to be in the same scope as handleEventWithText
  if (messageTree->label() == messageTree->insertedText()) {
  //  Remove the arguments if we kept one message for both inserted and displayed message
    int maxTextToInsertLength = strlen(text) + 1;
    assert(maxTextToInsertLength <= k_maxMessageSize);
    Shared::ToolboxHelpers::TextToInsertForCommandText(text, -1, textToInsert, maxTextToInsertLength, true);
    text = textToInsert;
  }
  sender()->handleEventWithText(text);
  Container::activeApp()->dismissModalViewController();
  return true;
}

const ToolboxMessageTree * MathToolbox::rootModel() const {
  return &toolboxModel;
}

MessageTableCellWithMessage<SlideableMessageTextView> * MathToolbox::leafCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

MessageTableCellWithChevron<SlideableMessageTextView> * MathToolbox::nodeCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_nodeCells[index];
}

int MathToolbox::maxNumberOfDisplayedRows() {
 return k_maxNumberOfDisplayedRows;
}

int MathToolbox::indexAfterFork() const {
    Preferences::UnitFormat unitFormat = GlobalPreferences::sharedGlobalPreferences()->unitFormat();
    if (unitFormat == Preferences::UnitFormat::Metric) {
      return 0;
    }
    assert(unitFormat == Preferences::UnitFormat::Imperial);
    return 1;
}

