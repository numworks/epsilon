#ifndef APPS_MATH_TOOLBOX_H
#define APPS_MATH_TOOLBOX_H

#define MATRICES_ARE_DEFINED 1
#include <escher.h>
#include <apps/i18n.h>

class MathToolbox : public Toolbox {
public:
  MathToolbox();
  bool handleEvent(Ion::Events::Event event) override;
protected:
  TextField * sender() override;
  bool selectLeaf(ToolboxMessageTree * selectedMessageTree) override;
  const ToolboxMessageTree * rootModel() override;
  MessageTableCellWithMessage * leafCellAtIndex(int index) override;
  MessageTableCellWithChevron* nodeCellAtIndex(int index) override;
  int maxNumberOfDisplayedRows() override;
  constexpr static int k_maxNumberOfDisplayedRows = 6; // = 240/40
private:
  void scrollToAndSelectChild(int i);
  MessageTableCellWithMessage m_leafCells[k_maxNumberOfDisplayedRows];
  MessageTableCellWithChevron m_nodeCells[k_maxNumberOfDisplayedRows];
};

#endif
