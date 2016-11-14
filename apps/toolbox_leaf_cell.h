#ifndef APPS_TOOLBOX_LEAF_CELL_H
#define APPS_TOOLBOX_LEAF_CELL_H

#include <escher.h>

class ToolboxLeafCell : public TableViewCell {
public:
  ToolboxLeafCell();
  void reloadCell() override;
  void setLabel(const char * text);
  void setSubtitle(const char * text);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  PointerTextView m_labelView;
  PointerTextView m_subtitleView;
};

#endif
