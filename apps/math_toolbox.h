#ifndef APPS_MATH_TOOLBOX_H
#define APPS_MATH_TOOLBOX_H

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/toolbox.h>

class MathToolbox : public Escher::Toolbox {
public:
  MathToolbox();
protected:
  bool selectLeaf(int selectedRow) override;
  const Escher::ToolboxMessageTree * rootModel() const override;
  Escher::MessageTableCellWithMessage * leafCellAtIndex(int index) override;
  Escher::MessageTableCellWithChevron* nodeCellAtIndex(int index) override;
  int maxNumberOfDisplayedRows() override;
  constexpr static int k_maxNumberOfDisplayedRows = 6; // = 240/40
private:
  int indexAfterFork() const override;

  Escher::MessageTableCellWithMessage m_leafCells[k_maxNumberOfDisplayedRows];
  Escher::MessageTableCellWithChevron m_nodeCells[k_maxNumberOfDisplayedRows];
};

#endif
