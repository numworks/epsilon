#ifndef ESCHER_LAYOUT_FIELD_DELEGATE_H //
#define ESCHER_LAYOUT_FIELD_DELEGATE_H //
 //
#include <escher/context_provider.h> //
#include <ion/events.h> //
#include <poincare/layout.h> //
 //
namespace Escher { //
 //
class LayoutField; //
 //
class LayoutFieldDelegate : public ContextProvider { //
 public: //
  bool layoutFieldShouldFinishEditing(LayoutField* layoutField, //
                                      Ion::Events::Event event) { //
    return event == Ion::Events::OK || event == Ion::Events::EXE; //
  } //
  virtual bool layoutFieldDidReceiveEvent(LayoutField* layoutField, //
                                          Ion::Events::Event event) { //
    return false; //
  } //
  virtual bool layoutFieldDidFinishEditing(LayoutField* layoutField, //
                                           Ion::Events::Event event) { //
    return false; //
  } //
  virtual void layoutFieldDidAbortEditing(LayoutField* layoutField) {} //
  virtual void layoutFieldDidHandleEvent(LayoutField* layoutField) {} //
  virtual void layoutFieldDidChangeSize(LayoutField* layoutField) {} //
 //
  virtual bool shouldInsertTextForStoEvent( //
      Escher::LayoutField* layoutField) const { //
    return false; //
  } //
  virtual bool shouldInsertTextForAnsEvent( //
      Escher::LayoutField* layoutField) const { //
    return false; //
  } //
  virtual bool shouldInsertSingleQuoteInsteadOfDoubleQuotes( //
      LayoutField* layoutField) { //
    return true; //
  } //
  virtual void updateRepetitionIndexes(LayoutField* layoutField, //
                                       Ion::Events::Event event) {} //
}; //
 //
}  // namespace Escher //
 //
#endif //
