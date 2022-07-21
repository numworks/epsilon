#include "elements_view_data_source.h"
#include <poincare/print_int.h>
#include <assert.h>

namespace Periodic {

void ElementsViewDataSource::setSelectedElement(AtomicNumber z) {
  if (z != m_selectedElement) {
    assert(m_delegate);
    m_previousElement = m_selectedElement;
    m_selectedElement = z;
    m_delegate->selectedElementHasChanged();
  }
}

void ElementsViewDataSource::setField(const DataField * field) {
  if (field != m_field) {
    m_field = field;
    m_delegate->activeDataFieldHasChanged();
  }
}

DataField::ColorPair ElementsViewDataSource::filteredColors(AtomicNumber z) const {
  return elementMatchesFilter(z) ? m_field->getColors(z) : DataField::ColorPair(KDColorBlack, Palette::SystemGrayDark);
}

AtomicNumber ElementsViewDataSource::firstMatchingElement() const {
  for (AtomicNumber z = 1; z <= ElementsDataBase::k_numberOfElements; z++) {
    if (elementMatchesFilter(z)) {
      return z;
    }
  }
  return 1;
}

bool ElementsViewDataSource::elementMatchesFilter(AtomicNumber z) const {
  if (!m_textFilter
   || UTF8Helper::IsPrefixCaseInsensitiveNoCombining(m_textFilter, I18n::translate(ElementsDataBase::Name(z)))
   || UTF8Helper::IsPrefixCaseInsensitiveNoCombining(m_textFilter, ElementsDataBase::Symbol(z)))
  {
    return true;
  }

  constexpr size_t k_maxZSize = 4;
  char zBuffer[k_maxZSize];
  int zLength = Poincare::PrintInt::Left(z, zBuffer, k_maxZSize);
  zBuffer[zLength] = '\0';
  return UTF8Helper::IsPrefixCaseInsensitiveNoCombining(m_textFilter, zBuffer);
}

}
