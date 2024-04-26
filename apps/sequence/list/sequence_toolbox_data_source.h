#ifndef SEQUENCE_SEQUENCE_TOOLBOX_H
#define SEQUENCE_SEQUENCE_TOOLBOX_H

#include <apps/shared/math_toolbox_controller.h>
#include <apps/shared/sequence_store.h>
#include <escher/layout_view.h>
#include <escher/menu_cell.h>
#include <poincare/old/layout.h>

namespace Sequence {

class SequenceToolboxDataSource
    : public Shared::MathToolboxExtraCellsDataSource {
 public:
  SequenceToolboxDataSource() : m_numberOfAddedCells(0) {}
  int numberOfExtraCells() override {
    assert(m_numberOfAddedCells <= k_maxNumberOfLayouts);
    return m_numberOfAddedCells;
  }
  Poincare::Layout extraCellLayoutAtRow(int row) override {
    assert(row < numberOfExtraCells());
    return m_addedCellLayout[row];
  }
  void buildExtraCellsLayouts(const char* sequenceName, int order);

 private:
  constexpr static int k_maxNumberOfLayouts =
      2 * Shared::SequenceStore::k_maxNumberOfSequences;
  Poincare::Layout m_addedCellLayout[k_maxNumberOfLayouts];
  int m_numberOfAddedCells;
};

}  // namespace Sequence

#endif
