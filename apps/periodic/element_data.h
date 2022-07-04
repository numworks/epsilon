#ifndef PERIODIC_ELEMENT_DATA
#define PERIODIC_ELEMENT_DATA

#include <apps/i18n.h>
#include <stdint.h>

namespace Periodic {

class ElementData {
public:
  enum class Group : uint8_t {
    Alkali,
    AlkalineEarth,
    TransitionMetal,
    PostTransitionMetal,
    Lanthanide,
    Actinide,
    /* Groups above Metalloid are metals. */
    Metalloid,
    Nonmetal,
    Halogen,
    NobleGas,
    Unidentified,
  };

  /* We do not store the lengthy electron configuration since it can be easily
   * computed from the atomic number, but the block is stored to save time when
   * filtering. */
  enum class Block : uint8_t { S, P, D, F };

  enum class PhysicalState : uint8_t { Solid, Liquid, Gas };

  constexpr ElementData(
    const char * symbol,
    I18n::Message name,
    uint16_t numberOfMass,
    double molarMass,
    double electronegativity,
    double radius,
    double meltingPoint,
    double boilingPoint,
    double density,
    double affinity,
    double energyOfIonisation,
    Group group,
    PhysicalState physicalState,
    Block block
  ) :
    m_molarMass(molarMass),
    m_electronegativity(electronegativity),
    m_radius(radius),
    m_meltingPoint(meltingPoint),
    m_boilingPoint(boilingPoint),
    m_density(density),
    m_affinity(affinity),
    m_energyOfIonisation(energyOfIonisation),
    m_symbol(symbol),
    m_name(name),
    m_numberOfMass(numberOfMass),
    m_group(group),
    m_physicalState(physicalState),
    m_block(block)
  {}

  double molarMass() const { return m_molarMass; }
  double electronegativity() const { return m_electronegativity; }
  double radius() const { return m_radius; }
  double meltingPoint() const { return m_meltingPoint; }
  double boilingPoint() const { return m_boilingPoint; }
  double density() const { return m_density; }
  double affinity() const { return m_affinity; }
  double energyOfIonisation() const { return m_energyOfIonisation; }
  const char * symbol() const { return m_symbol; }
  I18n::Message name() const { return m_name; }
  uint16_t numberOfMass() const { return m_numberOfMass; }
  Group group() const { return m_group; }
  PhysicalState physicalState() const { return m_physicalState; }
  Block block() const { return m_block; }

private:
  double m_molarMass;
  double m_electronegativity;
  double m_radius;
  double m_meltingPoint;
  double m_boilingPoint;
  double m_density;
  double m_affinity;
  double m_energyOfIonisation;
  const char * m_symbol;
  I18n::Message m_name;
  uint16_t m_numberOfMass;
  Group m_group;
  PhysicalState m_physicalState;
  Block m_block;
};

}

#endif
