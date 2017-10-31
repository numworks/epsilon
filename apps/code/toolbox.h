#ifndef CODE_TOOLBOX_H
#define CODE_TOOLBOX_H

#include <escher.h>
#include <apps/i18n.h>
#include <kandinsky/text.h>

namespace Code {

class Toolbox : public ::Toolbox {
public:
  typedef void (*Action)(void * sender, const char * text);
  Toolbox();
  void setAction(Action action);
protected:
  KDCoordinate rowHeight(int j) override;
  bool selectLeaf(ToolboxMessageTree * selectedMessageTree) override;
  const ToolboxMessageTree * rootModel() override;
  MessageTableCellWithMessage * leafCellAtIndex(int index) override;
  MessageTableCellWithChevron* nodeCellAtIndex(int index) override;
  int maxNumberOfDisplayedRows() override;
  constexpr static int k_maxNumberOfDisplayedRows = 13; // = 240/(13+2*3)
  // 13 = minimal string height size
  // 3 = vertical margins
private:
  constexpr static KDCoordinate k_nodeRowHeight = 40;
  constexpr static KDCoordinate k_leafRowHeight = 40;
  constexpr static KDText::FontSize k_fontSize = KDText::FontSize::Small;
  Action m_action;
  MessageTableCellWithMessage m_leafCells[k_maxNumberOfDisplayedRows];
  MessageTableCellWithChevron m_nodeCells[k_maxNumberOfDisplayedRows];
};

}

#endif
