#ifndef PERIODIC_ELEMENTS_VIEW_DATA_SOURCE_H
#define PERIODIC_ELEMENTS_VIEW_DATA_SOURCE_H

#include "coloring.h"
#include "elements_data_base.h"
#include "elements_view_delegate.h"
#include "palette.h"

namespace Periodic {

class ElementsViewDataSource {
public:
  constexpr static size_t k_numberOfColorings = 8;
  constexpr static GroupsColoring ColorByGroups;
  constexpr static BlocksColoring ColorByBlocks;
  constexpr static MetalsColoring ColorByMetals;
  constexpr static MassColoring ColorByMass;
  constexpr static ElectronegativityColoring ColorByElectronegativity;
  constexpr static MeltingPointColoring ColorByMeltingPoint;
  constexpr static BoilingPointColoring ColorByBoilingPoint;
  constexpr static RadiusColoring ColorByRadius;

  ElementsViewDataSource() : m_delegate(nullptr), m_coloring(&ColorByGroups), m_textFilter(nullptr), m_selectedElement(1) {}

  void setDelegate(ElementsViewDelegate * delegate) { m_delegate = delegate; }
  AtomicNumber selectedElement() const { return m_selectedElement; }
  void setSelectedElement(AtomicNumber z);
  const Coloring * coloring() const { return m_coloring; }
  void setColoring(const Coloring * coloring) { m_coloring = coloring; }
  void setTextFilter(const char * filter) { m_textFilter = filter; }

private:
  ElementsViewDelegate * m_delegate;
  const Coloring * m_coloring;
  const char * m_textFilter;
  AtomicNumber m_selectedElement;
};

}

#endif
