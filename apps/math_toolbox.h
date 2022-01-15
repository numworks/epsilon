#ifndef APPS_MATH_TOOLBOX_H
#define APPS_MATH_TOOLBOX_H

#include <escher.h>
#include <apps/i18n.h>

class MathToolbox : public Toolbox {
public:
  MathToolbox();
  const ToolboxMessageTree * rootModel() const override;
protected:
  bool selectLeaf(int selectedRow, bool quitToolbox) override;
  MessageTableCellWithMessage * leafCellAtIndex(int index) override;
  MessageTableCellWithChevron* nodeCellAtIndex(int index) override;
  int maxNumberOfDisplayedRows() override;
  constexpr static int k_maxNumberOfDisplayedRows = 6; // = 240/40
private:
  int indexAfterFork() const override;

  MessageTableCellWithMessage m_leafCells[k_maxNumberOfDisplayedRows];
  MessageTableCellWithChevron m_nodeCells[k_maxNumberOfDisplayedRows];
};

#endif
