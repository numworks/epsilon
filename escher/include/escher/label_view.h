#ifndef ESCHER_LABEL_VIEW_H
#define ESCHER_LABEL_VIEW_H

#include <escher/childless_view.h>
#include <kandinsky/color.h>

class LabelView : public ChildlessView {

public:
  LabelView(const char * label, KDColor backgroundColor = KDColorWhite, KDColor textColor = KDColorBlack);
  void setBackgroundColor(KDColor backgroundColor);
  void setTextColor(KDColor textColor);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  const char * m_label;
  KDColor m_backgroundColor;
  KDColor m_textColor;
};

#endif
