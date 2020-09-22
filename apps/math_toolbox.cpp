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
  ToolboxMessageTree::Leaf(I18n::Message::ProductCommandWithArg, I18n::Message::Product, false, I18n::Message::ProductCommand)
};

const ToolboxMessageTree complexChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::AbsCommandWithArg,I18n::Message::ComplexAbsoluteValue),
  ToolboxMessageTree::Leaf(I18n::Message::ArgCommandWithArg, I18n::Message::Agument),
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
  ToolboxMessageTree::Leaf(I18n::Message::MatrixCommandWithArg, I18n::Message::NewMatrix, false, I18n::Message::MatrixCommand),
  ToolboxMessageTree::Leaf(I18n::Message::IndentityCommandWithArg, I18n::Message::Identity),
  ToolboxMessageTree::Leaf(I18n::Message::InverseCommandWithArg, I18n::Message::Inverse),
  ToolboxMessageTree::Leaf(I18n::Message::DeterminantCommandWithArg, I18n::Message::Determinant),
  ToolboxMessageTree::Leaf(I18n::Message::TransposeCommandWithArg, I18n::Message::Transpose),
  ToolboxMessageTree::Leaf(I18n::Message::TraceCommandWithArg, I18n::Message::Trace),
  ToolboxMessageTree::Leaf(I18n::Message::DimensionCommandWithArg, I18n::Message::Dimension),
  ToolboxMessageTree::Leaf(I18n::Message::RowEchelonFormCommandWithArg, I18n::Message::RowEchelonForm),
  ToolboxMessageTree::Leaf(I18n::Message::ReducedRowEchelonFormCommandWithArg, I18n::Message::ReducedRowEchelonForm)
};

const ToolboxMessageTree vectorsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::DotCommandWithArg, I18n::Message::Dot),
  ToolboxMessageTree::Leaf(I18n::Message::CrossCommandWithArg, I18n::Message::Cross),
  ToolboxMessageTree::Leaf(I18n::Message::NormVectorCommandWithArg, I18n::Message::NormVector),
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
const ToolboxMessageTree unitDistanceMeterNode = ToolboxMessageTree::Node(I18n::Message::UnitDistanceMeter, unitDistanceMeterChildren);
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
const ToolboxMessageTree unitMassGramNode = ToolboxMessageTree::Node(I18n::Message::UnitMassGram, unitMassGramChildren);
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
  ToolboxMessageTree::Node(I18n::Message::UnitEnergyEletronVoltMenu, unitEnergyElectronVoltChildren)};

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
const ToolboxMessageTree unitVolumeLiterNode = ToolboxMessageTree::Node(I18n::Message::UnitVolumeLiter, unitVolumeLiterChildren);
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

const ToolboxMessageTree menu[] = {
  ToolboxMessageTree::Leaf(I18n::Message::AbsCommandWithArg, I18n::Message::AbsoluteValue),
  ToolboxMessageTree::Leaf(I18n::Message::RootCommandWithArg, I18n::Message::NthRoot),
  ToolboxMessageTree::Leaf(I18n::Message::LogCommandWithArg, I18n::Message::BasedLogarithm),
  ToolboxMessageTree::Node(I18n::Message::Calculation, calculChildren),
  ToolboxMessageTree::Node(I18n::Message::ComplexNumber, complexChildren),
  ToolboxMessageTree::Node(I18n::Message::Combinatorics, combinatoricsChildren),
  ToolboxMessageTree::Node(I18n::Message::Probability, probabilityChildren),
  ToolboxMessageTree::Node(I18n::Message::Arithmetic, arithmeticChildren),
  ToolboxMessageTree::Node(I18n::Message::Matrices, matricesChildren),
  ToolboxMessageTree::Node(I18n::Message::Vectors, vectorsChildren),
#if LIST_ARE_DEFINED
  ToolboxMessageTree::Node(I18n::Message::Lists,listsChildren),
#endif
  ToolboxMessageTree::Node(I18n::Message::Unit, unitChildren),
  ToolboxMessageTree::Node(I18n::Message::RandomAndApproximation, randomAndApproximationChildren),
  ToolboxMessageTree::Node(I18n::Message::HyperbolicTrigonometry, trigonometryChildren),
  ToolboxMessageTree::Node(I18n::Message::Fluctuation, predictionChildren)};

const ToolboxMessageTree toolboxModel = ToolboxMessageTree::Node(I18n::Message::Toolbox, menu);

MathToolbox::MathToolbox() :
  Toolbox(nullptr, rootModel()->label())
{
}

bool MathToolbox::selectLeaf(int selectedRow) {
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

MessageTableCellWithMessage * MathToolbox::leafCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

MessageTableCellWithChevron* MathToolbox::nodeCellAtIndex(int index) {
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

