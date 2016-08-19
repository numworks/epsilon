#ifndef ESCHER_STACK_VIEW_H
#define ESCHER_STACK_VIEW_H

#include <escher/childless_view.h>

class StackView : public ChildlessView {
public:
  StackView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setName(const char * name);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
  void logAttributes(std::ostream &os) const override;
#endif
private:
  const char * m_name;
};

#endif
