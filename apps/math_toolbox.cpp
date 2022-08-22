#include "math_toolbox.h"
#include "exam_mode_configuration.h"
#include "global_preferences.h"
#include "shared/global_context.h"
#include "./shared/toolbox_helpers.h"
#include <apps/apps_container.h>
#include <assert.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/layout_helper.h>
#include <string.h>

using namespace Poincare;
using namespace Escher;

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

constexpr ToolboxMessageTree unitDistanceMeterPico = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterPicoSymbol, I18n::Message::UnitDistanceMeterPico);
constexpr ToolboxMessageTree unitDistanceMeterNano = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterNanoSymbol, I18n::Message::UnitDistanceMeterNano);
constexpr ToolboxMessageTree unitDistanceMeterMicro = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterMicroSymbol, I18n::Message::UnitDistanceMeterMicro);
constexpr ToolboxMessageTree unitDistanceMeterMilli = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterMilliSymbol, I18n::Message::UnitDistanceMeterMilli);
constexpr ToolboxMessageTree unitDistanceMeterCenti = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterCentiSymbol, I18n::Message::UnitDistanceMeterCenti);
constexpr ToolboxMessageTree unitDistanceMeter = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterSymbol, I18n::Message::UnitDistanceMeter);
constexpr ToolboxMessageTree unitDistanceMeterKilo = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMeterKiloSymbol, I18n::Message::UnitDistanceMeterKilo);
constexpr ToolboxMessageTree unitDistanceAstronomicalUnit = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceAstronomicalUnitSymbol, I18n::Message::UnitDistanceAstronomicalUnit);
constexpr ToolboxMessageTree unitDistanceLightYear = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceLightYearSymbol, I18n::Message::UnitDistanceLightYear);
constexpr ToolboxMessageTree unitDistanceParsec = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceParsecSymbol, I18n::Message::UnitDistanceParsec);
constexpr ToolboxMessageTree unitDistanceInch = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceInchSymbol, I18n::Message::UnitDistanceInch);
constexpr ToolboxMessageTree unitDistanceFoot = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceFootSymbol, I18n::Message::UnitDistanceFoot);
constexpr ToolboxMessageTree unitDistanceYard = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceYardSymbol, I18n::Message::UnitDistanceYard);
constexpr ToolboxMessageTree unitDistanceMile = ToolboxMessageTree::Leaf(I18n::Message::UnitDistanceMileSymbol, I18n::Message::UnitDistanceMile);

constexpr const ToolboxMessageTree * const unitDistanceMeterChildren[] = {&unitDistanceMeterPico, &unitDistanceMeterNano, &unitDistanceMeterMicro, &unitDistanceMeterMilli, &unitDistanceMeterCenti, &unitDistanceMeter, &unitDistanceMeterKilo};
constexpr ToolboxMessageTree unitDistanceMeterNode = ToolboxMessageTree::Node(I18n::Message::UnitMetricMenu, unitDistanceMeterChildren);
constexpr const ToolboxMessageTree * const unitDistanceChildrenForImperialToolbox[] = {
  &unitDistanceInch,
  &unitDistanceFoot,
  &unitDistanceYard,
  &unitDistanceMile,
  &unitDistanceAstronomicalUnit,
  &unitDistanceLightYear,
  &unitDistanceParsec,
  &unitDistanceMeterNode
};
constexpr const ToolboxMessageTree * const unitDistanceImperialChildren[] = {&unitDistanceInch, &unitDistanceFoot, &unitDistanceYard, &unitDistanceMile};
constexpr ToolboxMessageTree unitDistanceImperialNode = ToolboxMessageTree::Node(I18n::Message::UnitImperialMenu, unitDistanceImperialChildren);
constexpr const ToolboxMessageTree * const unitDistanceChildrenForMetricToolbox[] = {
  &unitDistanceMeterPico,
  &unitDistanceMeterNano,
  &unitDistanceMeterMicro,
  &unitDistanceMeterMilli,
  &unitDistanceMeterCenti,
  &unitDistanceMeter,
  &unitDistanceMeterKilo,
  &unitDistanceAstronomicalUnit,
  &unitDistanceLightYear,
  &unitDistanceParsec,
  &unitDistanceImperialNode
};

constexpr ToolboxMessageTree unitDistanceFork[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitDistanceMenu, unitDistanceChildrenForMetricToolbox),
  ToolboxMessageTree::Node(I18n::Message::UnitDistanceMenu, unitDistanceChildrenForImperialToolbox)
};

constexpr ToolboxMessageTree unitTimeChildren[] = {
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

constexpr ToolboxMessageTree unitFrequencyHertzChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitFrequencyHertzSymbol, I18n::Message::UnitFrequencyHertz),
  ToolboxMessageTree::Leaf(I18n::Message::UnitFrequencyHertzKiloSymbol, I18n::Message::UnitFrequencyHertzKilo),
  ToolboxMessageTree::Leaf(I18n::Message::UnitFrequencyHertzMegaSymbol, I18n::Message::UnitFrequencyHertzMega),
  ToolboxMessageTree::Leaf(I18n::Message::UnitFrequencyHertzGigaSymbol, I18n::Message::UnitFrequencyHertzGiga)
};

constexpr ToolboxMessageTree timeAndFrequencyChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitTimeMenu, unitTimeChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitFrequencyMenu, unitFrequencyHertzChildren),
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

constexpr const ToolboxMessageTree * const unitVolumeLiterChildren[] = {&unitVolumeLiterMilli, &unitVolumeLiterCenti, &unitVolumeLiterDeci, &unitVolumeLiter};
constexpr ToolboxMessageTree unitVolumeLiterNode = ToolboxMessageTree::Node(I18n::Message::UnitMetricMenu, unitVolumeLiterChildren);
constexpr const ToolboxMessageTree * const unitVolumeChildrenForImperialToolbox[] = {
  &unitVolumeTeaspoon,
  &unitVolumeTablespoon,
  &unitVolumeFluidOunce,
  &unitVolumeCup,
  &unitVolumePint,
  &unitVolumeQuart,
  &unitVolumeGallon,
  &unitVolumeLiterNode
};
constexpr const ToolboxMessageTree * const unitVolumeImperialChildren[] = {&unitVolumeTeaspoon, &unitVolumeTablespoon, &unitVolumeFluidOunce, &unitVolumeCup, &unitVolumePint, &unitVolumeQuart, &unitVolumeGallon};
constexpr ToolboxMessageTree unitVolumeImperialNode = ToolboxMessageTree::Node(I18n::Message::UnitImperialMenu, unitVolumeImperialChildren);
constexpr const ToolboxMessageTree * const unitVolumeChildrenForMetricToolbox[] = {
  &unitVolumeLiterMilli,
  &unitVolumeLiterCenti,
  &unitVolumeLiterDeci,
  &unitVolumeLiter,
  &unitVolumeImperialNode
};

constexpr ToolboxMessageTree unitVolumeFork[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitVolumeMenu, unitVolumeChildrenForMetricToolbox),
  ToolboxMessageTree::Node(I18n::Message::UnitVolumeMenu, unitVolumeChildrenForImperialToolbox),
};

constexpr ToolboxMessageTree unitSurfaceHectar = ToolboxMessageTree::Leaf(I18n::Message::UnitSurfaceHectarSymbol, I18n::Message::UnitSurfaceHectar);
constexpr ToolboxMessageTree unitSurfaceAcre = ToolboxMessageTree::Leaf(I18n::Message::UnitSurfaceAcreSymbol, I18n::Message::UnitSurfaceAcre);

constexpr const ToolboxMessageTree * const unitSurfaceMetricChildren[] = {&unitSurfaceHectar};
constexpr ToolboxMessageTree unitSurfaceMetricNode = ToolboxMessageTree::Node(I18n::Message::UnitMetricMenu, unitSurfaceMetricChildren);
constexpr const ToolboxMessageTree * const unitSurfaceChildrenForImperialToolbox[] = {&unitSurfaceAcre, &unitSurfaceMetricNode};

constexpr const ToolboxMessageTree * const unitSurfaceImperialChildren[] = {&unitSurfaceAcre};
constexpr ToolboxMessageTree unitSurfaceImperialNode = ToolboxMessageTree::Node(I18n::Message::UnitImperialMenu, unitSurfaceImperialChildren);
constexpr const ToolboxMessageTree * const unitSurfaceChildrenForMetricToolbox[] = {&unitSurfaceHectar, &unitSurfaceImperialNode};

constexpr ToolboxMessageTree unitSurfaceFork[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitSurfaceMenu, unitSurfaceChildrenForMetricToolbox),
  ToolboxMessageTree::Node(I18n::Message::UnitSurfaceMenu, unitSurfaceChildrenForImperialToolbox)
};

constexpr ToolboxMessageTree volumeAndSurfaceChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitVolumeMenu, unitVolumeFork, true),
  ToolboxMessageTree::Node(I18n::Message::UnitSurfaceMenu, unitSurfaceFork, true),
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

constexpr const ToolboxMessageTree * const unitMassGramChildren[] = {&unitMassGramMicro, &unitMassGramMilli, &unitMassGram, &unitMassGramKilo, &unitMassTonne};
constexpr ToolboxMessageTree unitMassGramNode = ToolboxMessageTree::Node(I18n::Message::UnitMetricMenu, unitMassGramChildren);
constexpr const ToolboxMessageTree * const unitMassChildrenForImperialToolbox[] = {
  &unitMassOunce,
  &unitMassPound,
  &unitMassShortTon,
  &unitMassLongTon,
  &unitMassGramNode
};
constexpr const ToolboxMessageTree * const unitMassImperialChildren[] = {&unitMassOunce, &unitMassPound, &unitMassShortTon, &unitMassLongTon};
constexpr ToolboxMessageTree unitMassImperialNode = ToolboxMessageTree::Node(I18n::Message::UnitImperialMenu, unitMassImperialChildren);
constexpr const ToolboxMessageTree * const unitMassChildrenForMetricToolbox[] = {
  &unitMassGramMicro,
  &unitMassGramMilli,
  &unitMassGram,
  &unitMassGramKilo,
  &unitMassTonne,
  &unitMassImperialNode
};

constexpr ToolboxMessageTree unitMassFork[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitMassMenu, unitMassChildrenForMetricToolbox),
  ToolboxMessageTree::Node(I18n::Message::UnitMassMenu, unitMassChildrenForImperialToolbox),
};

constexpr ToolboxMessageTree unitCurrentAmpereChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitCurrentAmpereMicroSymbol, I18n::Message::UnitCurrentAmpereMicro),
  ToolboxMessageTree::Leaf(I18n::Message::UnitCurrentAmpereMilliSymbol, I18n::Message::UnitCurrentAmpereMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitCurrentAmpereSymbol, I18n::Message::UnitCurrentAmpere),
};

constexpr ToolboxMessageTree unitPotentialVoltChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitPotentialVoltMicroSymbol, I18n::Message::UnitPotentialVoltMicro),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPotentialVoltMilliSymbol, I18n::Message::UnitPotentialVoltMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPotentialVoltSymbol, I18n::Message::UnitPotentialVolt),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPotentialVoltKiloSymbol, I18n::Message::UnitPotentialVoltKilo),
};

constexpr ToolboxMessageTree unitResistanceOhmChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitResistanceOhmSymbol, I18n::Message::UnitResistanceOhm),
  ToolboxMessageTree::Leaf(I18n::Message::UnitResistanceOhmKiloSymbol, I18n::Message::UnitResistanceOhmKilo),
};

constexpr ToolboxMessageTree unitCapacitanceFaradChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitCapacitanceFaradMicroSymbol, I18n::Message::UnitCapacitanceFaradMicro),
  ToolboxMessageTree::Leaf(I18n::Message::UnitCapacitanceFaradMilliSymbol, I18n::Message::UnitCapacitanceFaradMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitCapacitanceFaradSymbol, I18n::Message::UnitCapacitanceFarad),
};

constexpr ToolboxMessageTree electricitOtherChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitInductanceHenrySymbol, I18n::Message::UnitInductanceHenry),
  ToolboxMessageTree::Leaf(I18n::Message::UnitChargeCoulombSymbol, I18n::Message::UnitChargeCoulomb),
  // ToolboxMessageTree::Leaf(I18n::Message::UnitConductanceSiemensMilliSymbol, I18n::Message::UnitConductanceSiemensMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitConductanceSiemensSymbol, I18n::Message::UnitConductanceSiemens),
  ToolboxMessageTree::Leaf(I18n::Message::UnitMagneticFieldTeslaSymbol, I18n::Message::UnitMagneticFieldTesla),
};

constexpr ToolboxMessageTree electricityChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitCurrentMenu, unitCurrentAmpereChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitPotentialMenu, unitPotentialVoltChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitResistanceMenu, unitResistanceOhmChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitCapacitanceMenu, unitCapacitanceFaradChildren),
  ToolboxMessageTree::Node(I18n::Message::OtherUnitsMenu, electricitOtherChildren),
};

constexpr ToolboxMessageTree unitForceNewtonChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitForceNewtonMilliSymbol, I18n::Message::UnitForceNewtonMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitForceNewtonSymbol, I18n::Message::UnitForceNewton),
  ToolboxMessageTree::Leaf(I18n::Message::UnitForceNewtonKiloSymbol, I18n::Message::UnitForceNewtonKilo),
};

constexpr ToolboxMessageTree unitPressureChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitPressurePascalSymbol, I18n::Message::UnitPressurePascal),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPressurePascalHectoSymbol, I18n::Message::UnitPressurePascalHecto),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPressureBarSymbol, I18n::Message::UnitPressureBar),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPressureAtmSymbol, I18n::Message::UnitPressureAtm)
};

constexpr ToolboxMessageTree forceAndPressureChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitForceMenu, unitForceNewtonChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitPressureMenu, unitPressureChildren),
};

constexpr ToolboxMessageTree unitEnergyJouleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyJouleMilliSymbol, I18n::Message::UnitEnergyJouleMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyJouleSymbol, I18n::Message::UnitEnergyJoule),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyJouleKiloSymbol, I18n::Message::UnitEnergyJouleKilo),
};

constexpr ToolboxMessageTree unitPowerWattChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattMicroSymbol, I18n::Message::UnitPowerWattMicro),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattMilliSymbol, I18n::Message::UnitPowerWattMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattSymbol, I18n::Message::UnitPowerWatt),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattKiloSymbol, I18n::Message::UnitPowerWattKilo),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattMegaSymbol, I18n::Message::UnitPowerWattMega),
  ToolboxMessageTree::Leaf(I18n::Message::UnitPowerWattGigaSymbol, I18n::Message::UnitPowerWattGiga),
};

constexpr ToolboxMessageTree unitEnergyElectronVoltChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyElectronVoltMilliSymbol, I18n::Message::UnitEnergyElectronVoltMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyElectronVoltSymbol, I18n::Message::UnitEnergyElectronVolt),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyElectronVoltKiloSymbol, I18n::Message::UnitEnergyElectronVoltKilo),
  ToolboxMessageTree::Leaf(I18n::Message::UnitEnergyElectronVoltMegaSymbol, I18n::Message::UnitEnergyElectronVoltMega),
};

constexpr ToolboxMessageTree energyAndPowerChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitEnergyJouleMenu, unitEnergyJouleChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitPowerMenu, unitPowerWattChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitEnergyEletronVoltMenu, unitEnergyElectronVoltChildren),
};

constexpr ToolboxMessageTree unitTemperatureKelvin = ToolboxMessageTree::Leaf(I18n::Message::UnitTemperatureKelvinSymbol, I18n::Message::UnitTemperatureKelvin);
constexpr ToolboxMessageTree unitTemperatureCelsius = ToolboxMessageTree::Leaf(I18n::Message::UnitTemperatureCelsiusSymbol, I18n::Message::UnitTemperatureCelsius);
constexpr ToolboxMessageTree unitTemperatureFahrenheit = ToolboxMessageTree::Leaf(I18n::Message::UnitTemperatureFahrenheitSymbol, I18n::Message::UnitTemperatureFahrenheit);

constexpr const ToolboxMessageTree * const unitTemperatureChildrenForMetricToolbox[] = { &unitTemperatureKelvin, &unitTemperatureCelsius, &unitTemperatureFahrenheit };
constexpr const ToolboxMessageTree * const unitTemperatureChildrenForImperialToolbox[] = { &unitTemperatureKelvin, &unitTemperatureFahrenheit, &unitTemperatureCelsius };

constexpr ToolboxMessageTree unitTemperatureFork[] = {
  ToolboxMessageTree::Node(I18n::Message::UnitTemperatureMenu, unitTemperatureChildrenForMetricToolbox),
  ToolboxMessageTree::Node(I18n::Message::UnitTemperatureMenu, unitTemperatureChildrenForImperialToolbox),
};

constexpr ToolboxMessageTree unitAmountMoleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitAmountMoleMicroSymbol, I18n::Message::UnitAmountMoleMicro),
  ToolboxMessageTree::Leaf(I18n::Message::UnitAmountMoleMilliSymbol, I18n::Message::UnitAmountMoleMilli),
  ToolboxMessageTree::Leaf(I18n::Message::UnitAmountMoleSymbol, I18n::Message::UnitAmountMole),
};

constexpr ToolboxMessageTree OtherChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitLuminousIntensityCandelaSymbol, I18n::Message::UnitLuminousIntensityCandela),
  ToolboxMessageTree::Node(I18n::Message::UnitAmountMenu, unitAmountMoleChildren),
};

constexpr ToolboxMessageTree ConstantsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::SpeedOfLightSymbol, I18n::Message::SpeedOfLight),
  ToolboxMessageTree::Leaf(I18n::Message::ElementaryChargeSymbol, I18n::Message::ElementaryCharge),
  ToolboxMessageTree::Leaf(I18n::Message::GravitationalConstantSymbol, I18n::Message::GravitationalConstant),
  ToolboxMessageTree::Leaf(I18n::Message::AccelerationOfGravitySymbol, I18n::Message::AccelerationOfGravity),
  ToolboxMessageTree::Leaf(I18n::Message::BoltzmannConstantSymbol, I18n::Message::BoltzmannConstant),
  ToolboxMessageTree::Leaf(I18n::Message::CoulombConstantSymbol, I18n::Message::CoulombConstant),
  ToolboxMessageTree::Leaf(I18n::Message::ElectronMassSymbol, I18n::Message::ElectronMass),
  ToolboxMessageTree::Leaf(I18n::Message::NeutronMassSymbol, I18n::Message::NeutronMass),
  ToolboxMessageTree::Leaf(I18n::Message::ProtonMassSymbol, I18n::Message::ProtonMass),
  ToolboxMessageTree::Leaf(I18n::Message::AvogadroConstantSymbol, I18n::Message::AvogadroConstant),
  ToolboxMessageTree::Leaf(I18n::Message::MolarGasConstantSymbol, I18n::Message::MolarGasConstant),
  ToolboxMessageTree::Leaf(I18n::Message::VacuumPermittivitySymbol, I18n::Message::VacuumPermittivity),
  ToolboxMessageTree::Leaf(I18n::Message::VacuumPermeabilitySymbol, I18n::Message::VacuumPermeability)
};

constexpr ToolboxMessageTree unitChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::UnitConversionCommandWithArg, I18n::Message::UnitConversion, false, I18n::Message::UnitConversionCommand),
  ToolboxMessageTree::Node(I18n::Message::UnitDistanceMenu, unitDistanceFork, true),
  ToolboxMessageTree::Node(I18n::Message::UnitTimeAndFrequencyMenu, timeAndFrequencyChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitVolumeAndAreaMenu, volumeAndSurfaceChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitMassMenu, unitMassFork, true),
  ToolboxMessageTree::Node(I18n::Message::UnitElectricityMenu, electricityChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitForceAndPressureMenu, forceAndPressureChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitEnergyAndPowerMenu, energyAndPowerChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitTemperatureMenu, unitTemperatureFork, true),
  ToolboxMessageTree::Node(I18n::Message::OtherUnitsMenu, OtherChildren),
  ToolboxMessageTree::Node(I18n::Message::Constants, ConstantsChildren),
};

constexpr ToolboxMessageTree calculChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::DiffCommandWithArg, I18n::Message::DerivateNumber, false, I18n::Message::DiffCommand),
  ToolboxMessageTree::Leaf(I18n::Message::HigherOrderDiffCommandWithArg, I18n::Message::HigherOrderDerivateNumber, false, I18n::Message::HigherOrderDiffCommand),
  ToolboxMessageTree::Leaf(I18n::Message::IntCommandWithArg, I18n::Message::Integral, false, I18n::Message::IntCommand),
  ToolboxMessageTree::Leaf(I18n::Message::SumCommandWithArg, I18n::Message::Sum, false, I18n::Message::SumCommand),
  ToolboxMessageTree::Leaf(I18n::Message::ProductCommandWithArg, I18n::Message::Product, false, I18n::Message::ProductCommand)
};

constexpr ToolboxMessageTree complexChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::AbsCommandWithArg,I18n::Message::ComplexAbsoluteValue),
  ToolboxMessageTree::Leaf(I18n::Message::ArgCommandWithArg, I18n::Message::Argument),
  ToolboxMessageTree::Leaf(I18n::Message::ReCommandWithArg, I18n::Message::RealPart),
  ToolboxMessageTree::Leaf(I18n::Message::ImCommandWithArg, I18n::Message::ImaginaryPart),
  ToolboxMessageTree::Leaf(I18n::Message::ConjCommandWithArg, I18n::Message::Conjugate)
};

constexpr ToolboxMessageTree combinatoricsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::BinomialCommandWithArg, I18n::Message::Combination),
  ToolboxMessageTree::Leaf(I18n::Message::PermuteCommandWithArg, I18n::Message::Permutation),
  ToolboxMessageTree::Leaf(I18n::Message::FactorialCommandWithArg, I18n::Message::Factorial, false, I18n::Message::FactorialCommand),
};

constexpr ToolboxMessageTree normalDistributionChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::NormCDFCommandWithArg, I18n::Message::NormCDF),
  ToolboxMessageTree::Leaf(I18n::Message::NormCDFRangeCommandWithArg, I18n::Message::NormCDFRange),
  ToolboxMessageTree::Leaf(I18n::Message::InvNormCommandWithArg, I18n::Message::InvNorm),
  ToolboxMessageTree::Leaf(I18n::Message::NormPDFCommandWithArg, I18n::Message::NormPDF)
};

constexpr ToolboxMessageTree studentDistributionChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::StudentCDFCommandWithArg, I18n::Message::StudentCDF),
  ToolboxMessageTree::Leaf(I18n::Message::StudentCDFRangeCommandWithArg, I18n::Message::StudentCDFRange),
  ToolboxMessageTree::Leaf(I18n::Message::InvStudentCommandWithArg, I18n::Message::InvStudent),
  ToolboxMessageTree::Leaf(I18n::Message::StudentPDFCommandWithArg, I18n::Message::StudentPDF)
};

constexpr ToolboxMessageTree binomialDistributionChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::BinomialPDFCommandWithArg, I18n::Message::BinomialPDF),
  ToolboxMessageTree::Leaf(I18n::Message::BinomialCDFCommandWithArg, I18n::Message::BinomialCDF),
  ToolboxMessageTree::Leaf(I18n::Message::InvBinomialCommandWithArg, I18n::Message::InvBinomial),
};

constexpr ToolboxMessageTree poissonDistributionChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PoissonPDFCommandWithArg, I18n::Message::PoissonPDF),
  ToolboxMessageTree::Leaf(I18n::Message::PoissonCDFCommandWithArg, I18n::Message::PoissonCDF),
};

constexpr ToolboxMessageTree geometricDistributionChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::GeomPDFCommandWithArg, I18n::Message::GeomPDF),
  ToolboxMessageTree::Leaf(I18n::Message::GeomCDFCommandWithArg, I18n::Message::GeomCDF),
  ToolboxMessageTree::Leaf(I18n::Message::GeomCDFRangeCommandWithArg, I18n::Message::GeomCDFRange),
  ToolboxMessageTree::Leaf(I18n::Message::InvGeomCommandWithArg, I18n::Message::InvGeom)
};

constexpr ToolboxMessageTree probabilityLawsChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::Normal, normalDistributionChildren),
  ToolboxMessageTree::Node(I18n::Message::Student, studentDistributionChildren),
  ToolboxMessageTree::Node(I18n::Message::Binomial, binomialDistributionChildren),
  ToolboxMessageTree::Node(I18n::Message::Poisson, poissonDistributionChildren),
  ToolboxMessageTree::Node(I18n::Message::Geometric, geometricDistributionChildren),
};

constexpr ToolboxMessageTree randomChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::RandomCommandWithArg, I18n::Message::RandomFloat),
  ToolboxMessageTree::Leaf(I18n::Message::RandintCommandWithArg, I18n::Message::RandomInteger),
  ToolboxMessageTree::Leaf(I18n::Message::RandintCommandWithOnlyOneArg, I18n::Message::RandomIntegerWithOnlyOneArg),
};

constexpr ToolboxMessageTree probabilityChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::Combinatorics, combinatoricsChildren),
  ToolboxMessageTree::Node(I18n::Message::Distributions, probabilityLawsChildren),
  ToolboxMessageTree::Node(I18n::Message::Random, randomChildren),
};

constexpr ToolboxMessageTree matricesChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::DeterminantCommandWithArg, I18n::Message::Determinant),
  ToolboxMessageTree::Leaf(I18n::Message::InverseCommandWithArg, I18n::Message::Inverse),
  ToolboxMessageTree::Leaf(I18n::Message::IndentityCommandWithArg, I18n::Message::Identity),
  ToolboxMessageTree::Leaf(I18n::Message::TraceCommandWithArg, I18n::Message::Trace),
  ToolboxMessageTree::Leaf(I18n::Message::RowEchelonFormCommandWithArg, I18n::Message::RowEchelonForm),
  ToolboxMessageTree::Leaf(I18n::Message::ReducedRowEchelonFormCommandWithArg, I18n::Message::ReducedRowEchelonForm),
};

constexpr ToolboxMessageTree vectorsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::DotCommandWithArg, I18n::Message::Dot),
  ToolboxMessageTree::Leaf(I18n::Message::CrossCommandWithArg, I18n::Message::Cross),
  ToolboxMessageTree::Leaf(I18n::Message::NormVectorCommandWithArg, I18n::Message::NormVector),
};

constexpr ToolboxMessageTree matricesVectorsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::MatrixCommandWithArg, I18n::Message::NewMatrix, false, I18n::Message::MatrixCommand),
  ToolboxMessageTree::Leaf(I18n::Message::TransposeCommandWithArg, I18n::Message::Transpose),
  ToolboxMessageTree::Leaf(I18n::Message::DimensionCommandWithArg, I18n::Message::Dimension),
  ToolboxMessageTree::Node(I18n::Message::Matrices, matricesChildren),
  ToolboxMessageTree::Node(I18n::Message::Vectors, vectorsChildren),
};

constexpr ToolboxMessageTree arithmeticChildrenWithMixedFractions[] = {
  ToolboxMessageTree::Leaf(I18n::Message::GcdCommandWithArg, I18n::Message::GreatCommonDivisor),
  ToolboxMessageTree::Leaf(I18n::Message::LcmCommandWithArg, I18n::Message::LeastCommonMultiple),
  ToolboxMessageTree::Leaf(I18n::Message::FactorCommandWithArg, I18n::Message::PrimeFactorDecomposition),
  ToolboxMessageTree::Leaf(I18n::Message::MixedFractionCommandWithArg, I18n::Message::MixedFraction, false, I18n::Message::MixedFractionCommand),
  ToolboxMessageTree::Leaf(I18n::Message::RemCommandWithArg, I18n::Message::Remainder),
  ToolboxMessageTree::Leaf(I18n::Message::QuoCommandWithArg, I18n::Message::Quotient)
};

constexpr ToolboxMessageTree arithmeticChildrenWithoutMixedFractions[] = {
  ToolboxMessageTree::Leaf(I18n::Message::GcdCommandWithArg, I18n::Message::GreatCommonDivisor),
  ToolboxMessageTree::Leaf(I18n::Message::LcmCommandWithArg, I18n::Message::LeastCommonMultiple),
  ToolboxMessageTree::Leaf(I18n::Message::FactorCommandWithArg, I18n::Message::PrimeFactorDecomposition),
  ToolboxMessageTree::Leaf(I18n::Message::RemCommandWithArg, I18n::Message::Remainder),
  ToolboxMessageTree::Leaf(I18n::Message::QuoCommandWithArg, I18n::Message::Quotient)
};

constexpr ToolboxMessageTree arithmeticFork[] = {
  ToolboxMessageTree::Node(I18n::Message::Arithmetic, arithmeticChildrenWithMixedFractions),
  ToolboxMessageTree::Node(I18n::Message::Arithmetic, arithmeticChildrenWithoutMixedFractions)
};

constexpr ToolboxMessageTree hyperbolicTrigonometryChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::SinhCommandWithArg, I18n::Message::HyperbolicSine),
  ToolboxMessageTree::Leaf(I18n::Message::CoshCommandWithArg, I18n::Message::HyperbolicCosine),
  ToolboxMessageTree::Leaf(I18n::Message::TanhCommandWithArg, I18n::Message::HyperbolicTangent),
  ToolboxMessageTree::Leaf(I18n::Message::AsinhCommandWithArg, I18n::Message::InverseHyperbolicSine),
  ToolboxMessageTree::Leaf(I18n::Message::AcoshCommandWithArg, I18n::Message::InverseHyperbolicCosine),
  ToolboxMessageTree::Leaf(I18n::Message::AtanhCommandWithArg, I18n::Message::InverseHyperbolicTangent)
};

constexpr ToolboxMessageTree advancedTrigonometryChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::CscCommandWithArg, I18n::Message::Cosecant),
  ToolboxMessageTree::Leaf(I18n::Message::SecCommandWithArg, I18n::Message::Secant),
  ToolboxMessageTree::Leaf(I18n::Message::CotCommandWithArg, I18n::Message::Cotangent),
  ToolboxMessageTree::Leaf(I18n::Message::AcscCommandWithArg, I18n::Message::ArcCosecant),
  ToolboxMessageTree::Leaf(I18n::Message::AsecCommandWithArg, I18n::Message::ArcSecant),
  ToolboxMessageTree::Leaf(I18n::Message::AcotCommandWithArg, I18n::Message::ArcCotangent)
};

constexpr ToolboxMessageTree trigonometryChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::HyperbolicTrigonometry, hyperbolicTrigonometryChildren),
  ToolboxMessageTree::Node(I18n::Message::AdvancedTrigonometry, advancedTrigonometryChildren),
};

constexpr ToolboxMessageTree decimalNumbersChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::FloorCommandWithArg, I18n::Message::Floor),
  ToolboxMessageTree::Leaf(I18n::Message::FracCommandWithArg, I18n::Message::FracPart),
  ToolboxMessageTree::Leaf(I18n::Message::CeilCommandWithArg, I18n::Message::Ceiling),
  ToolboxMessageTree::Leaf(I18n::Message::RoundCommandWithArg, I18n::Message::Rounding),
};

constexpr ToolboxMessageTree listsStatsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::ListMeanCommandWithArg, I18n::Message::Mean),
  ToolboxMessageTree::Leaf(I18n::Message::ListStandardDevCommandWithArg, I18n::Message::StandardDeviation),
  ToolboxMessageTree::Leaf(I18n::Message::ListMedianCommandWithArg, I18n::Message::Median),
  ToolboxMessageTree::Leaf(I18n::Message::ListVarianceCommandWithArg, I18n::Message::Deviation),
  ToolboxMessageTree::Leaf(I18n::Message::ListSampleStandardDevCommandWithArg, I18n::Message::SampleSTD)
};

constexpr int alternateListsStatsOrder[] {
  0, // Mean
  1, // StandardDev
  4, // SampleStandardDev
  2, // Median
  3  // Variance
};

static_assert(sizeof(alternateListsStatsOrder) / sizeof(int) == sizeof(listsStatsChildren) / sizeof(ToolboxMessageTree), "Alternate lists stats order in toolbox has wrong size");

constexpr ToolboxMessageTree listsOperationsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::ListLengthCommandWithArg, I18n::Message::ListLength),
  ToolboxMessageTree::Leaf(I18n::Message::ListMinCommandWithArg, I18n::Message::Minimum),
  ToolboxMessageTree::Leaf(I18n::Message::ListMaxCommandWithArg, I18n::Message::Maximum),
  ToolboxMessageTree::Leaf(I18n::Message::ListSortCommandWithArg, I18n::Message::AscendingSorting),
  ToolboxMessageTree::Leaf(I18n::Message::ListSumCommandWithArg, I18n::Message::SumOfElements),
  ToolboxMessageTree::Leaf(I18n::Message::ListProductCommandWithArg, I18n::Message::ProductOfElements)
};

constexpr ToolboxMessageTree listsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::ListCommandWithArg, I18n::Message::NewList, false, I18n::Message::ListCommand),
  ToolboxMessageTree::Leaf(I18n::Message::ListSequenceCommandWithArg, I18n::Message::ListSequenceDescription, false, I18n::Message::ListSequenceCommand),
  ToolboxMessageTree::Node(I18n::Message::StatsApp, listsStatsChildren),
  ToolboxMessageTree::Node(I18n::Message::Operations, listsOperationsChildren)
};

constexpr ToolboxMessageTree logicChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::LessOrEqual, I18n::Message::LessOrEqualText),
  ToolboxMessageTree::Leaf(I18n::Message::GreaterOrEqual, I18n::Message::GreaterOrEqualText),
  ToolboxMessageTree::Leaf(I18n::Message::NotEqual, I18n::Message::NotEqualText),
  ToolboxMessageTree::Leaf(I18n::Message::AndCommand, I18n::Message::AndMessage, false, I18n::Message::AndCommandWithSpaces),
  ToolboxMessageTree::Leaf(I18n::Message::OrCommand, I18n::Message::OrMessage, false, I18n::Message::OrCommandWithSpaces),
  ToolboxMessageTree::Leaf(I18n::Message::NotCommand, I18n::Message::NotMessage, false, I18n::Message::NotCommandWithSpaces),
  ToolboxMessageTree::Leaf(I18n::Message::XorCommand, I18n::Message::XorMessage, false, I18n::Message::XorCommandWithSpaces),
  ToolboxMessageTree::Leaf(I18n::Message::NorCommand, I18n::Message::NorMessage, false, I18n::Message::NorCommandWithSpaces),
  ToolboxMessageTree::Leaf(I18n::Message::NandCommand, I18n::Message::NandMessage, false, I18n::Message::NandCommandWithSpaces)
};

constexpr ToolboxMessageTree menu[] = {
  ToolboxMessageTree::Leaf(I18n::Message::AbsCommandWithArg, I18n::Message::AbsoluteValue),
  ToolboxMessageTree::Leaf(I18n::Message::RootCommandWithArg, I18n::Message::NthRoot),
  ToolboxMessageTree::Leaf(I18n::Message::LogCommandWithArg, I18n::Message::BasedLogarithm),
  ToolboxMessageTree::Node(I18n::Message::Calculus, calculChildren),
  ToolboxMessageTree::Node(I18n::Message::ComplexNumber, complexChildren),
  ToolboxMessageTree::Node(I18n::Message::Probability, probabilityChildren),
  ToolboxMessageTree::Node(I18n::Message::UnitAndConstant, unitChildren),
  ToolboxMessageTree::Node(I18n::Message::MatricesAndVectors, matricesVectorsChildren),
  ToolboxMessageTree::Node(I18n::Message::Lists,listsChildren),
  ToolboxMessageTree::Node(I18n::Message::Arithmetic, arithmeticFork, true),
  ToolboxMessageTree::Node(I18n::Message::Trigonometry, trigonometryChildren),
  ToolboxMessageTree::Node(I18n::Message::DecimalNumbers, decimalNumbersChildren),
  ToolboxMessageTree::Node(I18n::Message::Logic, logicChildren)
};

constexpr ToolboxMessageTree toolboxModel = ToolboxMessageTree::Node(I18n::Message::Toolbox, menu);

MathToolbox::MathToolbox() :
  Toolbox(nullptr, rootModel()->label())
{
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].setParentResponder(&m_selectableTableView);
    m_leafCells->setFont(KDFont::Size::Large);
  }
}

void MathToolbox::viewDidDisappear() {
  Toolbox::viewDidDisappear();

  /* NestedMenuController::viewDidDisappear might need cell heights, which would
   * use the MathToolbox cell heights memoization. We thus reset the MathToolbox
   * layouts only after calling the parent's viewDidDisappear. */

  // Tidy the layouts displayed in the MathToolbox to clean TreePool
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].setLayout(Layout());
  }
}

KDCoordinate MathToolbox::nonMemoizedRowHeight(int index) {
  if (typeAtIndex(index) == k_leafCellType) {
    ExpressionTableCellWithMessage tempCell;
    return heightForCellAtIndexWithWidthInit(&tempCell, index);
  }
  return Escher::Toolbox::nonMemoizedRowHeight(index);
}

bool MathToolbox::isMessageTreeDisabled(const ToolboxMessageTree * messageTree) const {
  I18n::Message label = messageTree->label();
  return (label == I18n::Message::SumCommandWithArg
       && ExamModeConfiguration::sumIsForbidden())
      || (label == I18n::Message::LogCommandWithArg
       && ExamModeConfiguration::basedLogarithmIsForbidden())
      || (label == I18n::Message::NormVectorCommandWithArg
       && ExamModeConfiguration::vectorNormIsForbidden())
      || ((label == I18n::Message::DotCommandWithArg || label == I18n::Message::CrossCommandWithArg)
       && ExamModeConfiguration::vectorProductsAreForbidden())
      || (label == I18n::Message::UnitAndConstant
       && ExamModeConfiguration::unitsAreForbidden());
}

bool MathToolbox::displayMessageTreeDisabledPopUp(const Escher::ToolboxMessageTree * messageTree) {
  if (isMessageTreeDisabled(messageTree)) {
    // TODO : It would be better if warning did not dismiss the toolbox
    Container::activeApp()->displayWarning(I18n::Message::DisabledFeatureInTestMode1, I18n::Message::DisabledFeatureInTestMode2);
    return true;
  }
  return false;
}

void MathToolbox::willDisplayCellForIndex(HighlightCell * cell, int index) {
  /* Unhighlight reusableCell to prevent display of multiple selected rows.
   * See SelectableTableView::reloadData comment. */
  cell->setHighlighted(false);
  const ToolboxMessageTree * messageTree = messageTreeModelAtIndex(index);
  KDColor textColor = isMessageTreeDisabled(messageTree) ? Palette::GrayDark : KDColorBlack;
  if (messageTree->numberOfChildren() == 0) {
    // Message is leaf
    if (GlobalPreferences::sharedGlobalPreferences()->listsStatsOrderInToolbox() == CountryPreferences::ListsStatsOrderInToolbox::Alternate && m_messageTreeModel->childrenList() == listsStatsChildren) {
      // We are in lists stats sub-menu
      messageTree = messageTreeModelAtIndex(alternateListsStatsOrder[index]);
    }
    ExpressionTableCellWithMessage * myCell = static_cast<ExpressionTableCellWithMessage *>(cell);
    const char * text = I18n::translate(messageTree->label());
    Layout resultLayout;

    if (Poincare::Preferences::sharedPreferences()->editionMode() == Poincare::Preferences::EditionMode::Edition2D) {
      Poincare::ExceptionCheckpoint ecp;
      // Try a 2D layout. If it fails, fall back on a lighter 1D edition layout
      if (ExceptionRun(ecp)) {
        // No context is given so that f(x) is never parsed as f×(x)
        Expression resultExpression = Expression::Parse(text, nullptr);
        if (!resultExpression.isUninitialized()) {
          // The text is parsable, we create its layout an insert it.
          resultLayout = resultExpression.createLayout(Poincare::Preferences::sharedPreferences()->displayMode(), Poincare::PrintFloat::k_numberOfStoredSignificantDigits, Container::activeApp()->localContext());
        }
      }
    }
    if (resultLayout.isUninitialized()) {
      // With Edition1D, pool exception or invalid syntax, use a simpler layout.
      resultLayout = LayoutHelper::String(text, strlen(text));
    }

    myCell->setLayout(resultLayout);
    myCell->setSubLabelMessage(messageTree->text());
    myCell->setTextColor(textColor);
  } else {
    // Message is a submenu
    MessageTableCell * typedCell = static_cast<MessageTableCell *>(cell);
    typedCell->setTextColor(textColor);
    Escher::Toolbox::willDisplayCellForIndex(cell, index);
  }
}

bool MathToolbox::selectSubMenu(int selectedRow) {
  return displayMessageTreeDisabledPopUp(messageTreeModelAtIndex(selectedRow))
      || Toolbox::selectSubMenu(selectedRow);
}

bool MathToolbox::selectLeaf(int selectedRow) {
  assert(typeAtIndex(selectedRow) == k_leafCellType);
  if (GlobalPreferences::sharedGlobalPreferences()->listsStatsOrderInToolbox() == CountryPreferences::ListsStatsOrderInToolbox::Alternate && m_messageTreeModel->childrenList() == listsStatsChildren) {
    // We are in lists stats sub-menu
    selectedRow = alternateListsStatsOrder[selectedRow];
  }
  const ToolboxMessageTree * messageTree = messageTreeModelAtIndex(selectedRow);
  if (displayMessageTreeDisabledPopUp(messageTree)) {
    return true;
  }
  m_selectableTableView.deselectTable();

  // Translate the message
  const char * text = I18n::translate(messageTree->insertedText());
  char textToInsert[k_maxMessageSize]; // Has to be in the same scope as handleEventWithText
  if (messageTree->label() == messageTree->insertedText()) {
  // Remove the arguments if we kept one message for both inserted and displayed message
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

ExpressionTableCellWithMessage * MathToolbox::leafCellAtIndex(int index) {
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

int MathToolbox::controlChecksum() const {
  return static_cast<int>(Preferences::sharedPreferences()->examMode()) * I18n::NumberOfCountries + static_cast<int>(GlobalPreferences::sharedGlobalPreferences()->country());
}

int MathToolbox::indexAfterFork(const ToolboxMessageTree * forkMessageTree) const {
  if (forkMessageTree->childrenList() == arithmeticFork) {
    if (Poincare::Preferences::sharedPreferences()->mixedFractionsAreEnabled()) {
      return 0;
    }
    return 1;
  }
  Preferences::UnitFormat unitFormat = GlobalPreferences::sharedGlobalPreferences()->unitFormat();
  if (unitFormat == Preferences::UnitFormat::Metric) {
    return 0;
  }
  assert(unitFormat == Preferences::UnitFormat::Imperial);
  return 1;
}
