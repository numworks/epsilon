#ifndef PERIODIC_ELEMENTS_VIEW_DATA_SOURCE_H
#define PERIODIC_ELEMENTS_VIEW_DATA_SOURCE_H

#include "elements_data_base.h"
#include "elements_view_delegate.h"
#include "palette.h"

namespace Periodic {

class ElementsViewDataSource {
public:
  ElementsViewDataSource() : m_delegate(nullptr), m_field(&ElementsDataBase::GroupField), m_textFilter(nullptr), m_selectedElement(1), m_previousElement(ElementsDataBase::k_noElement), m_searchResult(ElementsDataBase::k_noElement) {}

  void setDelegate(ElementsViewDelegate * delegate) { m_delegate = delegate; }
  AtomicNumber selectedElement() const { return m_selectedElement; }
  AtomicNumber previousElement() const { return m_previousElement; }
  void setSelectedElement(AtomicNumber z);
  const DataField * field() const { return m_field; }
  void setField(const DataField * field);
  void setTextFilter(const char * filter) { m_textFilter = filter; }
  /* Returns colors given by the data field, or default colors if the element
   * does not match the filter. */
  DataField::ColorPair filteredColors(AtomicNumber z) const;
  AtomicNumber elementSearchResult() const { return m_searchResult; }
  const char * suggestedElementName();
  const char * cycleSuggestion(bool goingDown);

private:
  bool elementMatchesFilter(AtomicNumber z) const;
  bool elementNameMatchesFilter(AtomicNumber z) const;
  bool elementSymbolMatchesFilter(AtomicNumber z) const;
  bool elementNumberMatchesFilter(AtomicNumber z) const;

  ElementsViewDelegate * m_delegate;
  const DataField * m_field;
  const char * m_textFilter;
  AtomicNumber m_selectedElement;
  AtomicNumber m_previousElement;
  AtomicNumber m_searchResult;
};

}

#endif
