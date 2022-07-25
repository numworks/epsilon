#ifndef PERIODIC_ELEMENTS_DATA_BASE
#define PERIODIC_ELEMENTS_DATA_BASE

#include "data_field.h"
#include "element_data.h"
#include <poincare/layout.h>

namespace Periodic {

class ElementsDataBase {
public:
  /* Available data fields */
  constexpr static ZDataField ZField = ZDataField();
  constexpr static ADataField AField = ADataField();
  constexpr static ConfigurationDataField ConfigurationField = ConfigurationDataField();
  constexpr static GroupDataField GroupField = GroupDataField();
  constexpr static BlockDataField BlockField = BlockDataField();
  constexpr static MetalDataField MetalField = MetalDataField();
  constexpr static StateDataField StateField = StateDataField();
  constexpr static MassDataField MassField = MassDataField();
  constexpr static ElectronegativityDataField ElectronegativityField = ElectronegativityDataField();
  constexpr static RadiusDataField RadiusField = RadiusDataField();
  constexpr static MeltingPointDataField MeltingPointField = MeltingPointDataField();
  constexpr static BoilingPointDataField BoilingPointField = BoilingPointDataField();
  constexpr static DensityDataField DensityField = DensityDataField();
  constexpr static AffinityDataField AffinityField = AffinityDataField();
  constexpr static IonizationDataField IonizationField = IonizationDataField();

  constexpr static uint8_t k_numberOfElements = 118;
  constexpr static AtomicNumber k_noElement = 0;

  static bool IsElement(AtomicNumber z) { return z > k_noElement && z <= k_numberOfElements; }
  static bool IsMetal(AtomicNumber z) { return Group(z) < ElementData::Group::Metalloid; }

  static double MolarMass(AtomicNumber z) { return DataForElement(z)->molarMass(); }
  static double Electronegativity(AtomicNumber z) { return DataForElement(z)->electronegativity(); }
  static double Radius(AtomicNumber z) { return DataForElement(z)->radius(); }
  static double MeltingPoint(AtomicNumber z) { return DataForElement(z)->meltingPoint(); }
  static double BoilingPoint(AtomicNumber z) { return DataForElement(z)->boilingPoint(); }
  static double Density(AtomicNumber z) { return DataForElement(z)->density(); }
  static double Affinity(AtomicNumber z) { return DataForElement(z)->affinity(); }
  static double EnergyOfIonization(AtomicNumber z) { return DataForElement(z)->energyOfIonization(); }
  static const char * Symbol(AtomicNumber z) { return DataForElement(z)->symbol(); }
  static I18n::Message Name(AtomicNumber z) { return DataForElement(z)->name(); }
  static uint16_t NumberOfMass(AtomicNumber z) { return DataForElement(z)->numberOfMass(); }
  static ElementData::Group Group(AtomicNumber z) { return DataForElement(z)->group(); }
  static ElementData::PhysicalState PhysicalState(AtomicNumber z) { return DataForElement(z)->physicalState(); }
  static ElementData::Block Block(AtomicNumber z) { return DataForElement(z)->block(); }

  static const char * MolarMassUnit() { return "g/mol"; }
  static const char * ElectronegativityUnit() { return ""; }
  static const char * TemperatureUnit() { return "°C"; }
  static const char * RadiusUnit() { return "pm"; }
  static const char * DensityUnit() { return "g/cm^3"; }
  static const char * EnergyUnit() { return "kJ/mol"; }

private:
  static const ElementData * DataForElement(AtomicNumber z);
};

}

#endif
