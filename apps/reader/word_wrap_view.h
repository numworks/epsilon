#ifndef _WORD_WRAP_VIEW_H_
#define _WORD_WRAP_VIEW_H_

#include <escher.h>

namespace reader
{

class WordWrapTextView : public PointerTextView {
public:

  void drawRect(KDContext * ctx, KDRect rect) const override;
  
protected:
  
};

}

#endif