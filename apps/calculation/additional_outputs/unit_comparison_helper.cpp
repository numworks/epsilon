#include "unit_comparison_helper.h"
#include "../app.h"
#include <apps/i18n.h>
#include <cmath>
#include <poincare/multiplication.h>
#include <poincare/print_float.h>
#include <poincare/float.h>
#include <poincare/unit.h>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

namespace UnitComparison {

// If you add new reference value, they always need to be sorted in increasing order.

constexpr static const ReferenceValue k_lengthReferences[] = {
  ReferenceValue({I18n::Message::AtomNucleusDiameterTitle1, I18n::Message::AtomNucleusDiameterTitle2, I18n::Message::AtomNucleusDiameterSubtitle, 1e-15}),
  ReferenceValue({I18n::Message::HydrogenAtomRadiusTitle1, I18n::Message::HydrogenAtomRadiusTitle2, I18n::Message::HydrogenAtomRadiusSubtitle, 1.06e-10}),
  ReferenceValue({I18n::Message::AdnDiameterTitle1, I18n::Message::AdnDiameterTitle2, I18n::Message::AdnDiameterSubtitle, 2e-09}),
  ReferenceValue({I18n::Message::VirusSizeTitle1, I18n::Message::VirusSizeTitle2, I18n::Message::VirusSizeSubtitle, 2e-08}),
  ReferenceValue({I18n::Message::BacteriaSizeTitle1, I18n::Message::BacteriaSizeTitle2, I18n::Message::BacteriaSizeSubtitle, 5e-06}),
  ReferenceValue({I18n::Message::SandGrainDiameterTitle1, I18n::Message::SandGrainDiameterTitle2, I18n::Message::SandGrainDiameterSubtitle, 6e-05}),
  ReferenceValue({I18n::Message::AcaridSizeTitle1, I18n::Message::AcaridSizeTitle2, I18n::Message::AcaridSizeSubtitle, 0.0001}),
  ReferenceValue({I18n::Message::RiceGrainLengthTitle1, I18n::Message::RiceGrainLengthTitle2, I18n::Message::RiceGrainLengthSubtitle, 0.006}),
  ReferenceValue({I18n::Message::CalculatorThicknessTitle1, I18n::Message::CalculatorThicknessTitle2, I18n::Message::CalculatorThicknessSubtitle, 0.01}),
  ReferenceValue({I18n::Message::StepLengthTitle1, I18n::Message::StepLengthTitle2, I18n::Message::StepLengthSubtitle, 0.7}),
  ReferenceValue({I18n::Message::HumanHeigthTitle1, I18n::Message::HumanHeigthTitle2, I18n::Message::HumanHeigthSubtitle, 1.65}),
  ReferenceValue({I18n::Message::SoccerFieldWidthTitle1, I18n::Message::SoccerFieldWidthTitle2, I18n::Message::SoccerFieldWidthSubtitle, 68.0}),
  ReferenceValue({I18n::Message::EiffelTowerHeigthTitle1, I18n::Message::EiffelTowerHeigthTitle2, I18n::Message::EiffelTowerHeigthSubtitle, 324.0}),
  ReferenceValue({I18n::Message::EverestAltitudeTitle1, I18n::Message::EverestAltitudeTitle2, I18n::Message::EverestAltitudeSubtitle, 8850.0}),
  ReferenceValue({I18n::Message::PortugalLengthTitle1, I18n::Message::PortugalLengthTitle2, I18n::Message::PortugalLengthSubtitle, 560000.0}),
  ReferenceValue({I18n::Message::NileLengthTitle1, I18n::Message::NileLengthTitle2, I18n::Message::NileLengthSubtitle, 6650000.0}),
  ReferenceValue({I18n::Message::EarthCircumferenceTitle1, I18n::Message::EarthCircumferenceTitle2, I18n::Message::EarthCircumferenceSubtitle, 40100000.0}),
  ReferenceValue({I18n::Message::MoonDistanceTitle1, I18n::Message::MoonDistanceTitle2, I18n::Message::MoonDistanceSubtitle, 384000000.0}),
  ReferenceValue({I18n::Message::SunDiameterTitle1, I18n::Message::SunDiameterTitle2, I18n::Message::SunDiameterSubtitle, 1390000000.0}),
  ReferenceValue({I18n::Message::SunDistanceTitle1, I18n::Message::SunDistanceTitle2, I18n::Message::SunDistanceSubtitle, 150000000000.0}),
  ReferenceValue({I18n::Message::SaturnDistanceTitle1, I18n::Message::SaturnDistanceTitle2, I18n::Message::SaturnDistanceSubtitle, 1400000000000.0}),
  ReferenceValue({I18n::Message::SolarSystemRadiusTitle1, I18n::Message::SolarSystemRadiusTitle2, I18n::Message::SolarSystemRadiusSubtitle, 36000000000000.0}),
  ReferenceValue({I18n::Message::SiriusDistanceTitle1, I18n::Message::SiriusDistanceTitle2, I18n::Message::SiriusDistanceSubtitle, 8e+16}),
  ReferenceValue({I18n::Message::MilkyWayDiameterTitle1, I18n::Message::MilkyWayDiameterTitle2, I18n::Message::MilkyWayDiameterSubtitle, 9e+20}),
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const ReferenceValue k_massReferences[] = {
  ReferenceValue({I18n::Message::ElectronWeightTitle1, I18n::Message::ElectronWeightTitle2, I18n::Message::ElectronWeightSubtitle, 9.1e-31}),
  ReferenceValue({I18n::Message::ProtonWeightTitle1, I18n::Message::ProtonWeightTitle2, I18n::Message::ProtonWeightSubtitle, 1.67e-27}),
  ReferenceValue({I18n::Message::WaterMoleculeWeightTitle1, I18n::Message::WaterMoleculeWeightTitle2, I18n::Message::WaterMoleculeWeightSubtitle, 2.99e-26}),
  ReferenceValue({I18n::Message::CaffeineWeightTitle1, I18n::Message::CaffeineWeightTitle2, I18n::Message::CaffeineWeightSubtitle, 3.22e-25}),
  ReferenceValue({I18n::Message::VirusWeightTitle1, I18n::Message::VirusWeightTitle2, I18n::Message::VirusWeightSubtitle, 1e-17}),
  ReferenceValue({I18n::Message::BacteriaWeightTitle1, I18n::Message::BacteriaWeightTitle2, I18n::Message::BacteriaWeightSubtitle, 3e-16}),
  ReferenceValue({I18n::Message::HumanCellWeightTitle1, I18n::Message::HumanCellWeightTitle2, I18n::Message::HumanCellWeightSubtitle, 1e-12}),
  ReferenceValue({I18n::Message::SandGrainWeightTitle1, I18n::Message::SandGrainWeightTitle2, I18n::Message::SandGrainWeightSubtitle, 3e-09}),
  ReferenceValue({I18n::Message::MousquitoWeightTitle1, I18n::Message::MousquitoWeightTitle2, I18n::Message::MousquitoWeightSubtitle, 2.5e-06}),
  ReferenceValue({I18n::Message::RiceGrainWeightTitle1, I18n::Message::RiceGrainWeightTitle2, I18n::Message::RiceGrainWeightSubtitle, 1.5e-05}),
  ReferenceValue({I18n::Message::EuroCoinWeightTitle1, I18n::Message::EuroCoinWeightTitle2, I18n::Message::EuroCoinWeightSubtitle, 0.0075}),
  ReferenceValue({I18n::Message::HenEggWeightTitle1, I18n::Message::HenEggWeightTitle2, I18n::Message::HenEggWeightSubtitle, 0.06}),
  ReferenceValue({I18n::Message::OrangeWeightTitle1, I18n::Message::OrangeWeightTitle2, I18n::Message::OrangeWeightSubtitle, 0.1}),
  ReferenceValue({I18n::Message::CatWeightTitle1, I18n::Message::CatWeightTitle2, I18n::Message::CatWeightSubtitle, 4.0}),
  ReferenceValue({I18n::Message::HumanWeightTitle1, I18n::Message::HumanWeightTitle2, I18n::Message::HumanWeightSubtitle, 65.0}),
  ReferenceValue({I18n::Message::CowWeightTitle1, I18n::Message::CowWeightTitle2, I18n::Message::CowWeightSubtitle, 700.0}),
  ReferenceValue({I18n::Message::ElephantWeightTitle1, I18n::Message::ElephantWeightTitle2, I18n::Message::ElephantWeightSubtitle, 5000.0}),
  ReferenceValue({I18n::Message::BigBenWeightTitle1, I18n::Message::BigBenWeightTitle2, I18n::Message::BigBenWeightSubtitle, 13700.0}),
  ReferenceValue({I18n::Message::WhaleWeightTitle1, I18n::Message::WhaleWeightTitle2, I18n::Message::WhaleWeightSubtitle, 150000.0}),
  ReferenceValue({I18n::Message::EiffelTowerWeightTitle1, I18n::Message::EiffelTowerWeightTitle2, I18n::Message::EiffelTowerWeightSubtitle, 7300000.0}),
  ReferenceValue({I18n::Message::TitanicWeightTitle1, I18n::Message::TitanicWeightTitle2, I18n::Message::TitanicWeightSubtitle, 52300000.0}),
  ReferenceValue({I18n::Message::EmpireStateBuildingWeightTitle1, I18n::Message::EmpireStateBuildingWeightTitle2, I18n::Message::EmpireStateBuildingWeightSubtitle, 365000000.0}),
  ReferenceValue({I18n::Message::CheopsPyramidWeightTitle1, I18n::Message::CheopsPyramidWeightTitle2, I18n::Message::CheopsPyramidWeightSubtitle, 6000000000.0}),
  ReferenceValue({I18n::Message::EarthAtmosphereWeightTitle1, I18n::Message::EarthAtmosphereWeightTitle2, I18n::Message::EarthAtmosphereWeightSubtitle, 5e+18}),
  ReferenceValue({I18n::Message::OceansWeightTitle1, I18n::Message::OceansWeightTitle2, I18n::Message::OceansWeightSubtitle, 1.4e+21}),
  ReferenceValue({I18n::Message::MoonWeightTitle1, I18n::Message::MoonWeightTitle2, I18n::Message::MoonWeightSubtitle, 7.35e+22}),
  ReferenceValue({I18n::Message::MarsWeightTitle1, I18n::Message::MarsWeightTitle2, I18n::Message::MarsWeightSubtitle, 6.42e+23}),
  ReferenceValue({I18n::Message::EarthWeightTitle1, I18n::Message::EarthWeightTitle2, I18n::Message::EarthWeightSubtitle, 5.97e+24}),
  ReferenceValue({I18n::Message::UranusWeightTitle1, I18n::Message::UranusWeightTitle2, I18n::Message::UranusWeightSubtitle, 8.68e+25}),
  ReferenceValue({I18n::Message::SaturnWeightTitle1, I18n::Message::SaturnWeightTitle2, I18n::Message::SaturnWeightSubtitle, 5.68e+26}),
  ReferenceValue({I18n::Message::JupiterWeightTitle1, I18n::Message::JupiterWeightTitle2, I18n::Message::JupiterWeightSubtitle, 1.9e+27}),
  ReferenceValue({I18n::Message::SunWeightTitle1, I18n::Message::SunWeightTitle2, I18n::Message::SunWeightSubtitle, 1.99e+30}),
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const ReferenceValue k_pressureReferences[] = {
  ReferenceValue({I18n::Message::MoonAtmospherePressureTitle1, I18n::Message::MoonAtmospherePressureTitle2, I18n::Message::MoonAtmospherePressureSubtitle, 2e-10}),
  ReferenceValue({I18n::Message::MarsAtmospherePressureTitle1, I18n::Message::MarsAtmospherePressureTitle2, I18n::Message::MarsAtmospherePressureSubtitle, 610.0}),
  ReferenceValue({I18n::Message::SoccerBallPressureTitle1, I18n::Message::SoccerBallPressureTitle2, I18n::Message::SoccerBallPressureSubtitle, 60000.0}),
  ReferenceValue({I18n::Message::EarthAtmospherePressureTitle1, I18n::Message::EarthAtmospherePressureTitle2, I18n::Message::EarthAtmospherePressureSubtitle, 101000.0}),
  ReferenceValue({I18n::Message::VenusAtmospherePressureTitle1, I18n::Message::VenusAtmospherePressureTitle2, I18n::Message::VenusAtmospherePressureSubtitle, 9200000.0}),
  ReferenceValue({I18n::Message::ScubaTankPressureTitle1, I18n::Message::ScubaTankPressureTitle2, I18n::Message::ScubaTankPressureSubtitle, 20000000.0}),
  ReferenceValue({I18n::Message::MarianaTrenchPressureTitle1, I18n::Message::MarianaTrenchPressureTitle2, I18n::Message::MarianaTrenchPressureSubtitle, 100000000.0}),
  ReferenceValue({I18n::Message::EarthCorePressureTitle1, I18n::Message::EarthCorePressureTitle2, I18n::Message::EarthCorePressureSubtitle, 360000000000.0}),
  ReferenceValue({I18n::Message::SunCorePressureTitle1, I18n::Message::SunCorePressureTitle2, I18n::Message::SunCorePressureSubtitle, 2.5e+16}),
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const ReferenceValue k_powerReferences[] = {
  ReferenceValue({I18n::Message::CdLaserPowerTitle1, I18n::Message::CdLaserPowerTitle2, I18n::Message::CdLaserPowerSubtitle, 0.005}),
  ReferenceValue({I18n::Message::LightBulbPowerTitle1, I18n::Message::LightBulbPowerTitle2, I18n::Message::LightBulbPowerSubtitle, 60.0}),
  ReferenceValue({I18n::Message::PcPowerTitle1, I18n::Message::PcPowerTitle2, I18n::Message::PcPowerSubtitle, 100.0}),
  ReferenceValue({I18n::Message::KettlePowerTitle1, I18n::Message::KettlePowerTitle2, I18n::Message::KettlePowerSubtitle, 1500.0}),
  ReferenceValue({I18n::Message::CameraFlashPowerTitle1, I18n::Message::CameraFlashPowerTitle2, I18n::Message::CameraFlashPowerSubtitle, 12000.0}),
  ReferenceValue({I18n::Message::TruckPowerTitle1, I18n::Message::TruckPowerTitle2, I18n::Message::TruckPowerSubtitle, 300000.0}),
  ReferenceValue({I18n::Message::WindTurbinePowerTitle1, I18n::Message::WindTurbinePowerTitle2, I18n::Message::WindTurbinePowerSubtitle, 2000000.0}),
  ReferenceValue({I18n::Message::QuennMaryPowerTitle1, I18n::Message::QuennMaryPowerTitle2, I18n::Message::QuennMaryPowerSubtitle, 120000000.0}),
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const ReferenceValue k_areaReferences[] = {
  ReferenceValue({I18n::Message::SpiderThreadSectionAreaTitle1, I18n::Message::SpiderThreadSectionAreaTitle2, I18n::Message::SpiderThreadSectionAreaSubtitle, 5e-12}),
  ReferenceValue({I18n::Message::SandGrainAreaTitle1, I18n::Message::SandGrainAreaTitle2, I18n::Message::SandGrainAreaSubtitle, 1e-08}),
  ReferenceValue({I18n::Message::PinAreaTitle1, I18n::Message::PinAreaTitle2, I18n::Message::PinAreaSubtitle, 5e-05}),
  ReferenceValue({I18n::Message::StampAreaTitle1, I18n::Message::StampAreaTitle2, I18n::Message::StampAreaSubtitle, 0.00054}),
  ReferenceValue({I18n::Message::SmartphoneScreenAreaTitle1, I18n::Message::SmartphoneScreenAreaTitle2, I18n::Message::SmartphoneScreenAreaSubtitle, 0.00992}),
  ReferenceValue({I18n::Message::ComputerScreenAreaTitle1, I18n::Message::ComputerScreenAreaTitle2, I18n::Message::ComputerScreenAreaSubtitle, 0.052}),
  ReferenceValue({I18n::Message::ClassTableAreaTitle1, I18n::Message::ClassTableAreaTitle2, I18n::Message::ClassTableAreaSubtitle, 0.3}),
  ReferenceValue({I18n::Message::ElevatorAreaTitle1, I18n::Message::ElevatorAreaTitle2, I18n::Message::ElevatorAreaSubtitle, 1.5}),
  ReferenceValue({I18n::Message::ParkingSpaceAreaTitle1, I18n::Message::ParkingSpaceAreaTitle2, I18n::Message::ParkingSpaceAreaSubtitle, 10.0}),
  ReferenceValue({I18n::Message::TennisCourtAreaTitle1, I18n::Message::TennisCourtAreaTitle2, I18n::Message::TennisCourtAreaSubtitle, 260.0}),
  ReferenceValue({I18n::Message::SoccerFieldAreaTitle1, I18n::Message::SoccerFieldAreaTitle2, I18n::Message::SoccerFieldAreaSubtitle, 7140.0}),
  ReferenceValue({I18n::Message::TajMahalAreaTitle1, I18n::Message::TajMahalAreaTitle2, I18n::Message::TajMahalAreaSubtitle, 30000.0}),
  ReferenceValue({I18n::Message::VaticanAreaTitle1, I18n::Message::VaticanAreaTitle2, I18n::Message::VaticanAreaSubtitle, 440000.0}),
  ReferenceValue({I18n::Message::MonacoAreaTitle1, I18n::Message::MonacoAreaTitle2, I18n::Message::MonacoAreaSubtitle, 2020000.0}),
  ReferenceValue({I18n::Message::ManhattanAreaTitle1, I18n::Message::ManhattanAreaTitle2, I18n::Message::ManhattanAreaSubtitle, 59100000.0}),
  ReferenceValue({I18n::Message::ParisAreaTitle1, I18n::Message::ParisAreaTitle2, I18n::Message::ParisAreaSubtitle, 105000000.0}),
  ReferenceValue({I18n::Message::LondonAreaTitle1, I18n::Message::LondonAreaTitle2, I18n::Message::LondonAreaSubtitle, 1570000000.0}),
  ReferenceValue({I18n::Message::ChannelAreaTitle1, I18n::Message::ChannelAreaTitle2, I18n::Message::ChannelAreaSubtitle, 75000000000.0}),
  ReferenceValue({I18n::Message::FranceAreaTitle1, I18n::Message::FranceAreaTitle2, I18n::Message::FranceAreaSubtitle, 672000000000.0}),
  ReferenceValue({I18n::Message::AustraliaAreaTitle1, I18n::Message::AustraliaAreaTitle2, I18n::Message::AustraliaAreaSubtitle, 7690000000000.0}),
  ReferenceValue({I18n::Message::AfricaAreaTitle1, I18n::Message::AfricaAreaTitle2, I18n::Message::AfricaAreaSubtitle, 30000000000000.0}),
  ReferenceValue({I18n::Message::EarthAreaTitle1, I18n::Message::EarthAreaTitle2, I18n::Message::EarthAreaSubtitle, 510000000000000.0}),
  ReferenceValue({I18n::Message::NeptuneAreaTitle1, I18n::Message::NeptuneAreaTitle2, I18n::Message::NeptuneAreaSubtitle, 7620000000000000.0}),
  ReferenceValue({I18n::Message::SaturnAreaTitle1, I18n::Message::SaturnAreaTitle2, I18n::Message::SaturnAreaSubtitle, 4.26e+16}),
  ReferenceValue({I18n::Message::SunAreaTitle1, I18n::Message::SunAreaTitle2, I18n::Message::SunAreaSubtitle, 6.08e+18}),
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const ReferenceValue k_timeReferences[] = {
  ReferenceValue({I18n::Message::XRayPeriodTitle1, I18n::Message::XRayPeriodTitle2, I18n::Message::XRayPeriodSubtitle, 1e-19}),
  ReferenceValue({I18n::Message::CameraFlashDurationTitle1, I18n::Message::CameraFlashDurationTitle2, I18n::Message::CameraFlashDurationSubtitle, 0.001}),
  ReferenceValue({I18n::Message::BlinkDurationTitle1, I18n::Message::BlinkDurationTitle2, I18n::Message::BlinkDurationSubtitle, 0.1}),
  ReferenceValue({I18n::Message::WorldRecord100mTitle1, I18n::Message::WorldRecord100mTitle2, I18n::Message::WorldRecord100mSubtitle, 9.58}),
  ReferenceValue({I18n::Message::FootballMatchDurationTitle1, I18n::Message::FootballMatchDurationTitle2, I18n::Message::FootballMatchDurationSubtitle, 5400.0}),
  ReferenceValue({I18n::Message::JupiterDayTitle1, I18n::Message::JupiterDayTitle2, I18n::Message::JupiterDaySubtitle, 35600.0}),
  ReferenceValue({I18n::Message::TravelToTheMoonTitle1, I18n::Message::TravelToTheMoonTitle2, I18n::Message::TravelToTheMoonSubtitle, 259000.0}),
  ReferenceValue({I18n::Message::TravelToMarsTitle1, I18n::Message::TravelToMarsTitle2, I18n::Message::TravelToMarsSubtitle, 23700000.0}),
  ReferenceValue({I18n::Message::LifeExpectancyTitle1, I18n::Message::LifeExpectancyTitle2, I18n::Message::LifeExpectancySubtitle, 2210000000.0}),
  ReferenceValue({I18n::Message::WritingAgeTitle1, I18n::Message::WritingAgeTitle2, I18n::Message::WritingAgeSubtitle, 158000000000.0}),
  ReferenceValue({I18n::Message::HomoSapiensAgeTitle1, I18n::Message::HomoSapiensAgeTitle2, I18n::Message::HomoSapiensAgeSubtitle, 9470000000000.0}),
  ReferenceValue({I18n::Message::LucyAgeTitle1, I18n::Message::LucyAgeTitle2, I18n::Message::LucyAgeSubtitle, 101000000000000.0}),
  ReferenceValue({I18n::Message::DinosaursAgeTitle1, I18n::Message::DinosaursAgeTitle2, I18n::Message::DinosaursAgeSubtitle, 2050000000000000.0}),
  ReferenceValue({I18n::Message::UniverseAgeTitle1, I18n::Message::UniverseAgeTitle2, I18n::Message::UniverseAgeSubtitle, 4.42e+17}),
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const ReferenceValue k_velocityReferences[] = {
  ReferenceValue({I18n::Message::SnailSpeedTitle1, I18n::Message::SnailSpeedTitle2, I18n::Message::SnailSpeedSubtitle, 0.01}),
  ReferenceValue({I18n::Message::HumanSpeedTitle1, I18n::Message::HumanSpeedTitle2, I18n::Message::HumanSpeedSubtitle, 2.8}),
  ReferenceValue({I18n::Message::TrainSpeedTitle1, I18n::Message::TrainSpeedTitle2, I18n::Message::TrainSpeedSubtitle, 89.0}),
  ReferenceValue({I18n::Message::SoundSpeedTitle1, I18n::Message::SoundSpeedTitle2, I18n::Message::SoundSpeedSubtitle, 340.0}),
  ReferenceValue({I18n::Message::SatelliteSpeedTitle1, I18n::Message::SatelliteSpeedTitle2, I18n::Message::SatelliteSpeedSubtitle, 7800.0}),
  ReferenceValue({I18n::Message::EarthRevolutionSpeedTitle1, I18n::Message::EarthRevolutionSpeedTitle2, I18n::Message::EarthRevolutionSpeedSubtitle, 30000.0}),
  ReferenceValue({I18n::Message::LightSpeedTitle1, I18n::Message::LightSpeedTitle2, I18n::Message::LightSpeedSubtitle, 300000000.0}),
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const ReferenceValue k_volumeReferences[] = {
  ReferenceValue({I18n::Message::SandGrainVolumeTitle1, I18n::Message::SandGrainVolumeTitle2, I18n::Message::SandGrainVolumeSubtitle, 1.13e-13}),
  ReferenceValue({I18n::Message::SemolinaGrainVolumeTitle1, I18n::Message::SemolinaGrainVolumeTitle2, I18n::Message::SemolinaGrainVolumeSubtitle, 2e-11}),
  ReferenceValue({I18n::Message::PinVolumeTitle1, I18n::Message::PinVolumeTitle2, I18n::Message::PinVolumeSubtitle, 1e-09}),
  ReferenceValue({I18n::Message::WaterDropVolumeTitle1, I18n::Message::WaterDropVolumeTitle2, I18n::Message::WaterDropVolumeSubtitle, 5e-08}),
  ReferenceValue({I18n::Message::EuroCoinVolumeTitle1, I18n::Message::EuroCoinVolumeTitle2, I18n::Message::EuroCoinVolumeSubtitle, 9.89e-07}),
  ReferenceValue({I18n::Message::CapsuleVolumeTitle1, I18n::Message::CapsuleVolumeTitle2, I18n::Message::CapsuleVolumeSubtitle, 1.37e-06}),
  ReferenceValue({I18n::Message::PingPongBallVolumeTitle1, I18n::Message::PingPongBallVolumeTitle2, I18n::Message::PingPongBallVolumeSubtitle, 3.35e-05}),
  ReferenceValue({I18n::Message::CanVolumeTitle1, I18n::Message::CanVolumeTitle2, I18n::Message::CanVolumeSubtitle, 0.00033}),
  ReferenceValue({I18n::Message::BasketballVolumeTitle1, I18n::Message::BasketballVolumeTitle2, I18n::Message::BasketballVolumeSubtitle, 0.00724}),
  ReferenceValue({I18n::Message::BathtubVolumeTitle1, I18n::Message::BathtubVolumeTitle2, I18n::Message::BathtubVolumeSubtitle, 0.15}),
  ReferenceValue({I18n::Message::VanVolumeTitle1, I18n::Message::VanVolumeTitle2, I18n::Message::VanVolumeSubtitle, 8.0}),
  ReferenceValue({I18n::Message::ContainerVolumeTitle1, I18n::Message::ContainerVolumeTitle2, I18n::Message::ContainerVolumeSubtitle, 33.0}),
  ReferenceValue({I18n::Message::EiffelTowerVolumeTitle1, I18n::Message::EiffelTowerVolumeTitle2, I18n::Message::EiffelTowerVolumeSubtitle, 900.0}),
  ReferenceValue({I18n::Message::OlympicPoolVolumeTitle1, I18n::Message::OlympicPoolVolumeTitle2, I18n::Message::OlympicPoolVolumeSubtitle, 2500.0}),
  ReferenceValue({I18n::Message::NotreDameVolumeTitle1, I18n::Message::NotreDameVolumeTitle2, I18n::Message::NotreDameVolumeSubtitle, 100000.0}),
  ReferenceValue({I18n::Message::CheopsPyramidVolumeTitle1, I18n::Message::CheopsPyramidVolumeTitle2, I18n::Message::CheopsPyramidVolumeSubtitle, 2600000.0}),
  ReferenceValue({I18n::Message::LacLemanVolumeTitle1, I18n::Message::LacLemanVolumeTitle2, I18n::Message::LacLemanVolumeSubtitle, 89000000000.0}),
  ReferenceValue({I18n::Message::TheChannelVolumeTitle1, I18n::Message::TheChannelVolumeTitle2, I18n::Message::TheChannelVolumeSubtitle, 4050000000000.0}),
  ReferenceValue({I18n::Message::BlackSeaVolumeTitle1, I18n::Message::BlackSeaVolumeTitle2, I18n::Message::BlackSeaVolumeSubtitle, 547000000000000.0}),
  ReferenceValue({I18n::Message::MediterraneanSeaVolumeTitle1, I18n::Message::MediterraneanSeaVolumeTitle2, I18n::Message::MediterraneanSeaVolumeSubtitle, 4390000000000000.0}),
  ReferenceValue({I18n::Message::TheAtlanticVolumeTitle1, I18n::Message::TheAtlanticVolumeTitle2, I18n::Message::TheAtlanticVolumeSubtitle, 3.06e+17}),
  ReferenceValue({I18n::Message::OceansVolumeTitle1, I18n::Message::OceansVolumeTitle2, I18n::Message::OceansVolumeSubtitle, 1.34e+18}),
  ReferenceValue({I18n::Message::MoonVolumeTitle1, I18n::Message::MoonVolumeTitle2, I18n::Message::MoonVolumeSubtitle, 2.2e+19}),
  ReferenceValue({I18n::Message::EarthVolumeTitle1, I18n::Message::EarthVolumeTitle2, I18n::Message::EarthVolumeSubtitle, 1.08e+21}),
  ReferenceValue({I18n::Message::JupiterVolumeTitle1, I18n::Message::JupiterVolumeTitle2, I18n::Message::JupiterVolumeSubtitle, 1.43e+24}),
  ReferenceValue({I18n::Message::SunVolumeTitle1, I18n::Message::SunVolumeTitle2, I18n::Message::SunVolumeSubtitle, 1.41e+27}),
  ReferenceValue({(I18n::Message)0, (I18n::Message)0, (I18n::Message)0, 0})
};

constexpr static const int k_numberOfReferenceTables = 8;
constexpr static ReferenceUnit k_referenceUnits[] = {
  ReferenceUnit({"_m", "_m"}), // Length
  ReferenceUnit({"_s", "_s"}), // Time
  ReferenceUnit({"_kg", "_kg"}), // Mass
  ReferenceUnit({"_m^2", "_m^2"}), // Area
  ReferenceUnit({"_m^3", "_m^3"}), // Volume
  ReferenceUnit({"_kg×_m^2×_s^\u0012-3\u0013", "_W"}), // Power
  ReferenceUnit({"_m×_s^\u0012-1\u0013", "_m×_s^\u0012-1\u0013"}), // Velocity
  ReferenceUnit({"_kg×_m^\u0012-1\u0013^\u0012-2\u0013", "_Pa"}) // Pressure
};
constexpr static const ReferenceValue * k_referenceTables[] = {k_lengthReferences, k_timeReferences, k_massReferences, k_areaReferences, k_volumeReferences, k_powerReferences, k_velocityReferences, k_pressureReferences};

static_assert(sizeof(k_referenceUnits) / sizeof(ReferenceUnit) == k_numberOfReferenceTables, "Wrong number of reference tables or missing reference unit");
static_assert(sizeof(k_referenceTables) / sizeof(ReferenceValue *) == k_numberOfReferenceTables, "Wrong number of reference tables or missing reference table");

int FindUpperAndLowerReferenceValues(double inputValue, Expression unit, const ReferenceValue ** referenceValues, int * returnReferenceTableIndex) {
  // 1. Find table of corresponding unit.
  const ReferenceValue * referenceTable = nullptr;
  char unitBuffer[k_sizeOfUnitBuffer];
  PoincareHelpers::Serialize(unit, unitBuffer, k_sizeOfUnitBuffer);
  int referenceTableIndex = 0;
  while (referenceTableIndex < k_numberOfReferenceTables) {
    if (strncmp(unitBuffer, k_referenceUnits[referenceTableIndex].SIUnit, k_sizeOfUnitBuffer) == 0) {
      referenceTable = k_referenceTables[referenceTableIndex];
      break;
    }
    referenceTableIndex++;
  }
  if (referenceTable == nullptr) {
    return 0;
  }


  // 2. Iterate through table to find upper and lower values indexes
  int referenceIndex = 0;
  int upperIndex = -1;
  while(referenceTable[referenceIndex].value != 0) {
    if (referenceTable[referenceIndex].value >= inputValue) {
      upperIndex = referenceIndex;
      break;
    }
    referenceIndex ++;
  }
  int lowerIndex = upperIndex < 0 ? referenceIndex - 1 : upperIndex - 1;

  // 3. Find ratios and save them if needed
  int indexes[] = {lowerIndex, upperIndex};
  double ratios[] = {0.0, 0.0};
  int numberOfReferenceFound = 0;
  for (int i = 0; i < 2; i++) {
    if (indexes[i] != -1) {
      ratios[i] = inputValue / referenceTable[indexes[i]].value;
      if (ratios[i] < 100.0 && ratios[i] >= 0.01) {
        if (referenceValues != nullptr) {
          referenceValues[numberOfReferenceFound] = &referenceTable[indexes[i]];
        }
        numberOfReferenceFound++;
      }
    }
  }

  if (returnReferenceTableIndex != nullptr) {
    *returnReferenceTableIndex = referenceTableIndex;
  }

  return numberOfReferenceFound;
}

void FillRatioBuffer(double ratio, char * textBuffer, int bufferSize) {
  assert(bufferSize <= k_sizeOfUnitComparisonBuffer);
  assert(ratio < 100.0 && ratio >= 0.01);
  int bufferIndex = 0;
  bool withPercentage = false;
  if (ratio <= 1.0) {
    ratio = 100.0*ratio;
    withPercentage = true;
  }
  bufferIndex = PoincareHelpers::ConvertFloatToText<double>(ratio, textBuffer, bufferSize - 1, ratio >= 99.5 ? k_numberOfSignicativeDigits + 1 : k_numberOfSignicativeDigits); // Ternary operator handles the 100% case
  if (withPercentage) {
    textBuffer[bufferIndex] = '%';
    bufferIndex++;
    textBuffer[bufferIndex] = 0;
  }
}

Expression BuildComparisonExpression(double value, const ReferenceValue * referenceValue, int unitIndex) {
  assert(unitIndex < k_numberOfReferenceTables);
  double ratio = value / referenceValue->value;
  Expression unit = Poincare::Expression::Parse(k_referenceUnits[unitIndex].displayedUnit, App::app()->localContext());
  return Multiplication::Builder(Float<double>::Builder(ratio), Float<double>::Builder(referenceValue->value), unit);
}

}

}
