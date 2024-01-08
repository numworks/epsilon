#include <escher/container.h>
#include <escher/editable_field.h>
#include <escher/metric.h>
#include <escher/toolbox.h>
#include <poincare/serialization_helper.h>
#include <poincare/symbol.h>

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

bool EditableField::insertXNT(CodePoint defaultXNTCodePoint) {
  if (!prepareToEdit()) {
    return false;
  }
  constexpr int bufferSize = SymbolAbstractNode::k_maxNameSize;
  char buffer[bufferSize];
  findXNT(buffer, bufferSize);
  if (strlen(buffer) == 0) {
    SerializationHelper::CodePoint(buffer, bufferSize, defaultXNTCodePoint);
    if (Ion::Events::repetitionFactor() > 0) {
      // TODO: Fix issues with repetition over a syntax error dismissal
      removePreviousXNT();
    }
  }
  return handleEventWithText(buffer, false, true);
}

}  // namespace Escher
