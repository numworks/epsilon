#include "details_list_controller.h"

#include <escher/clipboard.h>

#include "app.h"

using namespace Escher;
using namespace Poincare;

namespace Elements {

DetailsListController::DetailsListController(
    StackViewController* parentResponder)
    : ListWithTopAndBottomController(parentResponder, &m_topElementView,
                                     &m_bottomMessageView),
      m_topElementView(Escher::Palette::WallScreen),
      m_bottomMessageView(I18n::Message::ElementsDataConditions,
                          k_messageFormat) {}

bool DetailsListController::handleEvent(Ion::Events::Event e) {
  /* Navigate through elements, sorted by their atomic number.
   * Press the minus or left key to reach lighter element, plus or right to
   * reach a heavier one. */

  // -1: left. 1: right
  int step;
  if (e == Ion::Events::Minus || e == Ion::Events::Left) {
    step = -1;
  } else if (e == Ion::Events::Plus || e == Ion::Events::Right) {
    step = 1;
  } else {
    return false;
  }

  ElementsViewDataSource* dataSource = App::app()->elementsViewDataSource();
  /* Add an extra ElementsDataBase::k_numberOfElements to work around the %
   * operator not behvaing correctly with negative integers. */
  AtomicNumber newZ = (dataSource->selectedElement() + step +
                       ElementsDataBase::k_numberOfElements - 1) %
                          ElementsDataBase::k_numberOfElements +
                      1;
  dataSource->setSelectedElement(newZ);
  /* Pop and push back to update the title. */
  KDPoint offset = m_selectableListView.contentOffset();
  int row = m_selectableListView.selectedRow();
  StackViewController* stack = stackViewController();
  stack->pop();
  stack->push(this);
  m_selectableListView.setContentOffset(offset);
  m_selectableListView.selectCell(row);
  return true;
}

const char* DetailsListController::title() const {
  return I18n::translate(ElementsDataBase::Name(
      App::app()->elementsViewDataSource()->selectedElement()));
}

KDCoordinate DetailsListController::separatorBeforeRow(int row) {
  assert(row < numberOfRows());
  const DataField* dataField = DataFieldForRow(row);
  if (dataField == &ElementsDataBase::ConfigurationField ||
      dataField == &ElementsDataBase::GroupField ||
      dataField == &ElementsDataBase::RadiusField ||
      dataField == &ElementsDataBase::StateField ||
      dataField == &ElementsDataBase::AffinityField) {
    return k_defaultRowSeparator;
  }
  return 0;
}

void DetailsListController::fillCellForRow(HighlightCell* cell, int row) {
  AtomicNumber z = App::app()->elementsViewDataSource()->selectedElement();
  assert(ElementsDataBase::IsElement(z));
  PhysicalQuantityCell* typedCell = static_cast<PhysicalQuantityCell*>(cell);
  const DataField* dataField = DataFieldForRow(row);

  I18n::Message sublabel = I18n::Message::Default;
  if (dataField != &ElementsDataBase::GroupField) {
    /* The "Group of element" full legend is used in the display selector, but
     * not in the details card. */
    sublabel = dataField->fieldLegend();
  }

  typedCell->subLabel()->setMessage(sublabel);
  int significantDigits =
      Preferences::SharedPreferences()->numberOfSignificantDigits();
  typedCell->label()->setLayout(dataField->fieldSymbolLayout());
  typedCell->accessory()->setLayout(dataField->getLayout(z, significantDigits));
}

bool DetailsListController::canStoreCellAtRow(int row) {
  return DataFieldForRow(row)->canBeStored(
      App::app()->elementsViewDataSource()->selectedElement());
}

KDCoordinate DetailsListController::nonMemoizedRowHeight(int row) {
  PhysicalQuantityCell tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, row);
}

const DataField* DetailsListController::DataFieldForRow(int row) {
  constexpr const DataField* k_fields[k_numberOfRows] = {
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

}  // namespace Elements
