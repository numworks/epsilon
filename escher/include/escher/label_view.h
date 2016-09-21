#ifndef ESCHER_LABEL_VIEW_H
#define ESCHER_LABEL_VIEW_H

#include <escher/childless_view.h>
#include <kandinsky/color.h>

class LabelView : public ChildlessView {

public:
  LabelView(const char * label, KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  void setBackgroundColor(KDColor backgroundColor);
  void setTextColor(KDColor textColor);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  const char * m_label;
  KDColor m_textColor;
  KDColor m_backgroundColor;
};

#endif
