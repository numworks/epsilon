#include <escher/container.h>
#include <escher/editable_field.h>
#include <escher/metric.h>
#include <escher/toolbox.h>
#include <omg/utf8_helper.h>
#include <poincare/old/symbol.h>
#include <poincare/xnt.h>

using namespace Poincare;

namespace Escher {

bool EditableField::privateHandleBoxEvent(Ion::Events::Event event) {
  App* app = App::app();
  EditableFieldHelpBox* box = nullptr;
  if (event == Ion::Events::Toolbox) {
    box = app->toolbox();
  } else if (event == Ion::Events::Var) {
    box = app->variableBox();
  }
  if (!box) {
    return false;
  }
  box->setSender(this);
  box->open();
  return true;
}

bool EditableField::handleXNT(int currentIndex, CodePoint startingXNT) {
  if (!prepareToEdit()) {
    return false;
  }
  constexpr int bufferSize = SymbolAbstractNode::k_maxNameSize;
  char buffer[bufferSize];
  size_t cycleSize;
  // Find special XNT
  if (!findXNT(buffer, bufferSize, currentIndex, &cycleSize)) {
    // Use default XNT cycle
    CodePoint xnt = Poincare::XNT::CodePointAtIndexInDefaultCycle(
        currentIndex, startingXNT, &cycleSize);
    UTF8Helper::WriteCodePoint(buffer, bufferSize, xnt);
  }
  if (cycleSize > 1 && currentIndex > 0) {
    removePreviousXNT();
  }
  return handleEventWithText(buffer, false, true);
}

size_t EditableField::getTextFromEvent(Ion::Events::Event event, char* buffer,
                                       size_t bufferSize) {
  if (event == Ion::Events::Log &&
      Poincare::Preferences::SharedPreferences()->logarithmKeyEvent() ==
          Poincare::Preferences::LogarithmKeyEvent::WithBaseTen) {
    return strlcpy(buffer, k_logWithBase10, bufferSize);
  }
  return Ion::Events::copyText(static_cast<uint8_t>(event), buffer, bufferSize);
}

}  // namespace Escher
