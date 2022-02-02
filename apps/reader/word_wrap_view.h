#ifndef _WORD_WRAP_VIEW_H_
#define _WORD_WRAP_VIEW_H_

#include <apps/global_preferences.h>
#include <escher.h>

namespace Reader
{

struct BookSave {
  int offset;
  KDColor color;
};

class WordWrapTextView : public PointerTextView {
public:
  WordWrapTextView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setText(const char*, int length, bool isRichTextFile);
  void nextPage();
  void previousPage();
  BookSave getBookSave() const;
  void setBookSave(BookSave save);
private:
  void richTextPreviousPage();
  void plainTextPreviousPage();
  void richTextDrawRect(KDContext * ctx, KDRect rect) const;
  void plainTextDrawRect(KDContext * ctx, KDRect rect) const;
  bool updateTextColorForward(const char * colorStart) const;
  bool updateTextColorBackward(const char * colorStart) const;
  static const int k_margin = 10;
  static const int k_lastOffsetsBufferSize = 10;
  int m_pageOffset;
  mutable int m_nextPageOffset;
  int m_length;
  bool m_isRichTextFile;
  mutable KDColor m_textColor;
  /*
   * Beacause the text that we draw can be of different sizes, we can't
   * exactly know where the last page starts.
   * So we store into a buffer (a cyclic stack) the offsets of the last pages.
   */
  int m_lastPagesOffsets[k_lastOffsetsBufferSize];
  int m_lastPagesOffsetsIndex;
};

}

#endif
