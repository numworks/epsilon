#ifndef CODE_PYTHON_TOOLBOX_H
#define CODE_PYTHON_TOOLBOX_H

#include <apps/i18n.h>
#include <escher.h>
#include <ion/events.h>
#include <kandinsky/font.h>
#include "toolbox_ion_keys.h"

namespace Code {

class PythonToolbox : public Toolbox {
public:
  // PythonToolbox
  PythonToolbox();
  const ToolboxMessageTree * moduleChildren(const char * name, int * numberOfNodes) const;

  // Toolbox
  bool handleEvent(Ion::Events::Event event) override;
  const ToolboxMessageTree * rootModel() const override;

  // ListViewDataSource
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

protected:
  KDCoordinate rowHeight(int j) override;
  bool selectLeaf(int selectedRow, bool quitToolbox) override;
  bool selectSubMenu(int selectedRow) override;
  MessageTableCellWithMessage<SlideableMessageTextView> * leafCellAtIndex(int index) override;
  MessageTableCellWithChevron<SlideableMessageTextView> * nodeCellAtIndex(int index) override;
  int maxNumberOfDisplayedRows() override;
  bool canStayInMenu() override { return true; }
  constexpr static int k_maxNumberOfDisplayedRows = 13; // = 240/(13+2*3)
  // 13 = minimal string height size
  // 3 = vertical margins
private:
  constexpr static const KDFont * k_fontForMultiLine = KDFont::SmallFont;
  void scrollToLetter(char letter);
  void scrollToAndSelectChild(int i);
  MessageTableCellWithMessage<SlideableMessageTextView> m_leafCells[k_maxNumberOfDisplayedRows];
  MessageTableCellWithChevron<SlideableMessageTextView> m_nodeCells[k_maxNumberOfDisplayedRows];
  ToolboxIonKeys m_ionKeys;
};

}

#endif
