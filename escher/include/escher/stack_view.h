#ifndef ESCHER_STACK_VIEW_H
#define ESCHER_STACK_VIEW_H

#include <escher/view.h>
#include <escher/pointer_text_view.h>

class StackView : public View {
public:
  StackView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setName(const char * name);
  void setTextColor(KDColor textColor);
  void setBackgroundColor(KDColor backgroundColor);
  void setSeparatorColor(KDColor separatorColor);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
  void logAttributes(std::ostream &os) const override;
#endif
private:
  KDColor m_backgroundColor;
  KDColor m_separatorColor;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  PointerTextView m_textView;
};

#endif
