#ifndef _WORD_WRAP_VIEW_H_
#define _WORD_WRAP_VIEW_H_

#include <apps/global_preferences.h>
#include <escher.h>

namespace Reader
{

class WordWrapTextView : public PointerTextView {
public:
  WordWrapTextView() : PointerTextView(GlobalPreferences::sharedGlobalPreferences()->font()) {};
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setText(const char*, int length);
  void nextPage();
  void previousPage();
  int getPageOffset() const;
  void setPageOffset(int o);
protected:
  int m_pageOffset = 0;
  mutable int m_nextPageOffset = 0;
  int m_length = 0;
  static const int k_margin = 10;
};

}

#endif