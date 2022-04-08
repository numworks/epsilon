#ifndef _READ_BOOK_CONTROLLER_H_
#define _READ_BOOK_CONTROLLER_H_

#include <escher.h>
#include "apps/external/archive.h"
#include "word_wrap_view.h"

namespace Reader {

class ReadBookController : public ViewController {
public:
  ReadBookController(Responder * parentResponder);
  View * view() override;
  void setBook(const External::Archive::File& file, bool isRichTextFile);
  bool handleEvent(Ion::Events::Event event) override;
  void viewDidDisappear() override;
  void savePosition() const;
  void loadPosition();
  void throwError();
private:
  WordWrapTextView m_readerView;
  const External::Archive::File* m_file;
};

}

#endif
