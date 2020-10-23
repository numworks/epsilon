#ifndef CODE_PYTHON_TOOLBOX_H
#define CODE_PYTHON_TOOLBOX_H

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/toolbox.h>
#include <ion/events.h>
#include <kandinsky/font.h>

namespace Code {

class PythonToolbox : public Escher::Toolbox {
public:
  // PythonToolbox
  PythonToolbox();
  const Escher::ToolboxMessageTree * moduleChildren(const char * name, int * numberOfNodes) const;

  // Toolbox
  bool handleEvent(Ion::Events::Event event) override;
protected:
  KDCoordinate rowHeight(int j) override;
  bool selectLeaf(int selectedRow) override;
  const Escher::ToolboxMessageTree * rootModel() const override;
  Escher::MessageTableCellWithMessage * leafCellAtIndex(int index) override;
  Escher::MessageTableCellWithChevron* nodeCellAtIndex(int index) override;
  int maxNumberOfDisplayedRows() override;
  constexpr static int k_maxNumberOfDisplayedRows = 13; // = 240/(13+2*3)
  // 13 = minimal string height size
  // 3 = vertical margins
private:
  constexpr static const KDFont * k_font = KDFont::SmallFont;
  void scrollToLetter(char letter);
  void scrollToAndSelectChild(int i);
  Escher::MessageTableCellWithMessage m_leafCells[k_maxNumberOfDisplayedRows];
  Escher::MessageTableCellWithChevron m_nodeCells[k_maxNumberOfDisplayedRows];
};

}

#endif
