#include <escher/container.h>
#include <escher/editable_field.h>
#include <escher/metric.h>
#include <escher/toolbox.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/symbol.h>
#include <poincare/old/xnt_helpers.h>

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
    CodePoint xnt = XNTHelpers::CodePointAtIndexInDefaultCycle(
        currentIndex, startingXNT, &cycleSize);
    SerializationHelper::CodePoint(buffer, bufferSize, xnt);
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
