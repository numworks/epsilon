#ifndef ELEMENTS_ELEMENTS_VIEW_DATA_SOURCE_H
#define ELEMENTS_ELEMENTS_VIEW_DATA_SOURCE_H

#include "elements_data_base.h"
#include "elements_view_delegate.h"
#include "palette.h"

namespace Elements {

class ElementsViewDataSource {
 public:
  ElementsViewDataSource(ElementsViewDelegate* delegate);

  AtomicNumber selectedElement() const;
  AtomicNumber previousElement() const;
  void setSelectedElement(AtomicNumber z);
  const DataField* field() const;
  void setField(const DataField* dataField);
  void setTextFilter(const char* filter) { m_textFilter = filter; }
  /* Returns colors given by the data field, or default colors if the element
   * does not match the filter. */
  DataField::ColorPair filteredColors(AtomicNumber z) const;
  AtomicNumber elementSearchResult() const;
  const char* suggestedElementName();
  const char* cycleSuggestion(bool goingDown);

 private:
  typedef bool (ElementsViewDataSource::*ElementTest)(AtomicNumber) const;

  bool elementMatchesFilter(AtomicNumber z) const;
  bool elementNameMatchesFilter(AtomicNumber z) const;
  bool elementSymbolMatchesFilter(AtomicNumber z) const;
  bool elementNumberMatchesFilter(AtomicNumber z) const;
  AtomicNumber privateElementSearch(ElementTest test) const;

  ElementsViewDelegate* m_delegate;
  const char* m_textFilter;
  AtomicNumber m_suggestedElement;
};

}  // namespace Elements

#endif
