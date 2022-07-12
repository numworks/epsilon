#include "details_list_controller.h"
#include "app.h"
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/print_int.h>
#include <poincare/string_layout.h>
#include <poincare/vertical_offset_layout.h>

using namespace Escher;
using namespace Poincare;

namespace Periodic {

bool DetailsListController::handleEvent(Ion::Events::Event e) {
  // TODO
  return SelectableListViewController::handleEvent(e);
}

void DetailsListController::didBecomeFirstResponder() {
  selectCellAtLocation(0, 0);
  m_selectableTableView.reloadData(true, true);
}

const char * DetailsListController::title() {
  return I18n::translate(ElementsDataBase::Name(App::app()->elementsViewDataSource()->selectedElement()));
}

HighlightCell * DetailsListController::reusableCell(int index, int type) {
  switch (type) {
  case k_bufferCellType:
    assert(index < k_numberOfBufferCells);
    return m_bufferCells + index;
  case k_separatorBufferCellType:
    assert(index < k_numberOfSeparatorBufferCells);
    return m_separatorBufferCells + index;
  case k_layoutTitleCellType:
    assert(index < k_numberOfLayoutTitleCells);
    return m_layoutTitleCells + index;
  default:
    assert(type == k_separatorLayoutCellType && index < k_numberOfSeparatorLayoutCells);
    return m_separatorLayoutCells + index;
  }
}

int DetailsListController::reusableCellCount(int type) {
  switch (type) {
  case k_bufferCellType:
    return k_numberOfBufferCells;
  case k_separatorBufferCellType:
    return k_numberOfSeparatorBufferCells;
  case k_layoutTitleCellType:
    return k_numberOfLayoutTitleCells;
  default:
    assert(type == k_separatorLayoutCellType);
    return k_numberOfSeparatorLayoutCells;
  }
}

int DetailsListController::typeAtIndex(int index) {
  assert(index < numberOfRows());
  Row row = static_cast<Row>(index);
  switch (row) {
  case Row::Z:
  case Row::A:
  case Row::Mass:
  case Row::Electronegativity:
    return k_bufferCellType;
  case Row::Group:
  case Row::Radius:
  case Row::State:
    return k_separatorBufferCellType;
  case Row::MeltingPoint:
  case Row::BoilingPoint:
  case Row::Affinity:
  case Row::Ionisation:
    return k_layoutTitleCellType;
  default:
    assert(row == Row::Configuration || row == Row::Density);
    return k_separatorLayoutCellType;
  }
}

void fillBufferCell(MessageTableCellWithMessageWithBuffer * cell, I18n::Message label, I18n::Message sublabel, const char * accessory) {
  cell->setMessage(label);
  cell->setSubLabelMessage(sublabel);
  cell->setAccessoryText(accessory);
}

void fillBufferCellWithInteger(MessageTableCellWithMessageWithBuffer * cell, I18n::Message label, I18n::Message sublabel, int n, char * buffer, size_t bufferSize) {
  int length = PrintInt::Left(n, buffer, bufferSize);
  assert(length < bufferSize);
  buffer[length] = 0;
  fillBufferCell(cell, label, sublabel, buffer);
}

void fillLayoutTitleCell(InertExpressionTableCell * cell, char symbol, I18n::Message symbolSubscript, I18n::Message sublabel, const char * accessory) {
  Layout layout = HorizontalLayout::Builder(CodePointLayout::Builder(symbol), VerticalOffsetLayout::Builder(StringLayout::Builder(I18n::translate(symbolSubscript)), VerticalOffsetLayoutNode::Position::Subscript));
  cell->setLayout(layout);
  cell->setSubLabelMessage(sublabel);
  cell->setAccessoryText(accessory);
}

void DetailsListController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  AtomicNumber z = App::app()->elementsViewDataSource()->selectedElement();
  assert(ElementsDataBase::IsElement(z));

  constexpr size_t bufferSize = BufferTextView::k_maxNumberOfChar;
  char buffer[bufferSize];

  Row row = static_cast<Row>(index);
  switch (row) {
  case Row::Z:
    fillBufferCellWithInteger(static_cast<MessageTableCellWithMessageWithBuffer *>(cell), I18n::Message::DetailsZSymbol, I18n::Message::DetailsZLegend, z, buffer, bufferSize);
    return;

  case Row::A:
    // TODO Some elements have an undefined number of mass
    fillBufferCellWithInteger(static_cast<MessageTableCellWithMessageWithBuffer *>(cell), I18n::Message::DetailsASymbol, I18n::Message::DetailsALegend, ElementsDataBase::NumberOfMass(z), buffer, bufferSize);
    return;

  case Row::Mass:
    ElementsViewDataSource::ColorByMass.printValueWithUnitForElement(z, buffer, bufferSize);
    fillBufferCell(static_cast<MessageTableCellWithMessageWithBuffer *>(cell), I18n::Message::DetailsMassSymbol, I18n::Message::DetailsMassLegend, buffer);
    return;

  case Row::Configuration:
    {
      MessageTableCellWithMessageWithExpressionWithSeparator * typedCell = static_cast<MessageTableCellWithMessageWithExpressionWithSeparator *>(cell);
      typedCell->setSeparatorPosition(true);
      typedCell->innerCell()->setMessage(I18n::Message::DetailsConfigurationSymbol);
      typedCell->innerCell()->setSubLabelMessage(I18n::Message::DetailsConfigurationLegend);
      // TODO
      typedCell->innerCell()->setAccessoryLayout(Layout());
    }
    return;

  case Row::Electronegativity:
    ElementsViewDataSource::ColorByElectronegativity.printValueWithUnitForElement(z, buffer, bufferSize);
    fillBufferCell(static_cast<MessageTableCellWithMessageWithBuffer *>(cell), I18n::Message::DetailsElectronegativitySymbol, I18n::Message::DetailsElectronegativityLegend, buffer);
    return;

  case Row::Group:
    {
      MessageTableCellWithMessageWithBuffer * typedCell = static_cast<MessageTableCellWithMessageWithBufferWithSeparator *>(cell)->innerCell();
      GroupsDisplayType displayType = ElementsViewDataSource::ColorByGroups;
      typedCell->setAccessoryTextColor(displayType.colorPairForElement(z).fg());
      fillBufferCell(typedCell, I18n::Message::DetailsGroupsLegend, I18n::Message::Default, I18n::translate(displayType.titleForElement(z)));
    }
    return;

  case Row::Radius:
    ElementsViewDataSource::ColorByRadius.printValueWithUnitForElement(z, buffer, bufferSize);
    fillBufferCell(static_cast<MessageTableCellWithMessageWithBufferWithSeparator *>(cell)->innerCell(), I18n::Message::DetailsRadiusSymbol, I18n::Message::DetailsRadiusLegend, buffer);
    return;

  case Row::State:
    fillBufferCell(static_cast<MessageTableCellWithMessageWithBufferWithSeparator *>(cell)->innerCell(), I18n::Message::DetailsStateLegend, I18n::Message::Default, I18n::translate(ElementsViewDataSource::ColorByStates.titleForElement(z)));
    return;

  case Row::MeltingPoint:
    ElementsViewDataSource::ColorByMeltingPoint.printValueWithUnitForElement(z, buffer, bufferSize);
    fillLayoutTitleCell(static_cast<InertExpressionTableCell *>(cell), 'T', I18n::Message::DetailsMeltingPointSubscript, I18n::Message::DetailsMeltingPointLegend, buffer);
    return;

  case Row::BoilingPoint:
    ElementsViewDataSource::ColorByBoilingPoint.printValueWithUnitForElement(z, buffer, bufferSize);
    fillLayoutTitleCell(static_cast<InertExpressionTableCell *>(cell), 'T', I18n::Message::DetailsBoilingPointSubscript, I18n::Message::DetailsBoilingPointLegend, buffer);
    return;

  case Row::Density:
    {
      MessageTableCellWithMessageWithExpressionWithSeparator * typedCell = static_cast<MessageTableCellWithMessageWithExpressionWithSeparator *>(cell);
      typedCell->setSeparatorPosition(false);
      typedCell->innerCell()->setMessage(I18n::Message::DetailsDensitySymbol);
      typedCell->innerCell()->setSubLabelMessage(I18n::Message::DetailsDensityLegend);

      int length = ElementsViewDataSource::ColorByDensity.printValueWithUnitForElement(z, buffer, bufferSize);
      /* Replace '^3' with superscript 3. */
      buffer[length - 2] = '\0';
      Layout layout = LayoutHelper::StringToCodePointsLayout(buffer, length);
      assert(layout.type() == LayoutNode::Type::HorizontalLayout);
      int n = layout.numberOfChildren();
      static_cast<HorizontalLayout &>(layout).addOrMergeChildAtIndex(VerticalOffsetLayout::Builder(CodePointLayout::Builder('3'), VerticalOffsetLayoutNode::Position::Superscript), n, false);
      typedCell->innerCell()->setAccessoryLayout(layout);
    }
    return;

  case Row::Affinity:
    ElementsViewDataSource::ColorByAffinity.printValueWithUnitForElement(z, buffer, bufferSize);
    fillLayoutTitleCell(static_cast<InertExpressionTableCell *>(cell), 'E', I18n::Message::DetailsAffinitySubscript, I18n::Message::DetailsAffinityLegend, buffer);
    return;

  case Row::Ionisation:
    ElementsViewDataSource::ColorByIonisation.printValueWithUnitForElement(z, buffer, bufferSize);
    fillLayoutTitleCell(static_cast<InertExpressionTableCell *>(cell), 'E', I18n::Message::DetailsIonisationSubscript, I18n::Message::DetailsIonisationLegend, buffer);
    return;

  default:
    assert(false);
  }
}

KDCoordinate DetailsListController::nonMemoizedRowHeight(int j) {
  int type = typeAtIndex(j);
  switch (type) {
  case k_bufferCellType:
    {
      MessageTableCellWithMessageWithBuffer tempCell;
      return heightForCellAtIndexWithWidthInit(&tempCell, j);
    }
  case k_separatorBufferCellType:
    {
      MessageTableCellWithMessageWithBufferWithSeparator tempCell;
      return heightForCellAtIndexWithWidthInit(&tempCell, j);
    }
  case k_layoutTitleCellType:
    {
      InertExpressionTableCell tempCell;
      return heightForCellAtIndexWithWidthInit(&tempCell, j);
    }
  default:
    assert(type == k_separatorLayoutCellType);
    {
      MessageTableCellWithMessageWithExpressionWithSeparator tempCell;
      return heightForCellAtIndexWithWidthInit(&tempCell, j);
    }
  }
}

}
