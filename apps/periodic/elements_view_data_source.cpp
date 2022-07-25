#include "elements_view_data_source.h"
#include "app.h"
#include <poincare/print_int.h>
#include <assert.h>

namespace Periodic {

ElementsViewDataSource::ElementsViewDataSource(ElementsViewDelegate * delegate) :
  m_delegate(delegate),
  m_textFilter(nullptr),
  m_searchResult(ElementsDataBase::k_noElement)
{}

AtomicNumber ElementsViewDataSource::selectedElement() const {
  return App::app()->snapshot()->selectedElement();
}

AtomicNumber ElementsViewDataSource::previousElement() const {
  return App::app()->snapshot()->previousElement();
}

void ElementsViewDataSource::setSelectedElement(AtomicNumber z) {
  if (z != selectedElement()) {
    assert(m_delegate);
    App::Snapshot * snapshot = App::app()->snapshot();
    snapshot->setPreviousElement(selectedElement());
    snapshot->setSelectedElement(z);
    m_delegate->selectedElementHasChanged();
  }
}

const DataField * ElementsViewDataSource::field() const {
  return App::app()->snapshot()->field();
}

void ElementsViewDataSource::setField(const DataField * dataField) {
  if (dataField != field()) {
    App::app()->snapshot()->setField(dataField);
    m_delegate->activeDataFieldHasChanged();
  }
}

DataField::ColorPair ElementsViewDataSource::filteredColors(AtomicNumber z) const {
  return elementMatchesFilter(z) ? field()->getColors(z) : DataField::ColorPair(KDColorBlack, Palette::SystemGrayDark);
}

const char * ElementsViewDataSource::suggestedElementName() {
  m_searchResult = ElementsDataBase::k_noElement;
  for (AtomicNumber z = 1; z <= ElementsDataBase::k_numberOfElements; z++) {
    if (elementNameMatchesFilter(z)) {
      m_searchResult = z;
      return I18n::translate(ElementsDataBase::Name(z));
    }
    if ((elementSymbolMatchesFilter(z) || (elementNumberMatchesFilter(z))) && !ElementsDataBase::IsElement(m_searchResult)) {
      m_searchResult = z;
    }
  }
  return nullptr;
}

const char * ElementsViewDataSource::cycleSuggestion(bool goingDown) {
  if (!(ElementsDataBase::IsElement(m_searchResult) && elementNameMatchesFilter(m_searchResult))) {
    return nullptr;
  }
  for (int i = 1; i < ElementsDataBase::k_numberOfElements; i++) {
    AtomicNumber z = (m_searchResult + (goingDown ? i : -i) - 1 + ElementsDataBase::k_numberOfElements) % ElementsDataBase::k_numberOfElements + 1;
    if (elementNameMatchesFilter(z)) {
      m_searchResult = z;
      break;
    }
  }
  return I18n::translate(ElementsDataBase::Name(m_searchResult));
}

bool ElementsViewDataSource::elementMatchesFilter(AtomicNumber z) const {
  return !m_textFilter
      || elementSymbolMatchesFilter(z)
      || elementNameMatchesFilter(z)
      || elementNumberMatchesFilter(z);
}

bool ElementsViewDataSource::elementNameMatchesFilter(AtomicNumber z) const {
  assert(m_textFilter);
  return UTF8Helper::IsPrefixCaseInsensitiveNoCombining(m_textFilter, I18n::translate(ElementsDataBase::Name(z)));
}

bool ElementsViewDataSource::elementSymbolMatchesFilter(AtomicNumber z) const {
  assert(m_textFilter);
  return UTF8Helper::IsPrefixCaseInsensitiveNoCombining(m_textFilter, ElementsDataBase::Symbol(z));
}

bool ElementsViewDataSource::elementNumberMatchesFilter(AtomicNumber z) const {
  assert(m_textFilter);
  constexpr size_t k_maxZSize = 4;
  char zBuffer[k_maxZSize];
  int zLength = Poincare::PrintInt::Left(z, zBuffer, k_maxZSize);
  zBuffer[zLength] = '\0';
  return UTF8Helper::IsPrefixCaseInsensitiveNoCombining(m_textFilter, zBuffer);
}

}
