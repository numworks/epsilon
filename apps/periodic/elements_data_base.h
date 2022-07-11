#ifndef PERIODIC_ELEMENTS_DATA_BASE
#define PERIODIC_ELEMENTS_DATA_BASE

#include "element_data.h"
#include <stddef.h>

namespace Periodic {

/* Methods taking an AtomicNumber expect a number from 1 to 118 (i.e. 1-based
 * numbering). */
typedef uint8_t AtomicNumber;

class ElementsDataBase {
public:
  constexpr static uint8_t k_numberOfElements = 118;
  constexpr static AtomicNumber k_noElement = 0;

  static bool IsElement(AtomicNumber z) { return z > k_noElement && z <= k_numberOfElements; }

  static double MolarMass(AtomicNumber z) { return DataForElement(z)->molarMass(); }
  static double Electronegativity(AtomicNumber z) { return DataForElement(z)->electronegativity(); }
  static double Radius(AtomicNumber z) { return DataForElement(z)->radius(); }
  static double MeltingPoint(AtomicNumber z) { return DataForElement(z)->meltingPoint(); }
  static double BoilingPoint(AtomicNumber z) { return DataForElement(z)->boilingPoint(); }
  static double Density(AtomicNumber z) { return DataForElement(z)->density(); }
  static double Affinity(AtomicNumber z) { return DataForElement(z)->affinity(); }
  static double EnergyOfIonisation(AtomicNumber z) { return DataForElement(z)->energyOfIonisation(); }
  static const char * Symbol(AtomicNumber z) { return DataForElement(z)->symbol(); }
  static I18n::Message Name(AtomicNumber z) { return DataForElement(z)->name(); }
  static uint16_t NumberOfMass(AtomicNumber z) { return DataForElement(z)->numberOfMass(); }
  static ElementData::Group Group(AtomicNumber z) { return DataForElement(z)->group(); }
  static ElementData::PhysicalState PhysicalState(AtomicNumber z) { return DataForElement(z)->physicalState(); }
  static ElementData::Block Block(AtomicNumber z) { return DataForElement(z)->block(); }
  static size_t ElectronConfiguration(AtomicNumber z, char * buffer, size_t bufferSize);

  static const char * MolarMassUnit() { return "g/mol"; }
  static const char * ElectronegativityUnit() { return ""; }
  static const char * TemperatureUnit() { return "°C"; }
  static const char * RadiusUnit() { return "pm"; }
  static const char * DensityUnit() { return "kg/m^3"; }
  static const char * EnergyUnit() { return "kJ/mol"; }

private:
  static const ElementData * DataForElement(AtomicNumber z);
};

}

#endif
