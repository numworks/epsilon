#include "details_list_controller.h"

#include <escher/clipboard.h>

#include "app.h"

using namespace Escher;
using namespace Poincare;

namespace Elements {

DetailsListController::DetailsListController(
    StackViewController *parentResponder)
    : ViewController(parentResponder),
      m_selectableListView(this, this, this),
      m_topElementView(Escher::Palette::WallScreen),
      m_bottomMessageView(
          I18n::Message::ElementsDataConditions,
          {.style = {.glyphColor = Escher::Palette::GrayDark,
                     .backgroundColor = Escher::Palette::WallScreen,
                     .font = KDFont::Size::Small},
           .horizontalAlignment = KDGlyph::k_alignCenter}),
      m_view(&m_selectableListView, this, &m_topElementView,
             &m_bottomMessageView) {}

bool DetailsListController::handleEvent(Ion::Events::Event e) {
  if (e == Ion::Events::Plus || e == Ion::Events::Minus) {
    int step = e == Ion::Events::Plus ? 1 : -1;
    ElementsViewDataSource *dataSource = App::app()->elementsViewDataSource();
    /* Add an extra ElementsDataBase::k_numberOfElements to work around the %
     * operator not behvaing correctly with negative integers. */
    AtomicNumber newZ = (dataSource->selectedElement() + step +
                         ElementsDataBase::k_numberOfElements - 1) %
                            ElementsDataBase::k_numberOfElements +
                        1;
    dataSource->setSelectedElement(newZ);
    /* Pop and push back to update the title. */
    StackViewController *stack = stackViewController();
    stack->pop();
    stack->push(this);
    return true;
  }

  if (e == Ion::Events::Copy || e == Ion::Events::Cut ||
      e == Ion::Events::Sto || e == Ion::Events::Var) {
    constexpr size_t size = Escher::Clipboard::k_bufferSize;
    char buffer[size];
    int index = selectedRow();
    const DataField *dataField = DataFieldForRow(index);
    Layout l = DataFieldForRow(index)->getLayout(
        App::app()->elementsViewDataSource()->selectedElement(),
        PrintFloat::k_numberOfStoredSignificantDigits);
    int length;
    if (l.isIdenticalTo(DataField::UnknownValueLayout()) ||
        dataField == &ElementsDataBase::ConfigurationField ||
        dataField == &ElementsDataBase::GroupField ||
        dataField == &ElementsDataBase::StateField) {
      length = strlcpy(buffer, "", size);
    } else {
      length = l.serializeForParsing(buffer, size);
    }
    assert(length < static_cast<int>(size));
    (void)length;
    if (e == Ion::Events::Sto || e == Ion::Events::Var) {
      App::app()->storeValue(buffer);
    } else {
      Escher::Clipboard::SharedClipboard()->store(buffer);
    }
    return true;
  }

  return false;
}

void DetailsListController::didBecomeFirstResponder() { m_view.reload(); }

const char *DetailsListController::title() {
  return I18n::translate(ElementsDataBase::Name(
      App::app()->elementsViewDataSource()->selectedElement()));
}

KDCoordinate DetailsListController::separatorBeforeRow(int index) {
  assert(index < numberOfRows());
  const DataField *dataField = DataFieldForRow(index);
  if (dataField == &ElementsDataBase::ConfigurationField ||
      dataField == &ElementsDataBase::GroupField ||
      dataField == &ElementsDataBase::RadiusField ||
      dataField == &ElementsDataBase::StateField ||
      dataField == &ElementsDataBase::AffinityField) {
    return k_defaultRowSeparator;
  }
  return 0;
}

void DetailsListController::willDisplayCellForIndex(HighlightCell *cell,
                                                    int index) {
  AtomicNumber z = App::app()->elementsViewDataSource()->selectedElement();
  assert(ElementsDataBase::IsElement(z));
  PhysicalQuantityCell *typedCell = static_cast<PhysicalQuantityCell *>(cell);
  const DataField *dataField = DataFieldForRow(index);

  I18n::Message sublabel = I18n::Message::Default;
  if (dataField != &ElementsDataBase::GroupField) {
    /* The "Group of element" full legend is used in the display selector, but
     * not in the details card. */
    sublabel = dataField->fieldLegend();
  }

  typedCell->subLabel()->setMessage(sublabel);
  int significantDigits =
      Preferences::sharedPreferences->numberOfSignificantDigits();
  typedCell->label()->setLayout(dataField->fieldSymbolLayout());
  typedCell->accessory()->setLayout(dataField->getLayout(z, significantDigits));
}

KDCoordinate DetailsListController::nonMemoizedRowHeight(int j) {
  PhysicalQuantityCell tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, j);
}

const DataField *DetailsListController::DataFieldForRow(int row) {
  constexpr const DataField *k_fields[k_numberOfRows] = {
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
