#ifndef APPS_MATH_TOOLBOX_H
#define APPS_MATH_TOOLBOX_H

#define MATRICES_ARE_DEFINED 1
#include <escher.h>
#include <apps/i18n.h>
#include <apps/expression_editor/controller.h>

class MathToolbox : public Toolbox {
public:
  typedef void (*Action)(void * sender, const char * text, bool removeArguments);
  MathToolbox();
  void setSenderAndAction(Responder * sender, Action action);
  static void actionForScrollableExpressionViewWithCursor(void * sender, const char * text, bool removeArguments = true);
  static void actionForTextField(void * sender, const char * text, bool removeArguments = true);
protected:
  bool selectLeaf(ToolboxMessageTree * selectedMessageTree) override;
  const ToolboxMessageTree * rootModel() override;
  MessageTableCellWithMessage * leafCellAtIndex(int index) override;
  MessageTableCellWithChevron* nodeCellAtIndex(int index) override;
  int maxNumberOfDisplayedRows() override;
  constexpr static int k_maxNumberOfDisplayedRows = 6; // = 240/40
  Action m_action;
private:
  MessageTableCellWithMessage m_leafCells[k_maxNumberOfDisplayedRows];
  MessageTableCellWithChevron m_nodeCells[k_maxNumberOfDisplayedRows];
};

#endif
