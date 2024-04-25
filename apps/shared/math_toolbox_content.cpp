#include <apps/i18n.h>
#include <escher/toolbox_message_tree.h>

#include <array>

using namespace Escher;

namespace Shared {

/* TODO
 * Unit submenu should be created from the Poincare::Unit::Representative table.
 * This would avoid to store duplicates const char *.
 * This would requires classes as:
 * - PointerTree parent of MessageTree and BufferTree
 * - MenuCell<PointerTextView> instead of MenuCell<MessageTextView>
 *
 * We should add in the model tree the possibility to indicate a Leaf that has
 * to be selected when the menu appears. */

/* We create one model tree: each node keeps the label of the row it refers to
 * and the text which would be edited by clicking on the row. When the node is a
 * subtree, the edited text is set at I18n::Message::Default. */

constexpr ToolboxMessage unitDistanceMeterPico =
    ToolboxMessageLeaf(I18n::Message::UnitDistanceMeterPicoSymbol,
                       I18n::Message::UnitDistanceMeterPico);
constexpr ToolboxMessage unitDistanceMeterNano =
    ToolboxMessageLeaf(I18n::Message::UnitDistanceMeterNanoSymbol,
                       I18n::Message::UnitDistanceMeterNano);
constexpr ToolboxMessage unitDistanceMeterMicro =
    ToolboxMessageLeaf(I18n::Message::UnitDistanceMeterMicroSymbol,
                       I18n::Message::UnitDistanceMeterMicro);
constexpr ToolboxMessage unitDistanceMeterMilli =
    ToolboxMessageLeaf(I18n::Message::UnitDistanceMeterMilliSymbol,
                       I18n::Message::UnitDistanceMeterMilli);
constexpr ToolboxMessage unitDistanceMeterCenti =
    ToolboxMessageLeaf(I18n::Message::UnitDistanceMeterCentiSymbol,
                       I18n::Message::UnitDistanceMeterCenti);
constexpr ToolboxMessage unitDistanceMeter = ToolboxMessageLeaf(
    I18n::Message::UnitDistanceMeterSymbol, I18n::Message::UnitDistanceMeter);
constexpr ToolboxMessage unitDistanceMeterKilo =
    ToolboxMessageLeaf(I18n::Message::UnitDistanceMeterKiloSymbol,
                       I18n::Message::UnitDistanceMeterKilo);
constexpr ToolboxMessage unitDistanceAstronomicalUnit =
    ToolboxMessageLeaf(I18n::Message::UnitDistanceAstronomicalUnitSymbol,
                       I18n::Message::UnitDistanceAstronomicalUnit);
constexpr ToolboxMessage unitDistanceLightYear =
    ToolboxMessageLeaf(I18n::Message::UnitDistanceLightYearSymbol,
                       I18n::Message::UnitDistanceLightYear);
constexpr ToolboxMessage unitDistanceParsec = ToolboxMessageLeaf(
    I18n::Message::UnitDistanceParsecSymbol, I18n::Message::UnitDistanceParsec);
constexpr ToolboxMessage unitDistanceInch = ToolboxMessageLeaf(
    I18n::Message::UnitDistanceInchSymbol, I18n::Message::UnitDistanceInch);
constexpr ToolboxMessage unitDistanceFoot = ToolboxMessageLeaf(
    I18n::Message::UnitDistanceFootSymbol, I18n::Message::UnitDistanceFoot);
constexpr ToolboxMessage unitDistanceYard = ToolboxMessageLeaf(
    I18n::Message::UnitDistanceYardSymbol, I18n::Message::UnitDistanceYard);
constexpr ToolboxMessage unitDistanceMile = ToolboxMessageLeaf(
    I18n::Message::UnitDistanceMileSymbol, I18n::Message::UnitDistanceMile);

constexpr const ToolboxMessage *const unitDistanceMeterChildren[] = {
    &unitDistanceMeterPico,  &unitDistanceMeterNano,  &unitDistanceMeterMicro,
    &unitDistanceMeterMilli, &unitDistanceMeterCenti, &unitDistanceMeter,
    &unitDistanceMeterKilo};
constexpr ToolboxMessage unitDistanceMeterNode = ToolboxMessageNode(
    I18n::Message::UnitMetricMenu, unitDistanceMeterChildren);
constexpr const ToolboxMessage *const unitDistanceChildrenForImperialToolbox[] =
    {&unitDistanceInch,
     &unitDistanceFoot,
     &unitDistanceYard,
     &unitDistanceMile,
     &unitDistanceAstronomicalUnit,
     &unitDistanceLightYear,
     &unitDistanceParsec,
     &unitDistanceMeterNode};
constexpr const ToolboxMessage *const unitDistanceImperialChildren[] = {
    &unitDistanceInch, &unitDistanceFoot, &unitDistanceYard, &unitDistanceMile};
constexpr ToolboxMessage unitDistanceImperialNode = ToolboxMessageNode(
    I18n::Message::UnitImperialMenu, unitDistanceImperialChildren);
constexpr const ToolboxMessage *const unitDistanceChildrenForMetricToolbox[] = {
    &unitDistanceMeterPico,   &unitDistanceMeterNano,
    &unitDistanceMeterMicro,  &unitDistanceMeterMilli,
    &unitDistanceMeterCenti,  &unitDistanceMeter,
    &unitDistanceMeterKilo,   &unitDistanceAstronomicalUnit,
    &unitDistanceLightYear,   &unitDistanceParsec,
    &unitDistanceImperialNode};

constexpr ToolboxMessage unitDistanceFork[] = {
    ToolboxMessageNode(I18n::Message::UnitDistanceMenu,
                       unitDistanceChildrenForMetricToolbox),
    ToolboxMessageNode(I18n::Message::UnitDistanceMenu,
                       unitDistanceChildrenForImperialToolbox)};

constexpr ToolboxMessage unitAngleToolbox[] = {
    ToolboxMessageLeaf(I18n::Message::UnitAngleDegreeSymbol,
                       I18n::Message::UnitAngleDegree),
    ToolboxMessageLeaf(I18n::Message::UnitAngleDMSShortcut,
                       I18n::Message::UnitAngleDMS, false,
                       I18n::Message::UnitAngleDMSShortcut2),
    ToolboxMessageLeaf(I18n::Message::UnitAngleRadianSymbol,
                       I18n::Message::UnitAngleRadian),
    ToolboxMessageLeaf(I18n::Message::UnitAngleGradianSymbol,
                       I18n::Message::UnitAngleGradian),
};

constexpr ToolboxMessage distanceAndAngleChildren[] = {
    ToolboxMessageNode(I18n::Message::UnitDistanceMenu, unitDistanceFork, true),
    ToolboxMessageNode(I18n::Message::UnitAngleMenu, unitAngleToolbox)};

constexpr ToolboxMessage unitTimeChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitTimeSecondNanoSymbol,
                       I18n::Message::UnitTimeSecondNano),
    ToolboxMessageLeaf(I18n::Message::UnitTimeSecondMicroSymbol,
                       I18n::Message::UnitTimeSecondMicro),
    ToolboxMessageLeaf(I18n::Message::UnitTimeSecondMilliSymbol,
                       I18n::Message::UnitTimeSecondMilli),
    ToolboxMessageLeaf(I18n::Message::UnitTimeSecondSymbol,
                       I18n::Message::UnitTimeSecond),
    ToolboxMessageLeaf(I18n::Message::UnitTimeMinuteSymbol,
                       I18n::Message::UnitTimeMinute),
    ToolboxMessageLeaf(I18n::Message::UnitTimeHourSymbol,
                       I18n::Message::UnitTimeHour),
    ToolboxMessageLeaf(I18n::Message::UnitTimeDaySymbol,
                       I18n::Message::UnitTimeDay),
    ToolboxMessageLeaf(I18n::Message::UnitTimeWeekSymbol,
                       I18n::Message::UnitTimeWeek),
    ToolboxMessageLeaf(I18n::Message::UnitTimeMonthSymbol,
                       I18n::Message::UnitTimeMonth),
    ToolboxMessageLeaf(I18n::Message::UnitTimeYearSymbol,
                       I18n::Message::UnitTimeYear)};

constexpr ToolboxMessage unitFrequencyHertzChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitFrequencyHertzSymbol,
                       I18n::Message::UnitFrequencyHertz),
    ToolboxMessageLeaf(I18n::Message::UnitFrequencyHertzKiloSymbol,
                       I18n::Message::UnitFrequencyHertzKilo),
    ToolboxMessageLeaf(I18n::Message::UnitFrequencyHertzMegaSymbol,
                       I18n::Message::UnitFrequencyHertzMega),
    ToolboxMessageLeaf(I18n::Message::UnitFrequencyHertzGigaSymbol,
                       I18n::Message::UnitFrequencyHertzGiga)};

constexpr ToolboxMessage timeAndFrequencyChildren[] = {
    ToolboxMessageNode(I18n::Message::UnitTimeMenu, unitTimeChildren),
    ToolboxMessageNode(I18n::Message::UnitFrequencyMenu,
                       unitFrequencyHertzChildren),
};

constexpr ToolboxMessage unitVolumeLiterMilli =
    ToolboxMessageLeaf(I18n::Message::UnitVolumeLiterMilliSymbol,
                       I18n::Message::UnitVolumeLiterMilli);
constexpr ToolboxMessage unitVolumeLiterCenti =
    ToolboxMessageLeaf(I18n::Message::UnitVolumeLiterCentiSymbol,
                       I18n::Message::UnitVolumeLiterCenti);
constexpr ToolboxMessage unitVolumeLiterDeci =
    ToolboxMessageLeaf(I18n::Message::UnitVolumeLiterDeciSymbol,
                       I18n::Message::UnitVolumeLiterDeci);
constexpr ToolboxMessage unitVolumeLiter = ToolboxMessageLeaf(
    I18n::Message::UnitVolumeLiterSymbol, I18n::Message::UnitVolumeLiter);
constexpr ToolboxMessage unitVolumeTeaspoon = ToolboxMessageLeaf(
    I18n::Message::UnitVolumeTeaspoonSymbol, I18n::Message::UnitVolumeTeaspoon);
constexpr ToolboxMessage unitVolumeTablespoon =
    ToolboxMessageLeaf(I18n::Message::UnitVolumeTablespoonSymbol,
                       I18n::Message::UnitVolumeTablespoon);
constexpr ToolboxMessage unitVolumeFluidOunce =
    ToolboxMessageLeaf(I18n::Message::UnitVolumeFluidOunceSymbol,
                       I18n::Message::UnitVolumeFluidOunce);
constexpr ToolboxMessage unitVolumeCup = ToolboxMessageLeaf(
    I18n::Message::UnitVolumeCupSymbol, I18n::Message::UnitVolumeCup);
constexpr ToolboxMessage unitVolumePint = ToolboxMessageLeaf(
    I18n::Message::UnitVolumePintSymbol, I18n::Message::UnitVolumePint);
constexpr ToolboxMessage unitVolumeQuart = ToolboxMessageLeaf(
    I18n::Message::UnitVolumeQuartSymbol, I18n::Message::UnitVolumeQuart);
constexpr ToolboxMessage unitVolumeGallon = ToolboxMessageLeaf(
    I18n::Message::UnitVolumeGallonSymbol, I18n::Message::UnitVolumeGallon);

constexpr const ToolboxMessage *const unitVolumeLiterChildren[] = {
    &unitVolumeLiterMilli, &unitVolumeLiterCenti, &unitVolumeLiterDeci,
    &unitVolumeLiter};
constexpr ToolboxMessage unitVolumeLiterNode =
    ToolboxMessageNode(I18n::Message::UnitMetricMenu, unitVolumeLiterChildren);
constexpr const ToolboxMessage *const unitVolumeChildrenForImperialToolbox[] = {
    &unitVolumeTeaspoon, &unitVolumeTablespoon, &unitVolumeFluidOunce,
    &unitVolumeCup,      &unitVolumePint,       &unitVolumeQuart,
    &unitVolumeGallon,   &unitVolumeLiterNode};
constexpr const ToolboxMessage *const unitVolumeImperialChildren[] = {
    &unitVolumeTeaspoon, &unitVolumeTablespoon, &unitVolumeFluidOunce,
    &unitVolumeCup,      &unitVolumePint,       &unitVolumeQuart,
    &unitVolumeGallon};
constexpr ToolboxMessage unitVolumeImperialNode = ToolboxMessageNode(
    I18n::Message::UnitImperialMenu, unitVolumeImperialChildren);
constexpr const ToolboxMessage *const unitVolumeChildrenForMetricToolbox[] = {
    &unitVolumeLiterMilli, &unitVolumeLiterCenti, &unitVolumeLiterDeci,
    &unitVolumeLiter, &unitVolumeImperialNode};

constexpr ToolboxMessage unitVolumeFork[] = {
    ToolboxMessageNode(I18n::Message::UnitVolumeMenu,
                       unitVolumeChildrenForMetricToolbox),
    ToolboxMessageNode(I18n::Message::UnitVolumeMenu,
                       unitVolumeChildrenForImperialToolbox),
};

constexpr ToolboxMessage unitSurfaceHectar = ToolboxMessageLeaf(
    I18n::Message::UnitSurfaceHectarSymbol, I18n::Message::UnitSurfaceHectar);
constexpr ToolboxMessage unitSurfaceAcre = ToolboxMessageLeaf(
    I18n::Message::UnitSurfaceAcreSymbol, I18n::Message::UnitSurfaceAcre);

constexpr const ToolboxMessage *const unitSurfaceMetricChildren[] = {
    &unitSurfaceHectar};
constexpr ToolboxMessage unitSurfaceMetricNode = ToolboxMessageNode(
    I18n::Message::UnitMetricMenu, unitSurfaceMetricChildren);
constexpr const ToolboxMessage *const unitSurfaceChildrenForImperialToolbox[] =
    {&unitSurfaceAcre, &unitSurfaceMetricNode};

constexpr const ToolboxMessage *const unitSurfaceImperialChildren[] = {
    &unitSurfaceAcre};
constexpr ToolboxMessage unitSurfaceImperialNode = ToolboxMessageNode(
    I18n::Message::UnitImperialMenu, unitSurfaceImperialChildren);
constexpr const ToolboxMessage *const unitSurfaceChildrenForMetricToolbox[] = {
    &unitSurfaceHectar, &unitSurfaceImperialNode};

constexpr ToolboxMessage unitSurfaceFork[] = {
    ToolboxMessageNode(I18n::Message::UnitSurfaceMenu,
                       unitSurfaceChildrenForMetricToolbox),
    ToolboxMessageNode(I18n::Message::UnitSurfaceMenu,
                       unitSurfaceChildrenForImperialToolbox)};

constexpr ToolboxMessage volumeAndSurfaceChildren[] = {
    ToolboxMessageNode(I18n::Message::UnitVolumeMenu, unitVolumeFork, true),
    ToolboxMessageNode(I18n::Message::UnitSurfaceMenu, unitSurfaceFork, true),
};

constexpr ToolboxMessage unitMassGramMicro = ToolboxMessageLeaf(
    I18n::Message::UnitMassGramMicroSymbol, I18n::Message::UnitMassGramMicro);
constexpr ToolboxMessage unitMassGramMilli = ToolboxMessageLeaf(
    I18n::Message::UnitMassGramMilliSymbol, I18n::Message::UnitMassGramMilli);
constexpr ToolboxMessage unitMassGram = ToolboxMessageLeaf(
    I18n::Message::UnitMassGramSymbol, I18n::Message::UnitMassGram);
constexpr ToolboxMessage unitMassGramKilo = ToolboxMessageLeaf(
    I18n::Message::UnitMassGramKiloSymbol, I18n::Message::UnitMassGramKilo);
constexpr ToolboxMessage unitMassTonne = ToolboxMessageLeaf(
    I18n::Message::UnitMassTonneSymbol, I18n::Message::UnitMassTonne);
constexpr ToolboxMessage unitMassOunce = ToolboxMessageLeaf(
    I18n::Message::UnitMassOunceSymbol, I18n::Message::UnitMassOunce);
constexpr ToolboxMessage unitMassPound = ToolboxMessageLeaf(
    I18n::Message::UnitMassPoundSymbol, I18n::Message::UnitMassPound);
constexpr ToolboxMessage unitMassShortTon = ToolboxMessageLeaf(
    I18n::Message::UnitMassShortTonSymbol, I18n::Message::UnitMassShortTon);
constexpr ToolboxMessage unitMassLongTon = ToolboxMessageLeaf(
    I18n::Message::UnitMassLongTonSymbol, I18n::Message::UnitMassLongTon);

constexpr const ToolboxMessage *const unitMassGramChildren[] = {
    &unitMassGramMicro, &unitMassGramMilli, &unitMassGram, &unitMassGramKilo,
    &unitMassTonne};
constexpr ToolboxMessage unitMassGramNode =
    ToolboxMessageNode(I18n::Message::UnitMetricMenu, unitMassGramChildren);
constexpr const ToolboxMessage *const unitMassChildrenForImperialToolbox[] = {
    &unitMassOunce, &unitMassPound, &unitMassShortTon, &unitMassLongTon,
    &unitMassGramNode};
constexpr const ToolboxMessage *const unitMassImperialChildren[] = {
    &unitMassOunce, &unitMassPound, &unitMassShortTon, &unitMassLongTon};
constexpr ToolboxMessage unitMassImperialNode = ToolboxMessageNode(
    I18n::Message::UnitImperialMenu, unitMassImperialChildren);
constexpr const ToolboxMessage *const unitMassChildrenForMetricToolbox[] = {
    &unitMassGramMicro, &unitMassGramMilli, &unitMassGram,
    &unitMassGramKilo,  &unitMassTonne,     &unitMassImperialNode};

constexpr ToolboxMessage unitMassFork[] = {
    ToolboxMessageNode(I18n::Message::UnitMassMenu,
                       unitMassChildrenForMetricToolbox),
    ToolboxMessageNode(I18n::Message::UnitMassMenu,
                       unitMassChildrenForImperialToolbox),
};

constexpr ToolboxMessage unitCurrentAmpereChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitCurrentAmpereMicroSymbol,
                       I18n::Message::UnitCurrentAmpereMicro),
    ToolboxMessageLeaf(I18n::Message::UnitCurrentAmpereMilliSymbol,
                       I18n::Message::UnitCurrentAmpereMilli),
    ToolboxMessageLeaf(I18n::Message::UnitCurrentAmpereSymbol,
                       I18n::Message::UnitCurrentAmpere),
};

constexpr ToolboxMessage unitPotentialVoltChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitPotentialVoltMicroSymbol,
                       I18n::Message::UnitPotentialVoltMicro),
    ToolboxMessageLeaf(I18n::Message::UnitPotentialVoltMilliSymbol,
                       I18n::Message::UnitPotentialVoltMilli),
    ToolboxMessageLeaf(I18n::Message::UnitPotentialVoltSymbol,
                       I18n::Message::UnitPotentialVolt),
    ToolboxMessageLeaf(I18n::Message::UnitPotentialVoltKiloSymbol,
                       I18n::Message::UnitPotentialVoltKilo),
};

constexpr ToolboxMessage unitResistanceOhmChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitResistanceOhmSymbol,
                       I18n::Message::UnitResistanceOhm),
    ToolboxMessageLeaf(I18n::Message::UnitResistanceOhmKiloSymbol,
                       I18n::Message::UnitResistanceOhmKilo),
};

constexpr ToolboxMessage unitCapacitanceFaradChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitCapacitanceFaradMicroSymbol,
                       I18n::Message::UnitCapacitanceFaradMicro),
    ToolboxMessageLeaf(I18n::Message::UnitCapacitanceFaradMilliSymbol,
                       I18n::Message::UnitCapacitanceFaradMilli),
    ToolboxMessageLeaf(I18n::Message::UnitCapacitanceFaradSymbol,
                       I18n::Message::UnitCapacitanceFarad),
};

constexpr ToolboxMessage electricitOtherChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitInductanceHenrySymbol,
                       I18n::Message::UnitInductanceHenry),
    ToolboxMessageLeaf(I18n::Message::UnitChargeCoulombSymbol,
                       I18n::Message::UnitChargeCoulomb),
    /* ToolboxMessageLeaf(I18n::Message::UnitConductanceSiemensMilliSymbol,
     * I18n::Message::UnitConductanceSiemensMilli), */
    ToolboxMessageLeaf(I18n::Message::UnitConductanceSiemensSymbol,
                       I18n::Message::UnitConductanceSiemens),
    ToolboxMessageLeaf(I18n::Message::UnitMagneticFieldTeslaSymbol,
                       I18n::Message::UnitMagneticFieldTesla),
};

constexpr ToolboxMessage electricityChildren[] = {
    ToolboxMessageNode(I18n::Message::UnitCurrentMenu,
                       unitCurrentAmpereChildren),
    ToolboxMessageNode(I18n::Message::UnitPotentialMenu,
                       unitPotentialVoltChildren),
    ToolboxMessageNode(I18n::Message::UnitResistanceMenu,
                       unitResistanceOhmChildren),
    ToolboxMessageNode(I18n::Message::UnitCapacitanceMenu,
                       unitCapacitanceFaradChildren),
    ToolboxMessageNode(I18n::Message::OtherUnitsMenu, electricitOtherChildren),
};

constexpr ToolboxMessage unitForceNewtonChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitForceNewtonMilliSymbol,
                       I18n::Message::UnitForceNewtonMilli),
    ToolboxMessageLeaf(I18n::Message::UnitForceNewtonSymbol,
                       I18n::Message::UnitForceNewton),
    ToolboxMessageLeaf(I18n::Message::UnitForceNewtonKiloSymbol,
                       I18n::Message::UnitForceNewtonKilo),
};

constexpr ToolboxMessage unitPressureChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitPressurePascalSymbol,
                       I18n::Message::UnitPressurePascal),
    ToolboxMessageLeaf(I18n::Message::UnitPressurePascalHectoSymbol,
                       I18n::Message::UnitPressurePascalHecto),
    ToolboxMessageLeaf(I18n::Message::UnitPressureBarSymbol,
                       I18n::Message::UnitPressureBar),
    ToolboxMessageLeaf(I18n::Message::UnitPressureAtmSymbol,
                       I18n::Message::UnitPressureAtm)};

constexpr ToolboxMessage forceAndPressureChildren[] = {
    ToolboxMessageNode(I18n::Message::UnitForceMenu, unitForceNewtonChildren),
    ToolboxMessageNode(I18n::Message::UnitPressureMenu, unitPressureChildren),
};

constexpr ToolboxMessage unitEnergyJouleChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitEnergyJouleMilliSymbol,
                       I18n::Message::UnitEnergyJouleMilli),
    ToolboxMessageLeaf(I18n::Message::UnitEnergyJouleSymbol,
                       I18n::Message::UnitEnergyJoule),
    ToolboxMessageLeaf(I18n::Message::UnitEnergyJouleKiloSymbol,
                       I18n::Message::UnitEnergyJouleKilo),
};

constexpr ToolboxMessage unitEnergyElectronVoltChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitEnergyElectronVoltMilliSymbol,
                       I18n::Message::UnitEnergyElectronVoltMilli),
    ToolboxMessageLeaf(I18n::Message::UnitEnergyElectronVoltSymbol,
                       I18n::Message::UnitEnergyElectronVolt),
    ToolboxMessageLeaf(I18n::Message::UnitEnergyElectronVoltKiloSymbol,
                       I18n::Message::UnitEnergyElectronVoltKilo),
    ToolboxMessageLeaf(I18n::Message::UnitEnergyElectronVoltMegaSymbol,
                       I18n::Message::UnitEnergyElectronVoltMega),
};

constexpr ToolboxMessage unitEnergyChildren[] = {
    ToolboxMessageNode(I18n::Message::UnitEnergyJoule, unitEnergyJouleChildren),
    ToolboxMessageNode(I18n::Message::UnitEnergyElectronVolt,
                       unitEnergyElectronVoltChildren),
};

constexpr ToolboxMessage unitPowerChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitPowerWattMicroSymbol,
                       I18n::Message::UnitPowerWattMicro),
    ToolboxMessageLeaf(I18n::Message::UnitPowerWattMilliSymbol,
                       I18n::Message::UnitPowerWattMilli),
    ToolboxMessageLeaf(I18n::Message::UnitPowerWattSymbol,
                       I18n::Message::UnitPowerWatt),
    ToolboxMessageLeaf(I18n::Message::UnitPowerWattKiloSymbol,
                       I18n::Message::UnitPowerWattKilo),
    ToolboxMessageLeaf(I18n::Message::UnitPowerWattMegaSymbol,
                       I18n::Message::UnitPowerWattMega),
    ToolboxMessageLeaf(I18n::Message::UnitPowerWattGigaSymbol,
                       I18n::Message::UnitPowerWattGiga),
    ToolboxMessageLeaf(I18n::Message::UnitHorsePowerSymbol,
                       I18n::Message::UnitHorsePower),
};

constexpr ToolboxMessage energyAndPowerChildren[] = {
    ToolboxMessageNode(I18n::Message::UnitEnergyMenu, unitEnergyChildren),
    ToolboxMessageNode(I18n::Message::UnitPowerMenu, unitPowerChildren),
};

constexpr ToolboxMessage unitTemperatureKelvin =
    ToolboxMessageLeaf(I18n::Message::UnitTemperatureKelvinSymbol,
                       I18n::Message::UnitTemperatureKelvin);
constexpr ToolboxMessage unitTemperatureCelsius =
    ToolboxMessageLeaf(I18n::Message::UnitTemperatureCelsiusSymbol,
                       I18n::Message::UnitTemperatureCelsius);
constexpr ToolboxMessage unitTemperatureFahrenheit =
    ToolboxMessageLeaf(I18n::Message::UnitTemperatureFahrenheitSymbol,
                       I18n::Message::UnitTemperatureFahrenheit);

constexpr const ToolboxMessage *const
    unitTemperatureChildrenForMetricToolbox[] = {&unitTemperatureKelvin,
                                                 &unitTemperatureCelsius,
                                                 &unitTemperatureFahrenheit};
constexpr const ToolboxMessage *const
    unitTemperatureChildrenForImperialToolbox[] = {&unitTemperatureKelvin,
                                                   &unitTemperatureFahrenheit,
                                                   &unitTemperatureCelsius};

constexpr ToolboxMessage unitTemperatureFork[] = {
    ToolboxMessageNode(I18n::Message::UnitTemperatureMenu,
                       unitTemperatureChildrenForMetricToolbox),
    ToolboxMessageNode(I18n::Message::UnitTemperatureMenu,
                       unitTemperatureChildrenForImperialToolbox),
};

constexpr ToolboxMessage unitAmountMoleChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitAmountMoleMicroSymbol,
                       I18n::Message::UnitAmountMoleMicro),
    ToolboxMessageLeaf(I18n::Message::UnitAmountMoleMilliSymbol,
                       I18n::Message::UnitAmountMoleMilli),
    ToolboxMessageLeaf(I18n::Message::UnitAmountMoleSymbol,
                       I18n::Message::UnitAmountMole),
};

constexpr ToolboxMessage OtherChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitLuminousIntensityCandelaSymbol,
                       I18n::Message::UnitLuminousIntensityCandela),
    ToolboxMessageNode(I18n::Message::UnitAmountMenu, unitAmountMoleChildren),
};

constexpr ToolboxMessage ConstantsChildren[] = {
    ToolboxMessageLeaf(I18n::Message::SpeedOfLightSymbol,
                       I18n::Message::SpeedOfLight),
    ToolboxMessageLeaf(I18n::Message::ElementaryChargeSymbol,
                       I18n::Message::ElementaryCharge),
    ToolboxMessageLeaf(I18n::Message::GravitationalConstantSymbol,
                       I18n::Message::GravitationalConstant),
    ToolboxMessageLeaf(I18n::Message::AccelerationOfGravitySymbol,
                       I18n::Message::AccelerationOfGravity),
    ToolboxMessageLeaf(I18n::Message::BoltzmannConstantSymbol,
                       I18n::Message::BoltzmannConstant),
    ToolboxMessageLeaf(I18n::Message::CoulombConstantSymbol,
                       I18n::Message::CoulombConstant),
    ToolboxMessageLeaf(I18n::Message::ElectronMassSymbol,
                       I18n::Message::ElectronMass),
    ToolboxMessageLeaf(I18n::Message::NeutronMassSymbol,
                       I18n::Message::NeutronMass),
    ToolboxMessageLeaf(I18n::Message::ProtonMassSymbol,
                       I18n::Message::ProtonMass),
    ToolboxMessageLeaf(I18n::Message::AvogadroConstantSymbol,
                       I18n::Message::AvogadroConstant),
    ToolboxMessageLeaf(I18n::Message::MolarGasConstantSymbol,
                       I18n::Message::MolarGasConstant),
    ToolboxMessageLeaf(I18n::Message::VacuumPermittivitySymbol,
                       I18n::Message::VacuumPermittivity),
    ToolboxMessageLeaf(I18n::Message::VacuumPermeabilitySymbol,
                       I18n::Message::VacuumPermeability),
    ToolboxMessageLeaf(I18n::Message::PlanckConstantSymbol,
                       I18n::Message::PlanckConstant)};

constexpr ToolboxMessage unitChildren[] = {
    ToolboxMessageLeaf(I18n::Message::UnitConversionCommandWithArg,
                       I18n::Message::UnitConversion, false,
                       I18n::Message::UnitConversionCommand),
    ToolboxMessageNode(I18n::Message::UnitDistanceAndAngleMenu,
                       distanceAndAngleChildren),
    ToolboxMessageNode(I18n::Message::UnitTimeAndFrequencyMenu,
                       timeAndFrequencyChildren),
    ToolboxMessageNode(I18n::Message::UnitVolumeAndAreaMenu,
                       volumeAndSurfaceChildren),
    ToolboxMessageNode(I18n::Message::UnitMassMenu, unitMassFork, true),
    ToolboxMessageNode(I18n::Message::UnitElectricityMenu, electricityChildren),
    ToolboxMessageNode(I18n::Message::UnitForceAndPressureMenu,
                       forceAndPressureChildren),
    ToolboxMessageNode(I18n::Message::UnitEnergyAndPowerMenu,
                       energyAndPowerChildren),
    ToolboxMessageNode(I18n::Message::UnitTemperatureMenu, unitTemperatureFork,
                       true),
    ToolboxMessageNode(I18n::Message::OtherUnitsMenu, OtherChildren),
    ToolboxMessageNode(I18n::Message::Constants, ConstantsChildren),
};

constexpr ToolboxMessage calculChildren[] = {
    ToolboxMessageLeaf(I18n::Message::DiffCommandWithArg,
                       I18n::Message::DerivateNumber, false,
                       I18n::Message::DiffCommand),
    ToolboxMessageLeaf(I18n::Message::HigherOrderDiffCommandWithArg,
                       I18n::Message::HigherOrderDerivateNumber, false,
                       I18n::Message::HigherOrderDiffCommand),
    ToolboxMessageLeaf(I18n::Message::IntCommandWithArg,
                       I18n::Message::Integral, false,
                       I18n::Message::IntCommand),
    ToolboxMessageLeaf(I18n::Message::SumCommandWithArg, I18n::Message::Sum,
                       false, I18n::Message::SumCommand),
    ToolboxMessageLeaf(I18n::Message::ProductCommandWithArg,
                       I18n::Message::Product, false,
                       I18n::Message::ProductCommand)};

constexpr ToolboxMessage complexChildren[] = {
    ToolboxMessageLeaf(I18n::Message::AbsCommandWithArg,
                       I18n::Message::ComplexAbsoluteValue),
    ToolboxMessageLeaf(I18n::Message::ArgCommandWithArg,
                       I18n::Message::Argument),
    ToolboxMessageLeaf(I18n::Message::ReCommandWithArg,
                       I18n::Message::RealPart),
    ToolboxMessageLeaf(I18n::Message::ImCommandWithArg,
                       I18n::Message::ImaginaryPart),
    ToolboxMessageLeaf(I18n::Message::ConjCommandWithArg,
                       I18n::Message::Conjugate)};

constexpr ToolboxMessage combinatoricsChildren[] = {
    ToolboxMessageLeaf(I18n::Message::BinomialCommandWithArg,
                       I18n::Message::Combination),
    ToolboxMessageLeaf(I18n::Message::PermuteCommandWithArg,
                       I18n::Message::Permutation),
    ToolboxMessageLeaf(I18n::Message::FactorialCommandWithArg,
                       I18n::Message::Factorial, false,
                       I18n::Message::FactorialCommand),
};

constexpr ToolboxMessage normalDistributionChildren[] = {
    ToolboxMessageLeaf(I18n::Message::NormCDFCommandWithArg,
                       I18n::Message::NormCDF),
    ToolboxMessageLeaf(I18n::Message::NormCDFRangeCommandWithArg,
                       I18n::Message::NormCDFRange),
    ToolboxMessageLeaf(I18n::Message::InvNormCommandWithArg,
                       I18n::Message::InvNorm),
    ToolboxMessageLeaf(I18n::Message::NormPDFCommandWithArg,
                       I18n::Message::NormPDF)};

constexpr ToolboxMessage studentDistributionChildren[] = {
    ToolboxMessageLeaf(I18n::Message::StudentCDFCommandWithArg,
                       I18n::Message::StudentCDF),
    ToolboxMessageLeaf(I18n::Message::StudentCDFRangeCommandWithArg,
                       I18n::Message::StudentCDFRange),
    ToolboxMessageLeaf(I18n::Message::InvStudentCommandWithArg,
                       I18n::Message::InvStudent),
    ToolboxMessageLeaf(I18n::Message::StudentPDFCommandWithArg,
                       I18n::Message::StudentPDF)};

constexpr ToolboxMessage binomialDistributionChildren[] = {
    ToolboxMessageLeaf(I18n::Message::BinomialPDFCommandWithArg,
                       I18n::Message::BinomialPDF),
    ToolboxMessageLeaf(I18n::Message::BinomialCDFCommandWithArg,
                       I18n::Message::BinomialCDF),
    ToolboxMessageLeaf(I18n::Message::InvBinomialCommandWithArg,
                       I18n::Message::InvBinomial),
};

constexpr ToolboxMessage poissonDistributionChildren[] = {
    ToolboxMessageLeaf(I18n::Message::PoissonPDFCommandWithArg,
                       I18n::Message::PoissonPDF),
    ToolboxMessageLeaf(I18n::Message::PoissonCDFCommandWithArg,
                       I18n::Message::PoissonCDF),
};

constexpr ToolboxMessage geometricDistributionChildren[] = {
    ToolboxMessageLeaf(I18n::Message::GeomPDFCommandWithArg,
                       I18n::Message::GeomPDF),
    ToolboxMessageLeaf(I18n::Message::GeomCDFCommandWithArg,
                       I18n::Message::GeomCDF),
    ToolboxMessageLeaf(I18n::Message::GeomCDFRangeCommandWithArg,
                       I18n::Message::GeomCDFRange),
    ToolboxMessageLeaf(I18n::Message::InvGeomCommandWithArg,
                       I18n::Message::InvGeom)};

constexpr ToolboxMessage hypergeometricDistributionChildren[] = {
    ToolboxMessageLeaf(I18n::Message::HypergeomPDFCommandWithArg,
                       I18n::Message::HypergeomPDF),
    ToolboxMessageLeaf(I18n::Message::HypergeomCDFCommandWithArg,
                       I18n::Message::HypergeomCDF),
    ToolboxMessageLeaf(I18n::Message::HypergeomCDFRangeCommandWithArg,
                       I18n::Message::HypergeomCDFRange),
    ToolboxMessageLeaf(I18n::Message::InvHypergeomCommandWithArg,
                       I18n::Message::InvHypergeom)};

constexpr ToolboxMessage probabilityLawsChildren[] = {
    ToolboxMessageNode(I18n::Message::Normal, normalDistributionChildren),
    ToolboxMessageNode(I18n::Message::Student, studentDistributionChildren),
    ToolboxMessageNode(I18n::Message::Binomial, binomialDistributionChildren),
    ToolboxMessageNode(I18n::Message::Poisson, poissonDistributionChildren),
    ToolboxMessageNode(I18n::Message::Geometric, geometricDistributionChildren),
    ToolboxMessageNode(I18n::Message::Hypergeometric,
                       hypergeometricDistributionChildren),
};

constexpr ToolboxMessage randomChildren[] = {
    ToolboxMessageLeaf(I18n::Message::RandomCommandWithArg,
                       I18n::Message::RandomFloat),
    ToolboxMessageLeaf(I18n::Message::RandintCommandWithArg,
                       I18n::Message::RandomInteger),
    ToolboxMessageLeaf(I18n::Message::RandintNoRepeatCommandWithArg,
                       I18n::Message::RandomUniqueIntegers),
};

constexpr ToolboxMessage probabilityChildren[] = {
    ToolboxMessageNode(I18n::Message::Combinatorics, combinatoricsChildren),
    ToolboxMessageNode(I18n::Message::Distributions, probabilityLawsChildren),
    ToolboxMessageNode(I18n::Message::Random, randomChildren),
};

constexpr ToolboxMessage matricesChildren[] = {
    ToolboxMessageLeaf(I18n::Message::DeterminantCommandWithArg,
                       I18n::Message::Determinant),
    ToolboxMessageLeaf(I18n::Message::InverseCommandWithArg,
                       I18n::Message::Inverse),
    ToolboxMessageLeaf(I18n::Message::IndentityCommandWithArg,
                       I18n::Message::Identity),
    ToolboxMessageLeaf(I18n::Message::TraceCommandWithArg,
                       I18n::Message::Trace),
    ToolboxMessageLeaf(I18n::Message::RowEchelonFormCommandWithArg,
                       I18n::Message::RowEchelonForm),
    ToolboxMessageLeaf(I18n::Message::ReducedRowEchelonFormCommandWithArg,
                       I18n::Message::ReducedRowEchelonForm),
};

constexpr ToolboxMessage vectorsChildren[] = {
    ToolboxMessageLeaf(I18n::Message::DotCommandWithArg, I18n::Message::Dot),
    ToolboxMessageLeaf(I18n::Message::CrossCommandWithArg,
                       I18n::Message::Cross),
    ToolboxMessageLeaf(I18n::Message::NormVectorCommandWithArg,
                       I18n::Message::NormVector),
};

constexpr ToolboxMessage matricesVectorsChildren[] = {
    ToolboxMessageLeaf(I18n::Message::MatrixCommandWithArg,
                       I18n::Message::NewMatrix, false,
                       I18n::Message::MatrixCommand),
    ToolboxMessageLeaf(I18n::Message::TransposeCommandWithArg,
                       I18n::Message::Transpose),
    ToolboxMessageLeaf(I18n::Message::DimensionCommandWithArg,
                       I18n::Message::Dimension),
    ToolboxMessageNode(I18n::Message::Matrices, matricesChildren),
    ToolboxMessageNode(I18n::Message::Vectors, vectorsChildren),
};

constexpr ToolboxMessage arithmeticChildrenWithMixedFractions[] = {
    ToolboxMessageLeaf(I18n::Message::GcdCommandWithArg,
                       I18n::Message::GreatCommonDivisor),
    ToolboxMessageLeaf(I18n::Message::LcmCommandWithArg,
                       I18n::Message::LeastCommonMultiple),
    ToolboxMessageLeaf(I18n::Message::FactorCommandWithArg,
                       I18n::Message::PrimeFactorDecomposition),
    ToolboxMessageLeaf(I18n::Message::MixedFractionCommandWithArg,
                       I18n::Message::MixedFraction, false,
                       I18n::Message::MixedFractionCommand),
    ToolboxMessageLeaf(I18n::Message::RemCommandWithArg,
                       I18n::Message::Remainder),
    ToolboxMessageLeaf(I18n::Message::QuoCommandWithArg,
                       I18n::Message::Quotient)};

constexpr ToolboxMessage arithmeticChildrenWithoutMixedFractions[] = {
    ToolboxMessageLeaf(I18n::Message::GcdCommandWithArg,
                       I18n::Message::GreatCommonDivisor),
    ToolboxMessageLeaf(I18n::Message::LcmCommandWithArg,
                       I18n::Message::LeastCommonMultiple),
    ToolboxMessageLeaf(I18n::Message::FactorCommandWithArg,
                       I18n::Message::PrimeFactorDecomposition),
    ToolboxMessageLeaf(I18n::Message::RemCommandWithArg,
                       I18n::Message::Remainder),
    ToolboxMessageLeaf(I18n::Message::QuoCommandWithArg,
                       I18n::Message::Quotient)};

extern constexpr ToolboxMessage arithmeticFork[] = {
    ToolboxMessageNode(I18n::Message::Arithmetic,
                       arithmeticChildrenWithMixedFractions),
    ToolboxMessageNode(I18n::Message::Arithmetic,
                       arithmeticChildrenWithoutMixedFractions)};

constexpr ToolboxMessage hyperbolicTrigonometryChildren[] = {
    ToolboxMessageLeaf(I18n::Message::SinhCommandWithArg,
                       I18n::Message::HyperbolicSine),
    ToolboxMessageLeaf(I18n::Message::CoshCommandWithArg,
                       I18n::Message::HyperbolicCosine),
    ToolboxMessageLeaf(I18n::Message::TanhCommandWithArg,
                       I18n::Message::HyperbolicTangent),
    ToolboxMessageLeaf(I18n::Message::AsinhCommandWithArg,
                       I18n::Message::InverseHyperbolicSine),
    ToolboxMessageLeaf(I18n::Message::AcoshCommandWithArg,
                       I18n::Message::InverseHyperbolicCosine),
    ToolboxMessageLeaf(I18n::Message::AtanhCommandWithArg,
                       I18n::Message::InverseHyperbolicTangent)};

constexpr ToolboxMessage advancedTrigonometryChildren[] = {
    ToolboxMessageLeaf(I18n::Message::CscCommandWithArg,
                       I18n::Message::Cosecant),
    ToolboxMessageLeaf(I18n::Message::SecCommandWithArg, I18n::Message::Secant),
    ToolboxMessageLeaf(I18n::Message::CotCommandWithArg,
                       I18n::Message::Cotangent),
    ToolboxMessageLeaf(I18n::Message::AcscCommandWithArg,
                       I18n::Message::ArcCosecant),
    ToolboxMessageLeaf(I18n::Message::AsecCommandWithArg,
                       I18n::Message::ArcSecant),
    ToolboxMessageLeaf(I18n::Message::AcotCommandWithArg,
                       I18n::Message::ArcCotangent)};

constexpr ToolboxMessage trigonometryChildren[] = {
    ToolboxMessageNode(I18n::Message::HyperbolicTrigonometry,
                       hyperbolicTrigonometryChildren),
    ToolboxMessageNode(I18n::Message::AdvancedTrigonometry,
                       advancedTrigonometryChildren),
};

constexpr ToolboxMessage decimalNumbersChildren[] = {
    ToolboxMessageLeaf(I18n::Message::FloorCommandWithArg,
                       I18n::Message::Floor),
    ToolboxMessageLeaf(I18n::Message::FracCommandWithArg,
                       I18n::Message::FracPart),
    ToolboxMessageLeaf(I18n::Message::CeilCommandWithArg,
                       I18n::Message::Ceiling),
    ToolboxMessageLeaf(I18n::Message::RoundCommandWithArg,
                       I18n::Message::Rounding),
};

extern constexpr ToolboxMessage listsStatsChildren[] = {
    ToolboxMessageLeaf(I18n::Message::ListMeanCommandWithArg,
                       I18n::Message::Mean),
    ToolboxMessageLeaf(I18n::Message::ListStandardDevCommandWithArg,
                       I18n::Message::StandardDeviation),
    ToolboxMessageLeaf(I18n::Message::ListMedianCommandWithArg,
                       I18n::Message::Median),
    ToolboxMessageLeaf(I18n::Message::ListVarianceCommandWithArg,
                       I18n::Message::Deviation),
    ToolboxMessageLeaf(I18n::Message::ListSampleStandardDevCommandWithArg,
                       I18n::Message::SampleSTD)};

extern constexpr int alternateListsStatsOrder[]{
    0,  // Mean
    1,  // StandardDev
    4,  // SampleStandardDev
    2,  // Median
    3   // Variance
};

static_assert(std::size(alternateListsStatsOrder) ==
                  std::size(listsStatsChildren),
              "Alternate lists stats order in toolbox has wrong size");

constexpr ToolboxMessage listsOperationsChildren[] = {
    ToolboxMessageLeaf(I18n::Message::ListLengthCommandWithArg,
                       I18n::Message::ListLength),
    ToolboxMessageLeaf(I18n::Message::ListMinCommandWithArg,
                       I18n::Message::Minimum),
    ToolboxMessageLeaf(I18n::Message::ListMaxCommandWithArg,
                       I18n::Message::Maximum),
    ToolboxMessageLeaf(I18n::Message::ListSortCommandWithArg,
                       I18n::Message::AscendingSorting),
    ToolboxMessageLeaf(I18n::Message::ListSumCommandWithArg,
                       I18n::Message::SumOfElements),
    ToolboxMessageLeaf(I18n::Message::ListProductCommandWithArg,
                       I18n::Message::ProductOfElements)};

constexpr ToolboxMessage listsChildren[] = {
    ToolboxMessageLeaf(I18n::Message::ListCommandWithArg,
                       I18n::Message::NewList, false,
                       I18n::Message::ListCommand),
    ToolboxMessageLeaf(I18n::Message::ListSequenceCommandWithArg,
                       I18n::Message::ListSequenceDescription, false,
                       I18n::Message::ListSequenceCommand),
    ToolboxMessageNode(I18n::Message::StatsApp, listsStatsChildren),
    ToolboxMessageNode(I18n::Message::Operations, listsOperationsChildren)};

constexpr ToolboxMessage logicChildren[] = {
    ToolboxMessageLeaf(I18n::Message::PiecewiseCommandWithArgs,
                       I18n::Message::PieceWise, false,
                       I18n::Message::PiecewiseCommand),
    ToolboxMessageLeaf(I18n::Message::LessOrEqual,
                       I18n::Message::LessOrEqualText),
    ToolboxMessageLeaf(I18n::Message::GreaterOrEqual,
                       I18n::Message::GreaterOrEqualText),
    ToolboxMessageLeaf(I18n::Message::NotEqual, I18n::Message::NotEqualText),
    ToolboxMessageLeaf(I18n::Message::AndCommand, I18n::Message::AndMessage,
                       false, I18n::Message::AndCommandWithSpaces),
    ToolboxMessageLeaf(I18n::Message::OrCommand, I18n::Message::OrMessage,
                       false, I18n::Message::OrCommandWithSpaces),
    ToolboxMessageLeaf(I18n::Message::NotCommand, I18n::Message::NotMessage,
                       false, I18n::Message::NotCommandWithSpace),
    ToolboxMessageLeaf(I18n::Message::XorCommand, I18n::Message::XorMessage,
                       false, I18n::Message::XorCommandWithSpaces),
    ToolboxMessageLeaf(I18n::Message::NorCommand, I18n::Message::NorMessage,
                       false, I18n::Message::NorCommandWithSpaces),
    ToolboxMessageLeaf(I18n::Message::NandCommand, I18n::Message::NandMessage,
                       false, I18n::Message::NandCommandWithSpaces)};

constexpr ToolboxMessage menu[] = {
    ToolboxMessageLeaf(I18n::Message::AbsCommandWithArg,
                       I18n::Message::AbsoluteValue),
    ToolboxMessageLeaf(I18n::Message::RootCommandWithArg,
                       I18n::Message::NthRoot),
    ToolboxMessageLeaf(I18n::Message::LogCommandWithArg,
                       I18n::Message::BasedLogarithm),
    ToolboxMessageNode(I18n::Message::Calculus, calculChildren),
    ToolboxMessageNode(I18n::Message::ComplexNumber, complexChildren),
    ToolboxMessageNode(I18n::Message::Probability, probabilityChildren),
    ToolboxMessageNode(I18n::Message::UnitAndConstant, unitChildren),
    ToolboxMessageNode(I18n::Message::MatricesAndVectors,
                       matricesVectorsChildren),
    ToolboxMessageNode(I18n::Message::Lists, listsChildren),
    ToolboxMessageNode(I18n::Message::Arithmetic, arithmeticFork, true),
    ToolboxMessageNode(I18n::Message::Trigonometry, trigonometryChildren),
    ToolboxMessageNode(I18n::Message::DecimalNumbers, decimalNumbersChildren),
    ToolboxMessageNode(I18n::Message::Logic, logicChildren)};

extern constexpr ToolboxMessage toolboxModel =
    ToolboxMessageNode(I18n::Message::Toolbox, menu);

}  // namespace Shared
