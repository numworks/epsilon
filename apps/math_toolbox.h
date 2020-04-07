#ifndef APPS_MATH_TOOLBOX_H
#define APPS_MATH_TOOLBOX_H

#include <escher.h>
#include <apps/i18n.h>

class MathToolbox : public Toolbox {
public:
  MathToolbox();
protected:
  virtual bool selectLeaf(int selectedRow) override;
  virtual const ToolboxMessageTree * rootModel() const override;
  virtual MessageTableCellWithMessage * leafCellAtIndex(int index) override;
  virtual MessageTableCellWithChevron * nodeCellAtIndex(int index) override;
  virtual int maxNumberOfDisplayedRows() override;
  constexpr static int k_maxNumberOfDisplayedRows = 6; // = 240/40
private:
  MessageTableCellWithMessage m_leafCells[k_maxNumberOfDisplayedRows];
  MessageTableCellWithChevron m_nodeCells[k_maxNumberOfDisplayedRows];
};

#endif
