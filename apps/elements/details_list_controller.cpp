#include "details_list_controller.h"
#include "app.h"
#include <escher/clipboard.h>

using namespace Escher;
using namespace Poincare;

namespace Elements {

DetailsListController::DetailsListController(StackViewController * parentResponder) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this, &m_view),
  m_topElementView(Escher::Palette::WallScreen),
  m_bottomMessageView(KDFont::Size::Small, I18n::Message::ElementsDataConditions, KDContext::k_alignCenter, KDContext::k_alignCenter, Escher::Palette::GrayDark, Escher::Palette::WallScreen),
  m_view(&m_selectableTableView, this, &m_topElementView, &m_bottomMessageView)
{}

bool DetailsListController::handleEvent(Ion::Events::Event e) {
  if (e == Ion::Events::Plus || e == Ion::Events::Minus) {
    int step = e == Ion::Events::Plus ? 1 : -1;
    ElementsViewDataSource * dataSource = App::app()->elementsViewDataSource();
    /* Add an extra ElementsDataBase::k_numberOfElements to work around the %
     * operator not behvaing correctly with negative integers. */
    AtomicNumber newZ = (dataSource->selectedElement() + step + ElementsDataBase::k_numberOfElements - 1) % ElementsDataBase::k_numberOfElements + 1;
    dataSource->setSelectedElement(newZ);
    /* Pop and push back to update the title. */
    StackViewController * stack = stackViewController();
    stack->pop();
    stack->push(this);
    return true;
  }

  if (e == Ion::Events::Copy) {
    constexpr size_t size = Escher::Clipboard::k_bufferSize;
    char buffer[size];
    int index = selectedRow();
    Layout l = DataFieldForRow(index)->getLayout(App::app()->elementsViewDataSource()->selectedElement(), PrintFloat::k_numberOfStoredSignificantDigits);
    int length = l.serializeForParsing(buffer, size);
    assert(length < static_cast<int>(size));
    (void)length;
    Escher::Clipboard::sharedClipboard()->store(buffer);
    return true;
  }

  return false;
}

void DetailsListController::didBecomeFirstResponder() {
  m_view.reload();
}

const char * DetailsListController::title() {
  return I18n::translate(ElementsDataBase::Name(App::app()->elementsViewDataSource()->selectedElement()));
}

HighlightCell * DetailsListController::reusableCell(int index, int type) {
  if (type == k_separatorCellType) {
    assert(index < static_cast<int>(k_numberOfSeparatorCells));
    return m_separatorCells + index;
  }
  assert(type == k_normalCellType && index < static_cast<int>(k_numberOfNormalCells));
  return m_normalCells + index;

}

int DetailsListController::reusableCellCount(int type) {
  if (type == k_separatorCellType) {
    return k_numberOfSeparatorCells;
  }
  assert(type == k_normalCellType);
  return k_numberOfNormalCells;
}

int DetailsListController::typeAtIndex(int index) {
  assert(index < numberOfRows());
  const DataField * dataField = DataFieldForRow(index);
  if (dataField == &ElementsDataBase::ConfigurationField
   || dataField == &ElementsDataBase::GroupField
   || dataField == &ElementsDataBase::RadiusField
   || dataField == &ElementsDataBase::StateField
   || dataField == &ElementsDataBase::AffinityField)
  {
    return k_separatorCellType;
  }
  return k_normalCellType;
}

void DetailsListController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  AtomicNumber z = App::app()->elementsViewDataSource()->selectedElement();
  assert(ElementsDataBase::IsElement(z));
  PhysicalQuantityCell * typedCell = typeAtIndex(index) == k_separatorCellType ? static_cast<PhysicalQuantityCellWithSeparator *>(cell)->innerCell() : static_cast<PhysicalQuantityCell *>(cell);
  const DataField * dataField = DataFieldForRow(index);

  I18n::Message sublabel = I18n::Message::Default;
  if (dataField != &ElementsDataBase::GroupField) {
    /* The "Group of element" full legend is used in the display selector, but
     * not in the details card. */
    sublabel = dataField->fieldLegend();
  }

  typedCell->setSubLabelMessage(sublabel);
  int significantDigits = Preferences::sharedPreferences()->numberOfSignificantDigits();
  typedCell->setLayouts(dataField->fieldSymbolLayout(), dataField->getLayout(z, significantDigits));
}

KDCoordinate DetailsListController::nonMemoizedRowHeight(int j) {
  int type = typeAtIndex(j);
  if (type == k_separatorCellType) {
    PhysicalQuantityCellWithSeparator tempCell;
    return heightForCellAtIndexWithWidthInit(&tempCell, j);
  }
  assert(type == k_normalCellType);
  PhysicalQuantityCell tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, j);
}

const DataField * DetailsListController::DataFieldForRow(int row) {
  constexpr const DataField * k_fields[k_numberOfRows] = {
    &ElementsDataBase::ZField,
    &ElementsDataBase::AField,
    &ElementsDataBase::MassField,
    &ElementsDataBase::ConfigurationField,
    &ElementsDataBase::ElectronegativityField,
    &ElementsDataBase::GroupField,
    &ElementsDataBase::RadiusField,
    &ElementsDataBase::StateField,
    &ElementsDataBase::MeltingPointField,
    &ElementsDataBase::BoilingPointField,
    &ElementsDataBase::DensityField,
    &ElementsDataBase::AffinityField,
    &ElementsDataBase::IonizationField,
  };
  assert(row < static_cast<int>(k_numberOfRows));
  return k_fields[row];
}

}
