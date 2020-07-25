#ifndef CODE_PYTHON_TOOLBOX_H
#define CODE_PYTHON_TOOLBOX_H

#include <apps/i18n.h>
#include <escher.h>
#include <ion/events.h>
#include <kandinsky/font.h>

namespace Code {

class PythonToolbox : public Toolbox {
public:
  // PythonToolbox
  PythonToolbox();
  const ToolboxMessageTree * moduleChildren(const char * name, int * numberOfNodes) const;

  // Toolbox
  bool handleEvent(Ion::Events::Event event) override;
protected:
  KDCoordinate rowHeight(int j) override;
  bool selectLeaf(int selectedRow) override;
  const ToolboxMessageTree * rootModel() const override;
  MessageTableCellWithMessage * leafCellAtIndex(int index) override;
  MessageTableCellWithChevron* nodeCellAtIndex(int index) override;
  int maxNumberOfDisplayedRows() override;
  constexpr static int k_maxNumberOfDisplayedRows = 13; // = 240/(13+2*3)
  // 13 = minimal string height size
  // 3 = vertical margins
private:
  constexpr static const KDFont * k_font = KDFont::SmallFont;
  void scrollToLetter(char letter);
  void scrollToAndSelectChild(int i);
  MessageTableCellWithMessage m_leafCells[k_maxNumberOfDisplayedRows];
  MessageTableCellWithChevron m_nodeCells[k_maxNumberOfDisplayedRows];
};

}

#endif
