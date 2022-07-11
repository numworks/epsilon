#ifndef PERIODIC_ELEMENTS_VIEW_DATA_SOURCE_H
#define PERIODIC_ELEMENTS_VIEW_DATA_SOURCE_H

#include "display_type.h"
#include "elements_data_base.h"
#include "elements_view_delegate.h"
#include "palette.h"

namespace Periodic {

class ElementsViewDataSource {
public:
  constexpr static size_t k_numberOfDisplayTypes = 8;
  constexpr static GroupsDisplayType ColorByGroups;
  constexpr static BlocksDisplayType ColorByBlocks;
  constexpr static MetalsDisplayType ColorByMetals;
  constexpr static StatesDisplayType ColorByStates;
  constexpr static MassDisplayType ColorByMass;
  constexpr static ElectronegativityDisplayType ColorByElectronegativity;
  constexpr static MeltingPointDisplayType ColorByMeltingPoint;
  constexpr static BoilingPointDisplayType ColorByBoilingPoint;
  constexpr static RadiusDisplayType ColorByRadius;
  constexpr static DensityDisplayType ColorByDensity;
  constexpr static AffinityDisplayType ColorByAffinity;
  constexpr static IonisationDisplayType ColorByIonisation;

  ElementsViewDataSource() : m_delegate(nullptr), m_displayType(&ColorByGroups), m_textFilter(nullptr), m_selectedElement(1) {}

  void setDelegate(ElementsViewDelegate * delegate) { m_delegate = delegate; }
  AtomicNumber selectedElement() const { return m_selectedElement; }
  void setSelectedElement(AtomicNumber z);
  const DisplayType * displayType() const { return m_displayType; }
  void setDisplayType(const DisplayType * displayType) { m_displayType = displayType; }
  void setTextFilter(const char * filter) { m_textFilter = filter; }

private:
  ElementsViewDelegate * m_delegate;
  const DisplayType * m_displayType;
  const char * m_textFilter;
  AtomicNumber m_selectedElement;
};

}

#endif
